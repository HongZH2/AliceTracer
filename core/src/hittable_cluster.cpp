//
// Created by zhanghong50 on 2022/10/31.
//

#include "core/include/hittable_cluster.h"
#include "core/include/random_variable.h"
#include "core/include/sampler.h"

namespace ALICE_TRACER{
    // -------------------------------
    // BVH tree node
    // -------------------------------
    bool BVHNode::CheckHittable(Ray &ray, HitRes &hit_res) {
        AABB * bbox = boundLimit(ray.fm_t_);
        if(bbox->isHit(ray)){
            bool l_hit = left_->CheckHittable(ray, hit_res);
            bool r_hit = right_->CheckHittable(ray, hit_res);
            return l_hit || r_hit;
        }
        return false;
    }

    // -------------------------------
    // ClusterList
    // -------------------------------
    ClusterList::~ClusterList()  {
        releaseBVH(bvh_tree_);
    }

    void ClusterList::releaseBVH(Hittable *node) {
        if(!node) return;
        BVHNode * bvh_node = dynamic_cast<BVHNode *>(node);
        if(!bvh_node || (!bvh_node->left_ && !bvh_node->right_)){
            delete node;
            return;
        }
        releaseBVH(bvh_node->left_);
        releaseBVH(bvh_node->right_);
        delete bvh_node;
    }

    void ClusterList::buildBVH() {
        setUpBVH(bvh_tree_, hittable_array_);        // build BVH recursively
    }

    void ClusterList::setUpBVH(Hittable* & node, std::vector<Hittable *> & hittable_list) {
        if(hittable_array_.empty())
            return;
        if(hittable_list.size() == 1){
            node = hittable_list[0];
            return;
        }
        std::vector<Hittable *> hittable_inst = hittable_list;
        // step 1. randomly select one axis
        int32_t axis = random_int(0, 2);
        // step 2. sort the list
        std::sort(hittable_inst.begin(), hittable_inst.end(), [axis](Hittable * t1, Hittable * t2){
           AABB* bbox1 = t1->boundLimit(0.f);  // check the initial bounding box
           AABB* bbox2 = t2->boundLimit(0.f);
           return bbox1->b_min_[axis] < bbox2->b_min_[axis];
        });
        // create a BVH node
        node = new BVHNode();
        BVHNode * bvh_node = (BVHNode *)node;
        uint32_t mid = (hittable_inst.size())/2;
        std::vector<Hittable *> left_list {hittable_inst.begin(), hittable_inst.begin() + mid};
        std::vector<Hittable *> right_list {hittable_inst.begin() + mid, hittable_inst.end()};

        setUpBVH(bvh_node->left_, left_list);
        setUpBVH(bvh_node->right_, right_list);

        bvh_node->split_axis_ = axis;
        bvh_node->num_instance_ = (int32_t)(hittable_inst.size());
        AABB* bbox = bvh_node->boundLimit(0.f);
        bbox->unionAABB(*bvh_node->left_->boundLimit(0.f), *bvh_node->right_->boundLimit(0.f));
    }

    bool ClusterList::CheckHittable(Ray &ray, HitRes &hit_res) {
        if(bvh_tree_)
            bvh_tree_->CheckHittable(ray, hit_res);
        return hit_res.is_hit_;
    }

    AABB *ClusterList::boundLimit(float frame_time) {
        if(bvh_tree_)
            return bvh_tree_->boundLimit(frame_time);
        return Hittable::boundLimit(frame_time);
    }

