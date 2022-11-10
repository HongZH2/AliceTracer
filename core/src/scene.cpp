//
// Created by zhanghong50 on 2022/11/1.
//

#include "core/include/scene.h"

namespace ALICE_TRACER{

    Scene::Scene(){
        cluster_ = new ClusterList();
    }

    Scene::~Scene() {
        delete cluster_;
    }

    void Scene::buildBVH() {
        AliceLog::submitDebugLog("Start Building BVH ...\n");
        cluster_->buildBVH();
        AliceLog::submitDebugLog("BVH is all set!\n");
    }

    void Scene::addCamera(Camera &camera) {
        camera_ = &camera;
    }

    void Scene::addHittable(Hittable * hittable) {
        cluster_->addHittableInst(hittable);
    }

    void Scene::removeHittable(Hittable * hittable) {
        cluster_->removeHittableInst(hittable->ID());
    }




}