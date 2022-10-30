//
// Created by Hong Zhang on 2022/10/30.
//

#include "core/include/object.h"

namespace ALICE_TRACER{
    BoundingBox::BoundingBox(AVec3 b_min, AVec3 b_max): b_min_(b_min), b_max_(b_max) {
    }

    Sphere::Sphere(AVec3 center, float radius): center_(center), radius_(radius) {

    }

    AVec3 Sphere::getNormal(AVec3 &point) {
        return ANormalize(point - center_);
    }

    Cube::Cube(AVec3 center, float length): center_(center), length_(length) {

    }

}