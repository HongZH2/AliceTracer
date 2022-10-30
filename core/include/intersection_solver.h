//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_INTERSECTION_SOLVER_H
#define ALICE_TRACER_INTERSECTION_SOLVER_H

#include "utils/include/alice_common.h"
#include "utils/include/alice_math.h"
#include "ray.h"
#include "object.h"

namespace ALICE_TRACER{

    // Class Intersection Solver to compute the intersection between ray and objects
    class IntersectionSolver{
    public:
        IntersectionSolver() = default;
        ~IntersectionSolver() = default;

        static float isHitObject(Ray & ray, Sphere & sphere); // TODO: remove sphere, use a more general class
    };
}


#endif //ALICE_TRACER_INTERSECTION_SOLVER_H
