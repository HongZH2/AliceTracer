//
// Created by Hong Zhang on 2022/10/30.
//

#include "core/include/hittable.h"
#include "core/include/random_variable.h"

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

    void AABB::overlapAABB(AABB &bbox1, AABB &bbox2) {
        b_min_ = max(bbox1.b_min_, bbox2.b_min_);
        b_max_ = min(bbox1.b_max_, bbox2.b_max_);
    }

    void AABB::unionAABB(AABB &bbox1, AABB &bbox2) {
        b_min_ = min(bbox1.b_min_, bbox2.b_min_);
        b_max_ = max(bbox1.b_max_, bbox2.b_max_);
    }

    bool AABB::isOverlap(AABB &bbox1, AABB &bbox2) {
        AVec3 temp =  min(bbox1.b_max_, bbox2.b_max_) - max(bbox1.b_min_, bbox2.b_min_);
        if( temp.x > MIN_THRESHOLD && temp.y > MIN_THRESHOLD && temp.z > MIN_THRESHOLD){
            return true;
        }
        return false;
    }

    // ------------------------------------------------------------
    // The Hittable Objects/Surfaces/Volume
    // ------------------------------------------------------------
    Hittable::Hittable(){
        bound_ = new AABB();
        id_ = UniID();
    }

    Hittable::Hittable(Material *mtl, BxDFBase *bxdf): mtl_(mtl), bxdf_(bxdf) {
        bound_ = new AABB();
        id_ = UniID();
    }

    Hittable::Hittable(Material *mtl, BxDFBase *bxdf, Movement * movement): mtl_(mtl), bxdf_(bxdf), movement_(movement) {
        bound_ = new AABB();
        id_ = UniID();
    }

    Hittable::~Hittable() {
        delete bound_;
    }

    AABB *Hittable::boundLimit(float frame_time) {
        return bound_;
    }

    // --------------------
    // -- Sphere
    // --------------------
    Sphere::Sphere(AVec3 center, float radius, Material *mtl, BxDFBase *bxdf)
            : Hittable(mtl, bxdf), center_(center), radius_(radius){
    }
    Sphere::Sphere(Material *mtl, BxDFBase *bxdf)
            : Hittable(mtl, bxdf){
    }

    Sphere::Sphere(Material *mtl, BxDFBase *bxdf, Movement * movement)
            : Hittable(mtl, bxdf, movement){
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
        // 1. transform to world coordinate
        // 2. check intersection
        // 3. update hit result
        // 4. transform back to the original coordinate
        AVec3 c_center = center(ray.fm_t_);
        AVec3 oc = ray.start_ - c_center;
        float a = dot(ray.dir_, ray.dir_);
        float b = 2.f * dot(oc, ray.dir_);
        float c = dot(oc, oc) - radius_ * radius_;
        float discriminant = b*b - 4.f * a * c;
        if (discriminant > MIN_THRESHOLD) {
            float sqrt_d = sqrt(discriminant)/(2.f * a);
            float b_part = - b / (2.f * a);
            float t1 = b_part - sqrt_d;
            float t2 = b_part + sqrt_d;  // TODO

            if(t1 < ray.t_max_ && t1 > ray.t_min_) {
                ray.t_max_ = t1;
                hit_res.uni_id_ = id_;
                hit_res.is_hit_ = true;
                hit_res.mtl_ = mtl_;
                hit_res.bxdf_ = bxdf_;
                hit_res.point_ = ray.start_ + ray.dir_ * t1;
                hit_res.setNormal(ANormalize(hit_res.point_ - c_center), ray.dir_);
                hit_res.frame_time_ = ray.fm_t_;
                return true;
            }
        }
        return false;
    }

    // --------------------
    // -- Rectangle
    // --------------------
    RectangleXY::RectangleXY(AVec3 center, AVec2 scale, Material *mtl, BxDFBase *bxdf):
            Hittable(mtl, bxdf), center_(center), area_(scale){

    }

    RectangleXY::RectangleXY(Material *mtl, BxDFBase *bxdf):
            Hittable(mtl, bxdf){
    }

    RectangleXY::RectangleXY(Material *mtl, BxDFBase *bxdf, Movement * movement):
            Hittable(mtl, bxdf, movement){
    }

    AABB * RectangleXY::boundLimit(float frame_time) {
        AVec3 c_center = center(frame_time);
        bound_->b_min_ = c_center - AVec3(area_/2.f, 0.f) - AVec3(AABB_PADDING);
        bound_->b_max_ = c_center + AVec3(area_/2.f, 0.f) + AVec3(AABB_PADDING);
        return bound_;
    }

    AVec3 RectangleXY::center(float frame_time){
        if(movement_){
            return movement_->movementFunc(center_, frame_time);
        }
        return center_;
    }

    bool RectangleXY::CheckHittable(Ray &ray, HitRes & hit_res) {
        AVec3 normal = AVec3(0.f, 0.f, 1.f);
        AVec3 c_center = center(ray.fm_t_);
        AVec2 lb = AVec2(c_center) - area_/2.f;
        AVec2 rt = AVec2(c_center) + area_/2.f;

        float ldotn = ADot(ray.dir_, normal);
        if(abs(ldotn) < MIN_THRESHOLD)
            return false;
        float time = ADot(c_center - ray.start_, normal)/ ldotn;
        AVec3 point = ray.start_ + time * ray.dir_;
        if(point.x > lb[0] && point.x < rt[0] && point.y > lb[1] && point.y < rt[1]){
            if(time < ray.t_max_ && time > ray.t_min_) {
                ray.t_max_ = time;
                hit_res.uni_id_ = id_;
                hit_res.is_hit_ = true;
                hit_res.mtl_ = mtl_;
                hit_res.bxdf_ = bxdf_;
                hit_res.point_ = ray.start_ + ray.dir_ * time;
                hit_res.setNormal(normal, ray.dir_);
                hit_res.frame_time_ = ray.fm_t_;
                return true;
            }
        }
        return false;
    }

    // ---------------
    // -- RectangleXZ --
    // ---------------
    RectangleXZ::RectangleXZ(AVec3 center, AVec2 scale, Material *mtl, BxDFBase *bxdf):
            Hittable(mtl, bxdf), center_(center), area_(scale){

    }

    RectangleXZ::RectangleXZ(Material *mtl, BxDFBase *bxdf):
            Hittable(mtl, bxdf){
    }

    RectangleXZ::RectangleXZ(Material *mtl, BxDFBase *bxdf, Movement * movement):
            Hittable(mtl, bxdf, movement){
    }

    AABB * RectangleXZ::boundLimit(float frame_time) {
        AVec3 c_center = center(frame_time);
        bound_->b_min_ = c_center - AVec3(area_[0]/2.f, 0.f, area_[1]/2.f) - AVec3(AABB_PADDING);
        bound_->b_max_ = c_center + AVec3(area_[0]/2.f, 0.f, area_[1]/2.f) + AVec3(AABB_PADDING);
        return bound_;
    }

    AVec3 RectangleXZ::center(float frame_time){
        if(movement_){
            return movement_->movementFunc(center_, frame_time);
        }
        return center_;
    }

    bool RectangleXZ::CheckHittable(Ray &ray, HitRes & hit_res) {
        AVec3 normal = AVec3(0.f, 1.f, 0.f);
        AVec3 c_center = center(ray.fm_t_);
        AVec2 lb = AVec2(c_center.x, c_center.z) - area_/2.f;
        AVec2 rt = AVec2(c_center.x, c_center.z) + area_/2.f;

        float ldotn = ADot(ray.dir_, normal);
        if(abs(ldotn) < MIN_THRESHOLD)
            return false;
        float time = ADot(c_center - ray.start_, normal)/ ldotn;
        AVec3 point = ray.start_ + time * ray.dir_;
        if(point.x > lb[0] && point.x < rt[0] && point.z > lb[1] && point.z < rt[1]){
            if(time < ray.t_max_ && time > ray.t_min_) {
                ray.t_max_ = time;
                hit_res.uni_id_ = id_;
                hit_res.is_hit_ = true;
                hit_res.mtl_ = mtl_;
                hit_res.bxdf_ = bxdf_;
                hit_res.point_ = ray.start_ + ray.dir_ * time;
                hit_res.setNormal(normal, ray.dir_);
                hit_res.frame_time_ = ray.fm_t_;
                return true;
            }
        }
        return false;
    }
    // ---------------
    // -- RectangleYZ --
    // ---------------
    RectangleYZ::RectangleYZ(AVec3 center, AVec2 scale, Material *mtl, BxDFBase *bxdf):
            Hittable(mtl, bxdf), center_(center), area_(scale){

    }

    RectangleYZ::RectangleYZ(Material *mtl, BxDFBase *bxdf):
            Hittable(mtl, bxdf){
    }

    RectangleYZ::RectangleYZ(Material *mtl, BxDFBase *bxdf, Movement * movement):
            Hittable(mtl, bxdf, movement){
    }

    AABB * RectangleYZ::boundLimit(float frame_time) {
        AVec3 c_center = center(frame_time);
        bound_->b_min_ = c_center - AVec3(0.f, area_[0]/2.f, area_[1]/2.f) - AVec3(AABB_PADDING);
        bound_->b_max_ = c_center + AVec3(0.f, area_[0]/2.f, area_[1]/2.f) + AVec3(AABB_PADDING);
        return bound_;
    }

    AVec3 RectangleYZ::center(float frame_time){
        if(movement_){
            return movement_->movementFunc(center_, frame_time);
        }
        return center_;
    }

    bool RectangleYZ::CheckHittable(Ray &ray, HitRes & hit_res) {
        AVec3 normal = AVec3(1.f, 0.f, 0.f);
        AVec3 c_center = center(ray.fm_t_);
        AVec2 lb = AVec2(c_center.y, c_center.z) - area_/2.f;
        AVec2 rt = AVec2(c_center.y, c_center.z) + area_/2.f;

        float ldotn = ADot(ray.dir_, normal);
        if(abs(ldotn) < MIN_THRESHOLD)
            return false;
        float time = ADot(c_center - ray.start_, normal)/ ldotn;
        AVec3 point = ray.start_ + time * ray.dir_;
        if(point.y > lb[0] && point.y < rt[0] && point.z > lb[1] && point.z < rt[1]){
            if(time < ray.t_max_ && time > ray.t_min_) {
                ray.t_max_ = time;
                hit_res.uni_id_ = id_;
                hit_res.is_hit_ = true;
                hit_res.mtl_ = mtl_;
                hit_res.bxdf_ = bxdf_;
                hit_res.point_ = ray.start_ + ray.dir_ * time;
                hit_res.setNormal(normal, ray.dir_);
                hit_res.frame_time_ = ray.fm_t_;
                return true;
            }
        }
        return false;
    }

}