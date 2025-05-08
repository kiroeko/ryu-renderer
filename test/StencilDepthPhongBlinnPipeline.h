#ifndef __STENCILDEPTHPHONGBLINNPIPELINE_H__
#define __STENCILDEPTHPHONGBLINNPIPELINE_H__

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"

#include <memory>
#include <vector>

#include "app/App.h"
#include "app/events/WindowEvent.h"
#include "app/events/MouseEvent.h"
#include "app/events/KeyEvent.h"
#include "app/render-pipeline/IRenderPipeline.h"
#include "graphics/Frame.h"
#include "graphics/Mesh.h"
#include "graphics/Shader.h"
#include "graphics/ShaderManager.h"
#include "graphics/Texture2D.h"
#include "graphics/TextureManager.h"
#include "graphics/scene/Camera.h"
#include "graphics/scene/DirectionalLight.h"
#include "graphics/scene/Transform.h"
#include "graphics/scene/Scene.h"

namespace RyuRenderer::App::RenderPipeline
{
    class StencilDepthPhongBlinnPipeline : public IRenderPipeline
    {
    public:
        StencilDepthPhongBlinnPipeline() = default;

        void Init() override
        {
            boxMeshes.emplace_back(Graphics::Mesh(
                std::vector<GLuint>{
                    // front
                    0, 1, 2,
                    2, 3, 0,
                    // back
                    4, 5, 6,
                    6, 7, 4,
                    // left
                    8, 9, 10,
                    10, 11, 8,
                    // right
                    12, 13, 14,
                    14, 15, 12,
                    // bottom
                    16, 17, 18,
                    18, 19, 16,
                    // top
                    20, 21, 22,
                    22, 23, 20
                }, // indexes
                std::vector<std::array<float, 3>>{
                    // front
                    { -0.5f, -0.5f, -0.5f },  // 0
                    { 0.5f, -0.5f, -0.5f },   // 1
                    { 0.5f,  0.5f, -0.5f },   // 2
                    { -0.5f,  0.5f, -0.5f },  // 3
                    // back
                    { -0.5f, -0.5f,  0.5f },  // 4
                    { 0.5f, -0.5f,  0.5f },   // 5
                    { 0.5f,  0.5f,  0.5f },   // 6
                    { -0.5f,  0.5f,  0.5f },  // 7
                    // left
                    { -0.5f,  0.5f,  0.5f },  // 8
                    { -0.5f,  0.5f, -0.5f },  // 9
                    { -0.5f, -0.5f, -0.5f },  // 10
                    { -0.5f, -0.5f,  0.5f },  // 11
                    // right
                    { 0.5f,  0.5f,  0.5f },   // 12
                    { 0.5f,  0.5f, -0.5f },   // 13
                    { 0.5f, -0.5f, -0.5f },   // 14
                    { 0.5f, -0.5f,  0.5f },   // 15
                    // bottom
                    { -0.5f, -0.5f, -0.5f },  // 16
                    { 0.5f, -0.5f, -0.5f },   // 17
                    { 0.5f, -0.5f,  0.5f },   // 18
                    { -0.5f, -0.5f,  0.5f },  // 19
                    // top
                    { -0.5f,  0.5f, -0.5f },  // 20
                    { 0.5f,  0.5f, -0.5f },   // 21
                    { 0.5f,  0.5f,  0.5f },   // 22
                    { -0.5f,  0.5f,  0.5f }   // 23
                }, // Position
                std::vector<std::array<float, 3>>{
                    // front
                    { 0.0f, 0.0f, -1.0f },   // 0
                    { 0.0f,  0.0f, -1.0f },  // 1
                    { 0.0f,  0.0f, -1.0f },  // 2
                    { 0.0f,  0.0f, -1.0f },  // 3
                    // back
                    { 0.0f,  0.0f,  1.0f },  // 4
                    { 0.0f,  0.0f,  1.0f },  // 5
                    { 0.0f,  0.0f,  1.0f },  // 6
                    { 0.0f,  0.0f,  1.0f },  // 7
                    // left
                    { -1.0f,  0.0f,  0.0f },  // 8
                    { -1.0f,  0.0f,  0.0f },  // 9
                    { -1.0f,  0.0f,  0.0f },  // 10
                    { -1.0f,  0.0f,  0.0f },  // 11
                    // right
                    { 1.0f,  0.0f,  0.0f },  // 12
                    { 1.0f,  0.0f,  0.0f },  // 13
                    { 1.0f,  0.0f,  0.0f },  // 14
                    { 1.0f,  0.0f,  0.0f },  // 15
                    // bottom
                    { 0.0f, -1.0f,  0.0f },  // 16
                    { 0.0f, -1.0f,  0.0f },  // 17
                    { 0.0f, -1.0f,  0.0f },  // 18
                    { 0.0f, -1.0f,  0.0f },  // 19
                    // top
                    { 0.0f,  1.0f,  0.0f },  // 20
                    { 0.0f,  1.0f,  0.0f },  // 21
                    { 0.0f,  1.0f,  0.0f },  // 22
                    { 0.0f,  1.0f,  0.0f }   // 23
                }, // Normal
                std::vector<std::array<float, 2>>{
                    // front
                    { 0.0f, 0.0f },
                    { 1.0f, 0.0f },
                    { 1.0f, 1.0f },
                    { 0.0f, 1.0f },
                    // back
                    { 0.0f, 0.0f },
                    { 1.0f, 0.0f },
                    { 1.0f, 1.0f },
                    { 0.0f, 1.0f },
                    // left
                    { 0.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 0.0f },
                    { 0.0f, 0.0f },
                    // right
                    { 0.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 0.0f },
                    { 0.0f, 0.0f },
                    // bottom
                    { 0.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 0.0f },
                    { 0.0f, 0.0f },
                    // top
                    { 0.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 0.0f },
                    { 0.0f, 0.0f }
                } // TexCoord
            ));

            // init textures
            boxDiffuse = Graphics::TextureManager::GetInstance().FindOrCreate2d("res/textures/box_diffuse.jpg", 0);
            if (boxDiffuse)
                boxDiffuse->Use();

            boxSpecular = Graphics::TextureManager::GetInstance().FindOrCreate2d("res/textures/box_specular.jpg", 1);
            if (boxSpecular)
                boxSpecular->Use();

            boxEmission = Graphics::TextureManager::GetInstance().FindOrCreate2d("res/textures/box_no_emission.jpg", 2);
            if (boxEmission)
                boxEmission->Use();

            // init shader
            boxShader = Graphics::ShaderManager::GetInstance().Create("res/shaders/3d-blinn-phong-material.vert", "res/shaders/3d-blinn-phong-material.frag");
            if (boxShader)
            {
                boxShader->Use();
                boxShader->SetUniform("material.ambient", boxAmbient);
                boxShader->SetUniform("hasDiffuse", true);
                boxShader->SetUniform("material.diffuse", 0);
                boxShader->SetUniform("hasSpecular", true);
                boxShader->SetUniform("material.specular", 1);
                boxShader->SetUniform("hasEmission", true);
                boxShader->SetUniform("material.emission", 2);
                boxShader->SetUniform("material.shininess", boxShininess);
            }
            outlineShader = Graphics::ShaderManager::GetInstance().Create("res/shaders/3d-basic-color.vert", "res/shaders/3d-basic-color.frag");
            if (outlineShader)
            {
                outlineShader->Use();
                outlineShader->SetUniform("color", glm::vec3(0.4f, 0.2f, 0.8f));
            }

            // init camera
            camera = Graphics::Scene::Camera(
                glm::vec3(0.0f, 1.0f, 6.0f),
                glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                0.01f,
                1000000.f,
                60.f,
                (float)App::GetInstance().GetWindowWidth() / App::GetInstance().GetWindowHeight(),
                true
            );

            // init mvp
            view = camera.GetView();
            projection = camera.GetProjection();

            // init object trs
            boxATramsform.Rotate(Graphics::Scene::Scene::GetZAxisDirection(), 30.f);
            boxBTramsform.Rotate(Graphics::Scene::Scene::GetZAxisDirection(), 30.f);
            boxBTramsform.ScaleTo(glm::vec3(1.1f));
            planeTramsform.MoveTo(glm::vec3(0.f, -0.4f, 0.f));
            planeTramsform.ScaleTo(glm::vec3(10.f, 0.01f, 10.f));

            // init light objects
            directionLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);

