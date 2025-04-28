#include "graphics/Mesh.h"

namespace RyuRenderer::Graphics
{
    Mesh::Mesh(Mesh&& other) noexcept
    {
        Clear();
        VAOId = other.VAOId;
        elementSize = other.elementSize;
        VBOId = other.VBOId;
        EBOId = other.EBOId;
        other.VAOId = 0;
        other.elementSize = 0;
        other.VBOId = 0;
        other.EBOId = 0;
    }

    Mesh::~Mesh()
    {
        Clear();
    }

    Mesh& Mesh::operator=(Mesh&& other) noexcept
    {
        if (this == &other)
            return *this;

        Clear();
        VAOId = other.VAOId;
        elementSize = other.elementSize;
        VBOId = other.VBOId;
        EBOId = other.EBOId;
        other.VAOId = 0;
        other.elementSize = 0;
        other.VBOId = 0;
        other.EBOId = 0;
        return *this;
    }

    bool Mesh::IsValid() const
    {
        return VAOId != 0 &&
            elementSize != 0 &&
            VBOId != 0 &&
            EBOId != 0;
    }

    bool Mesh::IsUsing() const
    {
        if (VAOId == 0)
            return false;

        if (IsCleanMode)
        {
            if (lastestUsedVAOId == 0)
                return false;

            return lastestUsedVAOId == VAOId;
        }

        GLint currentVAO;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
        return currentVAO == VAOId;
    }

    void Mesh::Draw() const
    {
        if (!IsValid())
            return;

        if (!IsUsing())
        {
            glBindVertexArray(VAOId);
            lastestUsedVAOId = VAOId;
        }

        glDrawElements(GL_TRIANGLES, elementSize, GL_UNSIGNED_INT, 0);
    }

    void Mesh::Clear()
    {
        elementSize = 0;

        if (VAOId != 0)
        {
            if (IsUsing())
            {
                glBindVertexArray(0);
                lastestUsedVAOId = 0;
            }

            glDeleteVertexArrays(1, &VAOId);
            VAOId = 0;
        }

        if (VBOId != 0)
        {
            GLint currentVBO;
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentVBO);
            if (currentVBO == VBOId)
            {
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            glDeleteBuffers(1, &VBOId);
            VBOId = 0;
        }

        if (EBOId != 0)
        {
            GLint currentEBO;
            glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentEBO);
            if (currentEBO == EBOId)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }

            glDeleteBuffers(1, &EBOId);
            EBOId = 0;
        }
    }

    GLint Mesh::GetMaxAttributeAmount()
    {
        if (maxAttributeAmount < 0)
        {
            glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributeAmount);
        }

        return maxAttributeAmount;
    }
}