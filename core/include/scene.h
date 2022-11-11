//
// Created by Hong Zhang on 2022/10/29.
//

#ifndef ALICE_TRACER_SCENE_H
#define ALICE_TRACER_SCENE_H

#include "utils/include/alice_common.h"
#include "hittable_cluster.h"
#include "camera.h"

namespace ALICE_TRACER{

    /*
     * Class Scene for managing the hittable instances
     */
    class Scene{
    public:
        using LightType = std::variant<RectangleXZ*, RectangleXY*, RectangleYZ*, Sphere*>;

        Scene();
        ~Scene();

        void addCamera(Camera & camera);
        void addHittable(Hittable * hittable);
        void removeHittable(Hittable * hittable);
        void buildBVH();

        template<class T>
        void addLight(T * light){
            cluster_->addHittableInst(light);
            lights_.emplace_back(light);
        }

        inline void setBgFunc(std::function<void(AVec3 &, AVec3 &)> func){ background_func_ = func;}  // set the background
    public:
        Camera * camera_;
        ClusterList * cluster_;
        std::vector<LightType> lights_;
        std::function<void(AVec3 &, AVec3 &)> background_func_ = nullptr;
    };
}

#endif //ALICE_TRACER_SCENE_H
