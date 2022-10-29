//
// Created by Hong Zhang on 2022/10/29.
//
#include "core/include/image.h"

namespace ALICE_TRACER{
    ImageBase::ImageBase(uint32_t width, uint32_t height, uint32_t channel, ImageType type)
                                : width_(width), height_(height), channel_(channel), type_(type) {
        stride_ = channel * width;
    }

    ImageRGB::ImageRGB(uint32_t width, uint32_t height): ImageBase(width, height, 3, ImageType::IMG_RGB){
        buffer_ = new uint8_t [width * height * 3];
    }

    ImageRGB::~ImageRGB() {
        if(buffer_)
            delete [] buffer_;
        buffer_ = nullptr;
    }

    ImageRGBA::ImageRGBA(uint32_t width, uint32_t height) : ImageBase(width, height, 4, ImageType::IMG_RGBA){
        buffer_ = new uint8_t [width * height * 3];
    }

    ImageRGBA::~ImageRGBA() {
        if(buffer_)
            delete [] buffer_;
        buffer_ = nullptr;
    }



}