#ifndef __FRAME_H__
#define __FRAME_H__

#define RYU_RENDERER_MACROS

#include "glad/gl.h"

#include "common/Macros.h"
#include "graphics/Texture2d.h"

namespace RyuRenderer::Graphics
{
    class Frame
    {
    public:
        Frame()
        {
            glGenFramebuffers(1, &id);
        }

        Frame(const Frame& other) = delete;

        Frame(Frame&& other) noexcept :
            id(other.id)
        {
            other.id = 0;
        }

        ~Frame()
        {
            if (IsUsing())
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                lastestUsedFrameId = 0;
            }

            if (id != 0)
            {
                glDeleteFramebuffers(1, &id);
                id = 0;
            }
        }

        Frame& operator=(Frame& other) = delete;

        Frame& operator=(Frame&& other) noexcept
        {
            if (this == &other)
                return *this;

            id = other.id;
            other.id = 0;
            return *this;
        }

        bool IsValid()
        {
            return
                id != 0 &&
                frameCompleted;
        }

        bool IsUsing()
        {
            if (id == 0)
                return false;

            if (IsCleanMode)
            {
                if (lastestUsedFrameId == 0)
                    return false;
                return lastestUsedFrameId == id;
            }

            GLint fboId = 0;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fboId);
            if (fboId == 0)
                return false;
            return fboId == id;
        }

        bool Attach(Texture2d* t, GLint colorAttachmentIdx)
        {
            if (!t || !t->IsValid())
                return false;
            if (colorAttachmentIdx >= GetMaxColorAttachmentAmount())
                return false;

            frameCompleted = false;

            glBindFramebuffer(GL_FRAMEBUFFER, id);
            lastestUsedFrameId = id;

            glFramebufferTexture2D(GL_FRAMEBUFFER, GetColorAttachmentId(colorAttachmentIdx), GL_TEXTURE_2D, t->GetId(), 0);

            frameCompleted = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            lastestUsedFrameId = 0;

            if (!frameCompleted)
            {
                std::cerr << "frame buffer status error." << std::endl;
                return false;
            }
            return true;
        }

        bool Use()
        {
            if (!IsValid())
                return false;

            if (IsUsing())
                return true;

            glBindFramebuffer(GL_FRAMEBUFFER, id);
            lastestUsedFrameId = id;
            return true;
        }

        static GLint GetColorAttachmentId(GLint colorAttachmentIdx)
        {
            return GL_COLOR_ATTACHMENT0 + colorAttachmentIdx;
        }

        static GLint GetColorAttachmentIdx(GLint colorAttachmentId)
        {
            return colorAttachmentId - GL_COLOR_ATTACHMENT0;
        }

        inline static bool IsCleanMode = true;
    private:
        static GLint GetMaxColorAttachmentAmount()
        {
            if (maxColorAttachmentAmount < 0)
            {
                glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachmentAmount);
            }

            return maxColorAttachmentAmount;
        }

        GLuint id = 0;
        bool frameCompleted = false;

        inline static GLint maxColorAttachmentAmount = -1;
        inline static GLuint lastestUsedFrameId = 0;
    };
}

#undef RYU_RENDERER_MACROS

#endif