    void ClusterList::addHittableInst(Hittable *hittable_inst) {
        if(hittable_inst) {
            hittable_array_.push_back(hittable_inst);
        }
    }


//    // -------------------------------
//    // Box(6-face cluster list)
//    // -------------------------------
//    void Box::generateBox(){
//        // create the box
//        RectangleXY * rect1 = new RectangleXY{center_ + AVec3(0.f, 0.f, scale_.z/2.f), AVec2(scale_.x, scale_.y), mtl_, bxdf_};
//        RectangleXZ * rect2 = new RectangleXZ{center_ + AVec3(0.f, scale_.y/2.f, 0.f), AVec2(scale_.x, scale_.z), mtl_, bxdf_};
//        RectangleYZ * rect3 = new RectangleYZ{center_ + AVec3(scale_.x/2.f, 0.f, 0.f), AVec2(scale_.y, scale_.z), mtl_, bxdf_};
//        RectangleXY * rect4 = new RectangleXY{center_ + AVec3(0.f, 0.f, -scale_.z/2.f), AVec2(scale_.x, scale_.y),mtl_, bxdf_};
//        RectangleXZ * rect5 = new RectangleXZ{center_ + AVec3(0.f, -scale_.y/2.f, 0.f), AVec2(scale_.x, scale_.z),mtl_, bxdf_};
//        RectangleYZ * rect6 = new RectangleYZ{center_ + AVec3(-scale_.x/2.f, 0.f, 0.f), AVec2(scale_.y, scale_.z),mtl_, bxdf_};
//        hittable_array_.addHittableInst(rect1);
//        hittable_array_.addHittableInst(rect2);
//        hittable_array_.addHittableInst(rect3);
//        hittable_array_.addHittableInst(rect4);
//        hittable_array_.addHittableInst(rect5);
//        hittable_array_.addHittableInst(rect6);
//    };
//
//    Box::Box(ALICE_UTILS::AVec3 center, ALICE_UTILS::AVec3 scale, ALICE_TRACER::Material *mtl,
//             ALICE_TRACER::BxDFBase *bxdf): center_(center), scale_(scale) {
//        mtl_ = mtl;
//        bxdf_ = bxdf;
//        generateBox();
//        hittable_array_.buildBVH();
//    }
//
//    Box::Box(ALICE_TRACER::Material *mtl, ALICE_TRACER::BxDFBase *bxdf){
//        mtl_ = mtl;
//        bxdf_ = bxdf;
//        generateBox();
//        hittable_array_.buildBVH();
//    }
//
//    Box::Box(ALICE_TRACER::Material *mtl, ALICE_TRACER::BxDFBase *bxdf, ALICE_TRACER::Movement *movement){
//        mtl_ = mtl;
//        bxdf_ = bxdf;
//        movement_ = movement;
//        generateBox();
//        hittable_array_.buildBVH();
//    }
//
//    AABB *Box::boundLimit(float frame_time) {
//        return hittable_array_.boundLimit(frame_time);
//    }
//
//    bool Box::CheckHittable(ALICE_TRACER::Ray &ray, ALICE_TRACER::HitRes &hit_res) {
//        return hittable_array_.CheckHittable(ray, hit_res);
//    }

    // -------------------------------
    // Triangle Mesh
    // -------------------------------
    Triangle::Triangle(ALICE_TRACER::Material *mtl, ALICE_TRACER::BxDFBase *bxdf):
            Hittable(mtl, bxdf){

    }

    AABB *Triangle::boundLimit(float frame_time) {
        if(vertices_) {
            AVec3 b_min = AVec3(FLT_MAX);
            AVec3 b_max = AVec3(FLT_MIN);
            AVec3 v1 = *(vertices_ + idx_[0]);
            b_min = min(b_min, v1);
            b_max = max(b_max, v1);
            AVec3 v2 = *(vertices_ + idx_[1]);
            b_min = min(b_min, v2);
            b_max = max(b_max, v2);
            AVec3 v3 = *(vertices_ + idx_[2]);
            b_min = min(b_min, v3);
            b_max = max(b_max, v3);
            bound_->b_min_ = b_min - AVec3(AABB_PADDING);
            bound_->b_max_ = b_max + AVec3(AABB_PADDING);
        }
        return bound_;
    }

