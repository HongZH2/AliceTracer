//
// Created by Hong Zhang on 2022/11/6.
//

#include "interface/include/model_loader.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

namespace ALICE_TRACER{

    void ModelLoader::loadModel(const std::string &path, ALICE_TRACER::TriangleMesh *mesh) {
        // 1. load model from file
        Assimp::Importer importer;
        const aiScene* model = importer.ReadFile(path, aiProcess_GenNormals |
                                                        aiProcess_GlobalScale |
                                                        aiProcess_OptimizeMeshes |
                                                        aiProcess_OptimizeGraph|
                                                        aiProcess_FixInfacingNormals|
                                                        aiProcess_GenNormals);
        if (!model || !mesh) {
            AliceLog::submitInfoLog("Fail to load the model\n");
            return;
        }

        // parse the model
        float max_xyz = 0.0f;
        if(model->mNumMeshes > 0){
            // create a mesh component
            for(uint32_t i = 0; i<model->mNumMeshes; ++i){
                // mesh
                for(uint32_t j = 0; j<model->mMeshes[i]->mNumVertices; ++j){
                    AVec3 vertex;
                    vertex.x = model->mMeshes[i]->mVertices[j].x;
                    vertex.y = model->mMeshes[i]->mVertices[j].y;
                    vertex.z = model->mMeshes[i]->mVertices[j].z;
                    mesh->vertices_.push_back(vertex * mesh->scale() + mesh->offset());
                }

                // indices
                for(uint32_t j = 0; j<model->mMeshes[i]->mNumFaces; ++j){
                    if(model->mMeshes[i]->mFaces[j].mNumIndices != 3){
                        assert("only triangle face is supported!\n");
                        return;
                    }
                    AVec3i face;
                    for(uint32_t k = 0; k<model->mMeshes[i]->mFaces[j].mNumIndices; ++k){
                        face[k] = model->mMeshes[i]->mFaces[j].mIndices[k];
                    }
                    mesh->indices_.push_back(face);
                }
            }
            mesh->parseMesh();
        }
    }

}
