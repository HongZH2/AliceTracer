//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_ALICE_COMMON_H
#define ALICE_TRACER_ALICE_COMMON_H

#include <thread>
#include <vector>
#include <array>
#include <queue>
#include <random>
#include <mutex>
#include <condition_variable>
#include <cstdint>
#include <variant>
#include "alice_log.h"

namespace ALICE_UTILS {

#define MAX_NUM_THREADS 12
#define AABB_PADDING 1e-3
#define MIN_RAY_TIME 1e-4
#define MAX_RAY_TIME FLT_MAX
#define MIN_THRESHOLD 1e-4
}


#endif //ALICE_TRACER_ALICE_COMMON_H
