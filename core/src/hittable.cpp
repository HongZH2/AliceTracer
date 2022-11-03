//
// Created by Hong Zhang on 2022/10/30.
//

#include "core/include/hittable.h"

namespace ALICE_TRACER{
    BoundingBox::BoundingBox(AVec3 b_min, AVec3 b_max): b_min_(b_min), b_max_(b_max) {
    }

    Hittable::Hittable(Material *mtl, BxDFBase *bxdf): mtl_(mtl), bxdf_(bxdf) {

    }

    Hittable::Hittable(Material *mtl, BxDFBase *bxdf, Movement * movement): mtl_(mtl), bxdf_(bxdf), movement_(movement) {

    }

    void Hittable::translate(AVec3 offset) {
        transform_mat_ = ATranslate(transform_mat_, -offset);
    }

    void Hittable::scale(AVec3 scale) {
        transform_mat_ = AScale(transform_mat_, 1.f/scale);
    }

    void Hittable::rotate(float angle, AVec3 axis) {
        rot_mat_ = ARotate(rot_mat_, -angle, axis);
        transform_mat_ = ARotate(transform_mat_, -angle, axis);
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
        AVec3 start = transform_mat_ * AVec4(ray.start_, 1.f);
        AVec3 dir = rot_mat_ * AVec4(ray.dir_, 1.f);

        AVec3 oc = start - center(ray.fm_t_);
        float a = dot(dir, dir);
        float b = 2.f * dot(oc, dir);
        float c = dot(oc, oc) - radius_ * radius_;
        float discriminant = b*b - 4.f * a * c;
        if (discriminant < 0.f) {
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

    AVec3 RectangleXY::center(float frame_time) {
        if(movement_){
            return movement_->movementFunc(center_, frame_time);
        }
        return center_;
    }

    AVec3 RectangleXY::getNormal(AVec3 &point) {
        return normal_;
    }

    float RectangleXY::CheckHittable(Ray &ray) {
        AVec3 start = transform_mat_ * AVec4(ray.start_, 1.f);
        AVec3 dir = rot_mat_ * AVec4(ray.dir_, 1.f);

        float ldotn = ADot(dir, normal_);
        if(abs(ldotn) < MIN_THRESHOLD)
            return MAXFLOAT;
        float time = ADot(center(ray.fm_t_) - start, normal_)/ ldotn;
        AVec3 size = start + time * dir - center(ray.fm_t_);
        if(abs(size.x) < size_.x / 2.f && abs(size.y) < size_.y / 2.f)
            return time;
        return MAXFLOAT;
    }


}