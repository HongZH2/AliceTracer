//
// Created by Hong Zhang on 2022/11/6.
//

#ifndef ALICE_TRACER_MODEL_LOADER_H
#define ALICE_TRACER_MODEL_LOADER_H

#include "core/include/hittable_cluster.h"

namespace ALICE_TRACER{
    class ModelLoader{
    public:
        ModelLoader() = default;
        ~ModelLoader() = default;

        static void loadModel(const std::string & path, TriangleInstance * mesh);

    };
}


#endif //ALICE_TRACER_MODEL_LOADER_H
