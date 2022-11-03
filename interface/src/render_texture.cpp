//
// Created by Hong Zhang on 2022/10/30.
//
#include "interface/include/render_texture.h"
#include "OpenGL/gl3.h"

namespace ALICE_TRACER{

    void TextureBuffer::setUpShader() {
        const char *vertexShaderSource = "#version 330 core\n"
                                         "layout (location = 0) in vec3 aPos;\n"
                                         "layout (location = 1) in vec2 aUv;\n"
                                         "out vec2 uv;"
                                         "void main()\n"
                                         "{\n"
                                         "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                         "   uv = aUv;\n"
                                         "}\0";
        const char *fragmentShaderSource = "#version 330 core\n"
                                           "in vec2 uv;\n"
                                           "uniform sampler2D result;\n"
                                           "out vec4 FragColor;\n"
                                           "void main()\n"
                                           "{\n"
                                           "   FragColor = texture(result, vec2(uv.x, 1. - uv.y));\n"
                                           "}\n\0";


        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // fragment shader
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // link shaders
        program_id_ = glCreateProgram();
        glAttachShader(program_id_, vertexShader);
        glAttachShader(program_id_, fragmentShader);
        glLinkProgram(program_id_);
    }

    void TextureBuffer::setVertex() {
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float vertices[] = {
                // positions        // texture coords
                1.f,  1.f, 0.0f, 1.0f, 1.0f, // top right
                1.f, -1.f, 0.0f, 1.0f, 0.0f, // bottom right
                -1.f, -1.f, 0.0f, 0.0f, 0.0f, // bottom left
                -1.f,  1.f, 0.0f, 0.0f, 1.0f  // top left
        };

        unsigned int indices[] = {
                0, 1, 3,
                1, 2, 3
        };
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(vao_);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void TextureBuffer::drawTexture() {
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tid_);
        glUseProgram(program_id_);
        glUniform1i(glGetUniformLocation(program_id_, "result"), 0);
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }


    void TextureBuffer::loadGPUTexture(ALICE_TRACER::ImageBase *img) {
        width_ = img->w();
        height_ = img->h();
        channel_ = img->c();
        setVertex();
        setUpShader();
        // create a texture buffer
        glGenTextures(1, &tid_);
        glBindTexture(GL_TEXTURE_2D, tid_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        switch (img->type()) {
            case ImageType::IMG_RGB: {
                auto * temp = (ImageRGB *) img;
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, temp->w(), temp->h(), 0, GL_RGB, GL_UNSIGNED_BYTE, temp->getDataPtr());
                break;
            }
            case ImageType::IMG_RGBA:{
                auto * temp = (ImageRGBA *) img;
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, temp->w(), temp->h(), 0, GL_RGBA, GL_UNSIGNED_INT, temp->getDataPtr());
                break;
            }
            default:
                break;
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        is_update_ = false;
    }

}