//
// Created by Hong Zhang on 2022/10/30.
//

#include "core/include/intersection_solver.h"

namespace ALICE_TRACER{

    float IntersectionSolver::isHitObject(Ray &ray, Sphere &sphere) {
        AVec3 oc = ray.start_ - sphere.center();
        auto a = dot(ray.dir_, ray.dir_);
        auto b = 2.0 * dot(oc, ray.dir_);
        auto c = dot(oc, oc) - sphere.radius() * sphere.radius();
        auto discriminant = b*b - 4*a*c;
        if (discriminant < 0) {
            return -1.0;
        } else {
            return (-b - sqrt(discriminant) ) / (2.0*a);
        }
    }

}