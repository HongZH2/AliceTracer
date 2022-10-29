//
// Created by Hong Zhang on 2022/10/30.
//

#include "core/include/intersection_solver.h"

namespace ALICE_TRACER{

    bool IntersectionSolver::isHitObject(Ray &ray, Sphere &sphere) {
        AVec3 oc = ray.start_ - sphere.center();
        auto a = ADot(ray.dir_, ray.dir_);
        auto b = 2.0 * dot(oc, ray.dir_);
        auto c = dot(oc, oc) - sphere.radius() * sphere.radius();
        auto discriminant = b * b - 4 * a * c;
        return (discriminant > 0);
    }

}