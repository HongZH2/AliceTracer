//
// Created by Hong Zhang on 2022/10/29.
//
#include "core/include/image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "third_parties/stb_image/stb_image.h"

namespace ALICE_TRACER{
    ImageBase::ImageBase(uint32_t width, uint32_t height, uint32_t channel, ImageType type)
                                : width_(width), height_(height), channel_(channel), type_(type) {
        stride_ = channel * width;
    }

    ImageUByte::ImageUByte(ImageType t, uint32_t width, uint32_t height, uint32_t channel, uint32_t stride, uint8_t * buffer):
        ImageBase(width, height, channel, t), buffer_(buffer) {
        stride_ = stride;
    }

    ImageUByte::ImageUByte(ImageType t, uint32_t width, uint32_t height, uint32_t channel):
        ImageBase(width, height, channel, t){
        buffer_ = new uint8_t [width * height * channel];
    }

    ImageUByte::~ImageUByte() {
        if(buffer_)
            delete [] buffer_;
        buffer_ = nullptr;
    }

    ImageFloat::ImageFloat(ImageType t, uint32_t width, uint32_t height, uint32_t channel, uint32_t stride, float * buffer):
            ImageBase(width, height, channel, ImageType::IMG_RGB_Float), buffer_(buffer) {
        stride_ = stride;
    }


    ImageFloat::ImageFloat(ImageType t, uint32_t width, uint32_t height, uint32_t channel):
        ImageBase(width, height, channel, ImageType::IMG_RGB_Float){
        buffer_ = new float [width * height * channel];
    }

    ImageFloat::~ImageFloat() {
        if(buffer_)
            delete [] buffer_;
        buffer_ = nullptr;
    }

    ImagePool::~ImagePool() {
        for(auto & [name, img] : pool_){
            if(img)
                delete img;
        }
    }

    ImageBase * ImagePool::loadRGB(const std::string & name, const std::string & path){
        ImagePool & pool = getInstance();
        int32_t w, h, c;
        uint8_t * buffer;
        buffer = stbi_load(path.c_str(), &w, &h, &c, 0);
        if(!buffer) {
            assert("Fail to load image!!!\n");
            return nullptr;
        }
        ImageBase * img = new ImageUByte(ImageType::IMG_RGB_UByte, w, h, c, w * c, buffer);
        pool.pool_[name] = img;
        return img;
    };

    ImageBase * ImagePool::loadHdr(const std::string & name, const std::string & path){
        ImagePool & pool = getInstance();
        int32_t w, h, c;
        float * buffer;
        buffer = stbi_loadf(path.c_str(), &w, &h, &c, 0);
        ImageBase * img = new ImageFloat(ImageType::IMG_RGB_Float, w, h, c, w * c, buffer);
        pool.pool_[name] = img;
        return img;
    }

    ImageBase *ImagePool::getImage(const std::string & name) {
        ImagePool & pool = getInstance();
        if(pool.pool_.find(name) != pool.pool_.end()){
            return pool.pool_.at(name);
        }
        return nullptr;
    }


}