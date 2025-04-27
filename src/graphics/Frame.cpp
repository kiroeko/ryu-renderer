#include "graphics/Frame.h"

namespace RyuRenderer::Graphics
{
    Frame::Frame(Texture2d* initTexture, GLint initColorAttachmentIdx)
    {
        glGenFramebuffers(1, &id);
        Attach(initTexture, initColorAttachmentIdx);
    }

    Frame::Frame(Frame&& other) noexcept
    {
        Clear();
        id = other.id;
        frameCompleted = other.frameCompleted;
        other.id = 0;
        other.frameCompleted = false;
    }

    Frame::~Frame()
    {
        Clear();
    }

    Frame& Frame::operator=(Frame&& other) noexcept
    {
        if (this == &other)
            return *this;

        Clear();
        id = other.id;
        frameCompleted = other.frameCompleted;
        other.id = 0;
        other.frameCompleted = false;
        return *this;
    }

    bool Frame::IsValid() const
    {
        return id != 0;
    }

    bool Frame::IsCompleted() const
    {
        return
            IsValid() &&
            frameCompleted;
    }

    bool Frame::IsUsing() const
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

    bool Frame::Attach(Texture2d* t, GLint colorAttachmentIdx)
    {
        if (!t || !t->IsValid())
            return false;
        if (colorAttachmentIdx < 0 ||
            colorAttachmentIdx >= GetMaxColorAttachmentAmount())
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

    bool Frame::Use() const
    {
        if (!IsCompleted())
            return false;

        if (IsUsing())
            return true;

        glBindFramebuffer(GL_FRAMEBUFFER, id);
        lastestUsedFrameId = id;
        return true;
    }

    void Frame::Unuse()
    {
        if (IsCleanMode)
        {
            if (lastestUsedFrameId == 0)
                return;
        }
        else
        {
            GLint fboId = 0;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fboId);
            if (fboId == 0)
                return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        lastestUsedFrameId = 0;
    }

    GLint Frame::GetColorAttachmentId(GLint colorAttachmentIdx)
    {
        return GL_COLOR_ATTACHMENT0 + colorAttachmentIdx;
    }

    GLint Frame::GetColorAttachmentIdx(GLint colorAttachmentId)
    {
        return colorAttachmentId - GL_COLOR_ATTACHMENT0;
    }

    void Frame::Clear()
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

    GLint Frame::GetMaxColorAttachmentAmount()
    {
        if (maxColorAttachmentAmount < 0)
        {
            glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachmentAmount);
        }

        return maxColorAttachmentAmount;
    }
}