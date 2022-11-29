//
// Created by Hong Zhang on 2022/10/29.
//

#ifndef ALICE_TRACER_IMAGE_H
#define ALICE_TRACER_IMAGE_H

#include "utils/include/alice_common.h"
#include "utils/include/alice_math.h"
using namespace ALICE_UTILS;
// Define some image format for visualization
// PPM
namespace ALICE_TRACER{
    // Image Type
    enum class ImageType{
        IMG_RED = 0,
        IMG_RGB_UByte,
        IMG_RGBA_UByte,
        IMG_RGB_Float,
        IMG_RGBA_Float
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
    class ImageUByte: public ImageBase{
    public:
        ImageUByte(ImageType t, uint32_t width, uint32_t height, uint32_t channel, uint32_t stride, uint8_t * buffer);
        ImageUByte(ImageType t, uint32_t width, uint32_t height, uint32_t channel);
        ~ImageUByte() override;

        // reload the operator ()
        // i for height, j for width
        uint8_t & operator()(uint32_t i, uint32_t j, uint32_t c){
            return buffer_[i * stride_ + j * channel_+ c];
        }

        AVec3 operator()(uint32_t i, uint32_t j){
            return AVec3{buffer_[i * stride_ + j * channel_], buffer_[i * stride_ + j * channel_ + 1], buffer_[i * stride_ + j * channel_ + 2]};
        }

        // get the buffer ptr
        inline uint8_t* & getDataPtr(){return buffer_;}

    private:
        uint8_t * buffer_;
    };

    // RGB32F: 32 bits float per pixel
    class ImageFloat: public ImageBase{
    public:
        ImageFloat(ImageType t, uint32_t width, uint32_t height, uint32_t channel, uint32_t stride, float * buffer);
        ImageFloat(ImageType t, uint32_t width, uint32_t height, uint32_t channel);
        ~ImageFloat() override;

        // reload the operator ()
        AVec3 operator()(uint32_t i, uint32_t j){
            return AVec3{buffer_[i * stride_ + j * channel_], buffer_[i * stride_ + j * channel_ + 1], buffer_[i * stride_ + j * channel_ + 2]};
        }

        float & operator()(uint32_t i, uint32_t j, uint32_t c){
            return buffer_[i * stride_ + j * channel_+ c];
        }

        // get the buffer ptr
        inline float* & getDataPtr(){return buffer_;}
    private:
        float * buffer_;
    };

    // load from the disk
    class ImagePool{
    public:
        ImagePool(const ImagePool & ) = delete;
        ImagePool & operator= (const ImagePool &) = delete;
        static ImagePool & getInstance(){
            static ImagePool instance;
            return instance;
        }
        static ImageBase * loadRGB(const std::string & name, const std::string & path);
        static ImageBase * loadHdr(const std::string & name, const std::string & path);
        static ImageBase * getImage(const std::string & name);

    private:
        ImagePool() = default;
        ~ImagePool();
        std::unordered_map<std::string, ImageBase*> pool_;
    };



}


#endif //ALICE_TRACER_IMAGE_H
