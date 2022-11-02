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

    Sphere::Sphere(AVec3 center, float radius, Material *mtl, BxDFBase *bxdf)
            : Hittable(mtl, bxdf), center_(center), radius_(radius) {
    }

    Sphere::Sphere(AVec3 center, float radius, Material *mtl, BxDFBase *bxdf, Movement * movement)
            : Hittable(mtl, bxdf, movement), center_(center), radius_(radius) {
    }

    AVec3 Sphere::getNormal(AVec3 &point) {
        return ANormalize(point - center_);
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
        if (discriminant < 0.f) {
            return -1.f;
        } else {
            return (- b - sqrt(discriminant) ) / (2.f * a);
        }
    }

//    Cube::Cube(AVec3 center, float length): center_(center), length_(length) {
//            // TODO
//    }
//
//    AVec3 Cube::getNormal(AVec3 &point) { // TODO
//        return {0.f, 0.f, 0.f};
//    }

}