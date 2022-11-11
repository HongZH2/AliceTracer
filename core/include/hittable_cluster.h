//
// Created by Hong Zhang on 2022/10/30.
//

#ifndef ALICE_TRACER_HITTABLE_CLUSTER_H
#define ALICE_TRACER_HITTABLE_CLUSTER_H

#include "utils/include/alice_common.h"
#include "utils/include/alice_math.h"
#include "hittable.h"

namespace ALICE_TRACER{
    // -------------------------------
    // BVH tree node:
    // inherit the hittable class, we will use the AABB to
    // -------------------------------
    class BVHNode: public Hittable{
    public:
        BVHNode() = default;
        ~BVHNode() override = default;

        bool CheckHittable(Ray & ray, HitRes & hit_res) override;
    public:
        Hittable * left_ = nullptr;
        Hittable * right_ = nullptr;
        int32_t split_axis_ = -1;
        int32_t num_instance_ = -1;
    };

    // -------------------------------
    // A set of the hittable instances
    // -------------------------------
    class ClusterList: public Hittable{
    public:
        ClusterList() = default;
        ~ClusterList() override;
        bool CheckHittable(Ray & ray, HitRes & hit_res) override;
        AABB * boundLimit(float frame_time) override;
    public:
        void addHittableInst(Hittable * hittable_inst);
        void removeHittableInst(uint32_t id);
        void buildBVH();

    private:
        void releaseBVH(Hittable * node);
        void setUpBVH(Hittable* & node, std::vector<Hittable *> & hittable_list);
        Hittable * bvh_tree_ = nullptr;
        std::vector<Hittable *> hittable_array_;  // a list of the hittable instances
    };

//    // -------------------------------
//    // Box(6-face cluster list)
//    // -------------------------------
//    class Box: public Hittable{
//    public:
//        Box(AVec3 center, AVec3 scale, Material *mtl, BxDFBase *bxdf);
//        Box(Material *mtl, BxDFBase *bxdf);
//        Box(Material *mtl, BxDFBase *bxdf, Movement * movement);
//        ~Box() override = default;
//        bool CheckHittable(Ray & ray, HitRes & hit_res) override;
//        AABB * boundLimit(float frame_time) override;
//    private:
//        AVec3 center_ = AVec3(0.f);
//        AVec3 scale_ = AVec3(1.f);
//        void generateBox();
//        ClusterList hittable_array_;
//    };

    // -------------------------------
    // Linear Interpolation on the triangle
    // Triangle Mesh
    // -------------------------------
    class Triangle : public Hittable{
    public:
        Triangle(Material *mtl, BxDFBase *bxdf);
        ~Triangle() override = default;
        bool CheckHittable(Ray & ray, HitRes & hit_res) override;
        AABB * boundLimit(float frame_time) override;
    public:
        AVec3i idx_;
        AVec3 * vertices_;
        AVec3 * tex_coord_;
        AVec3 * normal_;
        AVec3 * tangent_;
    };

    class TriangleMesh : public Hittable{
    public:
        TriangleMesh(AVec3 center, AVec3 scale, float angle, AVec3 axis, Material *mtl, BxDFBase *bxdf);
        TriangleMesh(Material *mtl, BxDFBase *bxdf);
        TriangleMesh(Material *mtl, BxDFBase *bxdf, Movement * movement);
        ~TriangleMesh() override = default;
        bool CheckHittable(Ray & ray, HitRes & hit_res) override;
        AABB * boundLimit(float frame_time) override;
        void parseMesh();
        inline AVec3 offset(){return center_;}
        inline AVec3 scale(){return scale_;}
        inline AMat3 rot(){return rot_mat_;}
    public:
        std::vector<AVec3> vertices_;
        std::vector<AVec3i> indices_;
        std::vector<AVec3> tangent_;
        std::vector<AVec3> normal_;
        std::vector<AVec3> tex_coords_;
    protected:
        AVec3 center_ = AVec3(0.f);
        AVec3 scale_ = AVec3(1.f);
        AMat3 rot_mat_ = AMat3(1.f);
        ClusterList hittable_array_;
    };
}


#endif //ALICE_TRACER_HITTABLE_CLUSTER_H
