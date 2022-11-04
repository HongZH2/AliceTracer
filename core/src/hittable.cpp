//
// Created by Hong Zhang on 2022/10/30.
//

#include "core/include/hittable.h"

namespace ALICE_TRACER{
    // ------------------------------------------------------------
    // Axis Aligned Bounding Box
    // ------------------------------------------------------------
    AABB::AABB(AVec3 b_min, AVec3 b_max): b_min_(b_min), b_max_(b_max){

    }

    // cite: ray tracing in one week series
    bool AABB::isHit(Ray &ray) const {
        float t_min = MIN_RAY_TIME;
        float t_max = MAX_RAY_TIME;
        for(int32_t i = 0; i < 3; ++i){
            float inv_d = 1.f / ray.dir_[i];
            float t0 = (b_min_[i] - ray.start_[i]) * inv_d;
            float t1 = (b_max_[i] - ray.start_[i]) * inv_d;
            if(inv_d < 0.f){
                std::swap(t0, t1);
            }
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }
        return true;
    }

    bool AABB::isInside(AABB &bbox1) const {
        AVec3 temp_min = b_min_ - bbox1.b_min_;
        AVec3 temp_max = b_max_ - bbox1.b_max_;
        if(temp_min.x > MIN_THRESHOLD
            && temp_min.y > MIN_THRESHOLD
            && temp_min.z > MIN_THRESHOLD
            && temp_max.x > MIN_THRESHOLD
            && temp_max.y > MIN_THRESHOLD
            && temp_max.z > MIN_THRESHOLD)
            return true;
        return false;
    }

    bool AABB::isOverlap(AABB &bbox1, AABB &bbox2) {
        AVec3 temp =  min(bbox1.b_max_, bbox2.b_max_) - max(bbox1.b_min_, bbox2.b_min_);
        if( temp.x > MIN_THRESHOLD && temp.y > MIN_THRESHOLD && temp.z > MIN_THRESHOLD){
            return true;
        }
        return false;
    }

    AABB AABB::overlapAABB(AABB &bbox1, AABB &bbox2) {
        return {max(bbox1.b_min_, bbox2.b_min_), min(bbox1.b_max_, bbox2.b_max_)};
    }

    AABB AABB::unionAABB(AABB &bbox1, AABB &bbox2) {
        return {min(bbox1.b_min_, bbox2.b_min_), max(bbox1.b_max_, bbox2.b_max_)};
    }

    // ------------------------------------------------------------
    // The Hittable Objects/Surfaces/Volume
    // ------------------------------------------------------------
    Hittable::Hittable(Material *mtl, BxDFBase *bxdf): mtl_(mtl), bxdf_(bxdf) {
        bound_ = new AABB();
    }


    Hittable::Hittable(Material *mtl, BxDFBase *bxdf, Movement * movement): mtl_(mtl), bxdf_(bxdf), movement_(movement) {
        delete bound_;
    }

    AABB *Hittable::boundLimit(float frame_time) {
        return bound_;
    }

    // --------------------
    // -- Rectangle
    // --------------------

    Sphere::Sphere(Material *mtl, BxDFBase *bxdf)
            : Hittable(mtl, bxdf){
    }

    Sphere::Sphere(Material *mtl, BxDFBase *bxdf, Movement * movement)
            : Hittable(mtl, bxdf, movement){
    }

    void Sphere::translate(AVec3 offset) {
        center_ += offset;
    }

    void Sphere::scale(AVec3 scale) {
        radius_ = (scale.x + scale.y + scale.z)/3.f;
    }

    void Sphere::rotate(float angle, AVec3 axis) {
        // TODO: Rotate Texture
        return;
    }

    AVec3 Sphere::center(float frame_time) {
        if(movement_){
            return movement_->movementFunc(center_, frame_time);
        }
        return center_;
    }

    AABB *Sphere::boundLimit(float frame_time) {
        AVec3 c_center = center(frame_time);
        bound_->b_min_ = c_center - AVec3(radius_);
        bound_->b_max_ = c_center + AVec3(radius_);
        return bound_;
    }

