//
// Created by Hong Zhang on 2022/10/30.
//

#include "core/include/hittable.h"

namespace ALICE_TRACER{
    BoundingBox::BoundingBox(AVec3 b_min, AVec3 b_max): b_min_(b_min), b_max_(b_max) {
    }

    Hittable::Hittable(Material *mtl, BxDFBase *bxdf): mtl_(mtl), bxdf_(bxdf) {

    }

    // ------------------------------------------------------------
    // The Hittable Objects/Surfaces/Volume
    // ------------------------------------------------------------
    Hittable::Hittable(Material *mtl, BxDFBase *bxdf, Movement * movement): mtl_(mtl), bxdf_(bxdf), movement_(movement) {

    }

//    void Hittable::translate(AVec3 offset) {
//        transform_mat_ = ATranslate(transform_mat_, -offset);
//    }
//
//    void Hittable::scale(AVec3 scale) {
//        transform_mat_ = AScale(transform_mat_, 1.f/scale);
//    }
//
//    void Hittable::rotate(float angle, AVec3 axis) {
//        rot_mat_ = ARotate(rot_mat_, -angle, axis);
//        transform_mat_ = ARotate(transform_mat_, -angle, axis);
//    }

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

    AVec3 Sphere::getNormal(AVec3 &point) {
        return  ANormalize(point - center_);
    }

    AVec3 Sphere::center(float frame_time) {
        if(movement_){
            return movement_->movementFunc(center_, frame_time);
        }
        return center_;
    }

    float Sphere::CheckHittable(Ray & ray) {
        AVec3 oc = ray.start_ - center(ray.fm_t_);
        float a = dot(ray.dir_, ray.dir_);
        float b = 2.f * dot(oc, ray.dir_);
        float c = dot(oc, oc) - radius_ * radius_;
        float discriminant = b*b - 4.f * a * c;
        if (discriminant < MIN_THRESHOLD) {
            return -1.f;
        } else {
            return (- b - sqrt(discriminant) ) / (2.f * a);
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

    AVec3 RectangleXY::getNormal(AVec3 &point) {
        return norm_;
    }

    AVec3 RectangleXY::cornel(float frame_time){
        if(movement_){
            return movement_->movementFunc(l_b_, frame_time);
        }
        return l_b_;
    }

    float RectangleXY::CheckHittable(Ray &ray) {
        float ldotn = ADot(ray.dir_, norm_);
        if(abs(ldotn) < MIN_THRESHOLD)
            return MAXFLOAT;
        float time = ADot(cornel(ray.fm_t_) - ray.start_, norm_)/ ldotn;
        AVec3 point = ray.start_ + time * ray.dir_ - cornel(ray.fm_t_);
        float width = ADot(point, u_w_);
        float height = ADot(point, v_h_);
        if(width >= 0.f && width < area_.x && height >= 0.f && height < area_.y)
            return time;
        return MAXFLOAT;
    }


}