    bool Triangle::CheckHittable(ALICE_TRACER::Ray &ray, ALICE_TRACER::HitRes &hit_res) {
        // Barycentric Method to get the intersection
        if(!vertices_) return false;
        AVec3 v1 = *(vertices_ + idx_[0]);
        AVec3 v2 = *(vertices_ + idx_[1]);
        AVec3 v3 = *(vertices_ + idx_[2]);
        AVec3 v21 = v1 - v2;
        AVec3 v31 = v1 - v3;
        AVec3 norm = ANormalize(ACross(v21, v31));
        if(ADot(norm, ray.dir_) < MIN_THRESHOLD && ADot(norm, ray.dir_) > -MIN_THRESHOLD)
            return false;
        AVec3 vo1 = v1 - ray.start_;
        AMat3 A {v21, v31, ray.dir_}; // refer to the book "the foundations of computer graphics"
        float M = A[0][0] * (A[1][1] * A[2][2] - A[2][1] * A[1][2])
                    + A[0][1] * (A[2][0] * A[1][2] - A[1][0] * A[2][2])
                    + A[0][2] * (A[1][0] * A[2][1] - A[1][1] * A[2][0]);
        float beta = vo1[0] * (A[1][1] * A[2][2] - A[2][1] * A[1][2])
                     + vo1[1] * (A[2][0] * A[1][2] - A[1][0] * A[2][2])
                     + vo1[2] * (A[1][0] * A[2][1] - A[1][1] * A[2][0]);
        beta /= M;
        float lambda = A[2][2] * (A[0][0] * vo1[1] - vo1[0] * A[0][1])
                        + A[2][1] * (vo1[0] * A[0][2] - A[0][0] * vo1[2])
                        + A[2][0] * (A[0][1] * vo1[2] - vo1[1] * A[0][2]);
        lambda /= M;
        float time = A[1][2] * (A[0][0] * vo1[1] - vo1[0] * A[0][1])
                    + A[1][1] * (vo1[0] * A[0][2] - vo1[2]* A[0][0])
                    + A[1][0] * (A[0][1] * vo1[2] - vo1[1] * A[0][2]);
        time /= -M;
        if(beta > 0.f && lambda > 0.f && beta + lambda < 1.f){
            if(time < ray.t_max_ && time > ray.t_min_) {

                // compute the normals
                AVec3 n1 = *(normal_ + idx_[0]);
                AVec3 n2 = *(normal_ + idx_[1]);
                AVec3 n3 = *(normal_ + idx_[2]);
                norm = ANormalize(n2 * beta + n3 * lambda + n1 * (1.f - beta - lambda));

                ray.t_max_ = time;
                hit_res.uni_id_ = id_;
                hit_res.is_hit_ = true;
                hit_res.mtl_ = mtl_;
                hit_res.bxdf_ = bxdf_;
                hit_res.point_ = ray.start_ + ray.dir_ * time;
                hit_res.setNormal(norm, ray.dir_);
                hit_res.frame_time_ = ray.fm_t_;
                return true;
            }
        }
        return false;
    }

    TriangleMesh::TriangleMesh(ALICE_TRACER::Material *mtl, ALICE_TRACER::BxDFBase *bxdf):
        Hittable(mtl, bxdf){

    }

    TriangleMesh::TriangleMesh(ALICE_TRACER::Material *mtl, ALICE_TRACER::BxDFBase *bxdf,
                               ALICE_TRACER::Movement *movement):
            Hittable(mtl, bxdf){

    }

    void TriangleMesh::setTriangleMaterial(ALICE_TRACER::Material *mtl, ALICE_TRACER::BxDFBase *bxdf) {
        for(auto & triangle: hittable_array_.getInst()){
            triangle->setMaterial(mtl);
            triangle->setBxdf(bxdf);
        }
    }

    AABB *TriangleMesh::boundLimit(float frame_time) {
        return hittable_array_.boundLimit(frame_time);
    }

    bool TriangleMesh::CheckHittable(ALICE_TRACER::Ray &ray, ALICE_TRACER::HitRes &hit_res) {
        return hittable_array_.CheckHittable(ray, hit_res);
    }

    void TriangleMesh::parseMesh() {
        for(auto & index : indices_){
            auto * triangle = new Triangle(mtl_, bxdf_);
            if(!vertices_.empty()) triangle->vertices_ = &vertices_[0];
            if(!normal_.empty()) triangle->normal_ = &normal_[0];
            if(!tex_coords_.empty()) triangle->tex_coord_ = &tex_coords_[0];
            if(!tangent_.empty()) triangle->tangent_ = &tangent_[0];
            triangle->idx_ = index;
            hittable_array_.addHittableInst(triangle);
        }
        hittable_array_.buildBVH();
    }