    bool Sphere::CheckHittable(Ray & ray, HitRes & hit_res) {
        AVec3 c_center = center(ray.fm_t_);
        AVec3 oc = ray.start_ - c_center;
        float a = dot(ray.dir_, ray.dir_);
        float b = 2.f * dot(oc, ray.dir_);
        float c = dot(oc, oc) - radius_ * radius_;
        float discriminant = b*b - 4.f * a * c;
        if (discriminant < MIN_THRESHOLD) {
            return false;
        } else {
            float t = (- b - sqrt(discriminant) ) / (2.f * a);
            hit_res.is_hit_ = true;
            hit_res.mtl_ = mtl_;
            hit_res.bxdf_ = bxdf_;
            hit_res.point_ = ray.start_ + ray.dir_ * t;
            hit_res.normal_ = ANormalize(hit_res.point_ - c_center);
            hit_res.frame_time_ = ray.fm_t_;
            return true;
        }
    }

    // --------------------
    // -- Rectangle
    // --------------------
    RectangleXY::RectangleXY(Material *mtl, BxDFBase *bxdf):
            Hittable(mtl, bxdf){
    }

    RectangleXY::RectangleXY(Material *mtl, BxDFBase *bxdf, Movement * movement):
            Hittable(mtl, bxdf, movement){
    }

    void RectangleXY::translate(AVec3 offset) {
        l_b_ += offset;
    }

    void RectangleXY::rotate(float angle, AVec3 axis) {
        l_b_ = ARotate(l_b_, angle, axis);
        u_w_ = ARotate(u_w_, angle, axis);
        v_h_ = ARotate(v_h_, angle, axis);
        norm_ = ARotate(norm_, angle, axis);
    }

    void RectangleXY::scale(ALICE_UTILS::AVec3 scale) {
        l_b_ *= abs(scale);
        area_ *= abs(AVec2(scale));
    }

    AABB * RectangleXY::boundLimit(float frame_time) {
        AVec3 c_cornel = cornel(frame_time);
        AVec3 b_min = AVec3(FLT_MAX);
        AVec3 b_max = AVec3(-FLT_MAX);
        AVec3 lt = c_cornel + area_.y * v_h_;
        b_min = min(b_min, lt);
        b_max = min(b_max, lt);
        AVec3 rb = c_cornel + area_.x * u_w_;
        b_min = min(b_min, rb);
        b_max = min(b_max, rb);
        AVec3 rt = lt + area_.x * u_w_;
        b_min = min(b_min, rt);
        b_max = min(b_max, rt);
        bound_->b_min_ = b_min - AVec3(AABB_PADDING);
        bound_->b_max_ = b_max + AVec3(AABB_PADDING);
        return bound_;
    }

    AVec3 RectangleXY::cornel(float frame_time){
        if(movement_){
            return movement_->movementFunc(l_b_, frame_time);
        }
        return l_b_;
    }

    bool RectangleXY::CheckHittable(Ray &ray, HitRes & hit_res) {  // TODO: fix the precision issue
        float ldotn = ADot(ray.dir_, norm_);
        if(abs(ldotn) < MIN_THRESHOLD)
            return false;
        float time = ADot(cornel(ray.fm_t_) - ray.start_, norm_)/ ldotn;
        AVec3 point = ray.start_ + time * ray.dir_ - cornel(ray.fm_t_);
        float width = ADot(point, u_w_);
        float height = ADot(point, v_h_);
        if(width >= 0.f && width < area_.x && height >= 0.f && height < area_.y){
            hit_res.is_hit_ = true;
            hit_res.mtl_ = mtl_;
            hit_res.bxdf_ = bxdf_;
            hit_res.point_ = ray.start_ + ray.dir_ * time;
            hit_res.normal_ = norm_;
            hit_res.frame_time_ = ray.fm_t_;
        }
        return false;
    }


}