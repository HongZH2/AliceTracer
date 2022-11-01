//
// Created by Hong Zhang on 2022/10/29.
//

#ifndef ALICE_TRACER_IMAGE_H
#define ALICE_TRACER_IMAGE_H

#include "utils/include/alice_common.h"

// Define some image format for visualization
// PPM
namespace ALICE_TRACER{
    // Image Type
    enum class ImageType{
        IMG_RED = 0,
        IMG_RGB,
        IMG_RGBA,
        IMG_RGB16F,
        IMG_RGB32F
    };

    // Image format
    class ImageBase{
    public:
        ImageBase(uint32_t width, uint32_t height, uint32_t channel, ImageType type);
        virtual ~ImageBase() = default;

        inline uint32_t w() const{return width_;}
        inline uint32_t h() const{return height_;}
        inline uint32_t c() const{return channel_;}
        inline uint32_t s() const{return stride_;}
        inline ImageType type() const{return type_;}

    protected:
        uint32_t width_;
        uint32_t height_;
        uint32_t channel_;
        uint32_t stride_;
        ImageType type_;
    };

    // RGBA 24 bits per pixel
    class ImageRGB: public ImageBase{
    public:
        ImageRGB(uint32_t width, uint32_t height);
        ~ImageRGB() override;

        // reload the operator ()
        // i for height, j for width
        uint8_t & operator()(uint32_t i, uint32_t j){
            return buffer_[i * stride_ + j * channel_];
        }

        uint8_t & operator()(uint32_t i, uint32_t j, uint32_t c){
            return buffer_[i * stride_ + j * channel_+ c];
        }

        // get the buffer ptr
        inline uint8_t* & getDataPtr(){return buffer_;}

    private:
        uint8_t * buffer_;
    };

    // RGBA 32 bits per pixel
    class ImageRGBA: public ImageBase{
    public:
        ImageRGBA(uint32_t width, uint32_t height);
        ~ImageRGBA() override;

        // reload the operator ()
        uint8_t & operator()(uint32_t i, uint32_t j){
            return buffer_[i * stride_ + j * channel_];
        }

        uint8_t & operator()(uint32_t i, uint32_t j, uint32_t c){
            return buffer_[i * stride_ + j * channel_+ c];
        }

        // get the buffer ptr
        inline uint8_t* & getDataPtr(){return buffer_;}

    private:
        uint8_t * buffer_;
    };

    // RGB32F: 32 bits float per pixel
    class ImageRGB32F: public ImageBase{
    public:
        ImageRGB32F(uint32_t width, uint32_t height);
        ~ImageRGB32F() override;

        // reload the operator ()
        float & operator()(uint32_t i, uint32_t j){
            return buffer_[i * stride_ + j * channel_];
        }

        float & operator()(uint32_t i, uint32_t j, uint32_t c){
            return buffer_[i * stride_ + j * channel_+ c];
        }

        // get the buffer ptr
        inline float* & getDataPtr(){return buffer_;}
    private:
        float * buffer_;
    };






}


#endif //ALICE_TRACER_IMAGE_H
