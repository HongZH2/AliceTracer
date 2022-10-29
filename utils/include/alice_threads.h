//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_ALICE_THREADS_H
#define ALICE_TRACER_ALICE_THREADS_H

#include "alice_common.h"
namespace ALICE_UTILS {
    /*
     * TheadPool
     */
    class ThreadPool{
    public:

    private:
        std::array<std::thread, MAX_NUM_THREADS> thread_pool_;
    };
}

#endif //ALICE_TRACER_ALICE_THREADS_H