            // Other settings
            App::GetInstance().EventPublisher.RegisterHandler(this, &StencilDepthPhongBlinnPipeline::OnWindowResize);
            App::GetInstance().EventPublisher.RegisterHandler(this, &StencilDepthPhongBlinnPipeline::OnMouseMove);
            App::GetInstance().EventPublisher.RegisterHandler(this, &StencilDepthPhongBlinnPipeline::OnKeyEvent);
        }

        void Tick(double deltaTimeInS) override
        {
            if (!boxShader)
                return;

            // Update camera
            camera.OnTick(deltaTimeInS);
            view = camera.GetView();

            glStencilMask(0x00);

            // draw plane
            const auto mp = planeTramsform.GetMatrix();
            boxShader->Use();
            boxShader->SetUniform("view", view);
            boxShader->SetUniform("projection", projection);
            boxShader->SetUniform("directionalLight.color", directionLight.Color);
            boxShader->SetUniform("directionalLight.viewDirection", glm::transpose(glm::inverse(glm::mat3(view))) * directionLight.Transformer.GetFrontDirection());
            glm::mat3 viewNormalMatrix = glm::transpose(glm::inverse(glm::mat3(view * mp)));
            boxShader->SetUniform("model", mp);
            boxShader->SetUniform("viewNormalMatrix", viewNormalMatrix);
            for (size_t j = 0; j < boxMeshes.size(); ++j)
            {
                boxMeshes[j].Draw();
            }

            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            glStencilMask(0xFF);

            // draw box a
            const auto ma = boxATramsform.GetMatrix();
            boxShader->Use();
            boxShader->SetUniform("view", view);
            boxShader->SetUniform("projection", projection);
            boxShader->SetUniform("directionalLight.color", directionLight.Color);
            boxShader->SetUniform("directionalLight.viewDirection", glm::transpose(glm::inverse(glm::mat3(view))) * directionLight.Transformer.GetFrontDirection());
            viewNormalMatrix = glm::transpose(glm::inverse(glm::mat3(view * ma)));
            boxShader->SetUniform("model", ma);
            boxShader->SetUniform("viewNormalMatrix", viewNormalMatrix);
            for (size_t j = 0; j < boxMeshes.size(); ++j)
            {
                boxMeshes[j].Draw();
            }

            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);