    // -------------------------------
    // Linear Interpolation on the triangle
    // Triangle Mesh
    // -------------------------------
    TriangleInstance::TriangleInstance(AVec3 center, AVec3 scale, float angle, AVec3 axis, Material *mtl, BxDFBase *bxdf):
            Hittable(mtl, bxdf), center_(center), scale_(scale) {
        rot_mat_ = ARotate(AMat4(1.f), ARadians(angle), axis);
    }

    TriangleInstance::TriangleInstance(ALICE_TRACER::Material *mtl, ALICE_TRACER::BxDFBase *bxdf):
            Hittable(mtl, bxdf){

    }

    TriangleInstance::TriangleInstance(ALICE_TRACER::Material *mtl, ALICE_TRACER::BxDFBase *bxdf,
                                       ALICE_TRACER::Movement *movement):
            Hittable(mtl, bxdf){

    }

    AABB *TriangleInstance::boundLimit(float frame_time) {
        return hittable_array_.boundLimit(frame_time);
    }

    bool TriangleInstance::CheckHittable(ALICE_TRACER::Ray &ray, ALICE_TRACER::HitRes &hit_res) {
        return hittable_array_.CheckHittable(ray, hit_res);
    }

    void TriangleInstance::addTriangleMesh(ALICE_TRACER::TriangleMesh *triangle_mesh) {
        hittable_array_.addHittableInst(triangle_mesh);
    }

    void TriangleInstance::setTriangleMaterial(uint32_t id, ALICE_TRACER::Material *mtl, ALICE_TRACER::BxDFBase *bxdf) {
        auto & hittable_list = hittable_array_.getInst();
        if(id < hittable_list.size()){
            TriangleMesh * triangle_mesh = dynamic_cast<TriangleMesh *>(hittable_list[id]);
            if(! triangle_mesh) return;
            triangle_mesh->setTriangleMaterial(mtl, bxdf);
            triangle_mesh->setMaterial(mtl);
            triangle_mesh->setBxdf(bxdf);
        }
    }

    void TriangleInstance::parseMesh() {
        for(auto & hittable: hittable_array_.getInst()){
            TriangleMesh * triangle_mesh = dynamic_cast<TriangleMesh*>(hittable);
            if(triangle_mesh) triangle_mesh->parseMesh();
        }
        hittable_array_.buildBVH();
    }

    // -------------------------------
    // Light Probe
    // -------------------------------
    EnvironmentalLight::EnvironmentalLight(ALICE_TRACER::ImageBase *img) : env_(img){
        auto img_float = dynamic_cast<ImageFloat *>(img);
        int32_t h = env_->h();
        int32_t w = env_->w();

        std::vector<double> rows(h);
        sum_ = 0.;
        for(int32_t i = 0; i < h; ++i){
            double row = 0.;
            for(int32_t j = 0; j < w; ++j){
                AVec3 c_pixel = (*img_float)(i, j);
                sum_ += (c_pixel.x + c_pixel.y + c_pixel.z)/3./(w * h);
                row += (c_pixel.x + c_pixel.y + c_pixel.z)/3.;
            }
            rows[i] = row / (double)w;
        }
        for(auto & row: rows){
            row /= sum_;
        }
        row_ = new Discrete1DSampler(rows);

        // column
        map_.resize(h);
        for(int32_t i = 0; i < h; ++i){
            std::vector<double> row_u(w);
            for (int j = 0; j < w; ++j) {
                AVec3 c_pixel = (*img_float)(i, j);
                double uv = (c_pixel.x + c_pixel.y + c_pixel.z)/3./sum_;
                row_u[j] = uv/rows[i];
            }
            Discrete1DSampler * v_row = new Discrete1DSampler(row_u);
            map_[i] = v_row;
        }
    }

    EnvironmentalLight::~EnvironmentalLight() {
        for(auto & row: map_)
            delete row;
        delete row_;
    }

    bool EnvironmentalLight::CheckHittable(ALICE_TRACER::Ray &ray, ALICE_TRACER::HitRes &hit_res) {
        hit_res.uni_id_ = id_;
        hit_res.is_hit_ = false;
        hit_res.point_ = ANormalize(ray.dir_);
        hit_res.frame_time_ = ray.fm_t_;
        return false;
    }


}