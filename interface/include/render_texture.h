//
// Created by Hong Zhang on 2022/10/30.
//

#ifndef ALICE_TRACER_TEXTURE_H
#define ALICE_TRACER_TEXTURE_H

#include "utils/include/alice_common.h"
#include "core/include/image.h"

namespace ALICE_TRACER{
    class TextureBuffer{
    public:
        TextureBuffer() = default;
        ~TextureBuffer() = default;

        void loadGPUTexture(ImageBase * img);

        inline bool isUpdate() const {return is_update_;}
        inline uint32_t texID() const {return tid_;}
        inline uint32_t w() const {return width_;}
        inline uint32_t h() const {return height_;}
        inline uint32_t c() const {return channel_;}
        void drawTexture();
    private:
        void setUpShader();
        void setVertex();
        bool is_update_ = true;
        uint32_t vao_;
        uint32_t program_id_;
        uint32_t bid_;
        uint32_t tid_;  // texture id
        uint32_t width_;
        uint32_t height_;
        uint32_t channel_;
    };
}

#endif //ALICE_TRACER_TEXTURE_H
