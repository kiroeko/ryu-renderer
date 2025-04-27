#ifndef __FRAME_H__
#define __FRAME_H__

#include "glad/gl.h"

#include "graphics/Texture2d.h"

namespace RyuRenderer::Graphics
{
    class Frame
    {
    public:
        Frame() = default;

        Frame(Texture2d* initTexture, GLint initColorAttachmentIdx = 0);

        Frame(const Frame& other) = delete;

        Frame(Frame&& other) noexcept;

        ~Frame();

        Frame& operator=(Frame& other) = delete;

        Frame& operator=(Frame&& other) noexcept;

        bool IsValid() const;

        bool IsCompleted() const;

        bool IsUsing() const;

        bool Attach(Texture2d* t, GLint colorAttachmentIdx);

        bool Use() const;

        static void Unuse();

        static GLint GetColorAttachmentId(GLint colorAttachmentIdx);

        static GLint GetColorAttachmentIdx(GLint colorAttachmentId);

        inline static bool IsCleanMode = true;
    private:
        void Clear();

        static GLint GetMaxColorAttachmentAmount();

        GLuint id = 0;
        bool frameCompleted = false;

        inline static GLint maxColorAttachmentAmount = -1;
        inline static GLuint lastestUsedFrameId = 0;
    };
}

#endif