            // draw box b
            const auto mb = boxBTramsform.GetMatrix();
            outlineShader->Use();
            outlineShader->SetUniform("view", view);
            outlineShader->SetUniform("projection", projection);
            outlineShader->SetUniform("model", mb);

            for (size_t j = 0; j < boxMeshes.size(); ++j)
            {
                boxMeshes[j].Draw();
            }

            glEnable(GL_DEPTH_TEST);
            glStencilMask(0xFF);
        }
    private:
        void OnWindowResize(const Events::WindowEvent& e)
        {
            if (e.Event != Events::WindowEvent::EventType::WINDOW_RESIZE)
                return;

            camera.SetAspectRatio((float)e.Width / e.Height);
            projection = camera.GetProjection();
        }

        void OnMouseMove(const Events::MouseEvent& e)
        {
            camera.OnMouseMove(e);
        }

        void OnKeyEvent(const Events::KeyEvent& e)
        {
            camera.OnKeyEvent(e);
        }

        std::shared_ptr<Graphics::Shader> boxShader;
        std::shared_ptr<Graphics::Shader> outlineShader;

        std::vector<Graphics::Mesh> boxMeshes;

        // lights
        Graphics::Scene::DirectionalLight directionLight = { glm::vec3(0.0f, 0.0f, 0.0f) };

        // box
        glm::vec3 boxAmbient = { 0.2f, 0.2f, 0.2f };
        std::shared_ptr<Graphics::Texture2d> boxDiffuse;
        std::shared_ptr<Graphics::Texture2d> boxSpecular;
        std::shared_ptr<Graphics::Texture2d> boxEmission;
        float boxShininess = 128.f;

        Graphics::Scene::Transform planeTramsform;
        Graphics::Scene::Transform boxATramsform;
        Graphics::Scene::Transform boxBTramsform;

        // camera
        glm::mat4 view = glm::identity<glm::mat4>();
        glm::mat4 projection = glm::identity<glm::mat4>();
        Graphics::Scene::Camera camera;
    };
}

#endif