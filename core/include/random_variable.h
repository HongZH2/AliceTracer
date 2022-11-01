//
// Created by Hong Zhang on 2022/10/29.
//

#ifndef ALICE_TRACER_RANDOM_VARIABLE_H
#define ALICE_TRACER_RANDOM_VARIABLE_H

#include <random>

namespace ALICE_TRACER {
    template<typename T>
    inline T random_val() {
        static std::uniform_real_distribution<T> distribution(0.0, 1.0);
        static std::mt19937 generator;
        return distribution(generator);
    }
}

#endif //ALICE_TRACER_RANDOM_VARIABLE_H
