#ifndef __BLENDPIPELINE_H__
#define __BLENDPIPELINE_H__

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
    class BlendPipeline : public IRenderPipeline
    {
    public:
        BlendPipeline() = default;

        void Init() override
        {
            quadMeshes.emplace_back(Graphics::Mesh(
                std::vector<GLuint>{
                    // front
                    0, 1, 2,
                    2, 3, 0
                }, // indexes
                std::vector<std::array<float, 3>>{
                    // front
                    { -0.5f, -0.5f, -0.5f },  // 0
                    { 0.5f, -0.5f, -0.5f },   // 1
                    { 0.5f,  0.5f, -0.5f },   // 2
                    { -0.5f,  0.5f, -0.5f },  // 3
                }, // Position
                std::vector<std::array<float, 2>>{
                    // front
                    { 0.0f, 0.0f },
                    { 1.0f, 0.0f },
                    { 1.0f, 1.0f },
                    { 0.0f, 1.0f },
                } // TexCoord
            ));

            // init textures
            grass = Graphics::TextureManager::GetInstance().FindOrCreate2d("res/textures/grass.png", 0);
            if (grass)
                grass->Use();

            // init shader
            texture2dShader = Graphics::ShaderManager::GetInstance().Create("res/shaders/2d-texture.vert", "res/shaders/2d-texture.frag");
            if (texture2dShader)
            {
                texture2dShader->Use();
                texture2dShader->SetUniform("mainTexture", 0);
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
            Graphics::Scene::Transform t1;
            Graphics::Scene::Transform t2;
            Graphics::Scene::Transform t3;
            Graphics::Scene::Transform t4;
            Graphics::Scene::Transform t5;
            t1.MoveTo(glm::vec3(-1.5f, 0.0f, -0.48f));
            t2.MoveTo(glm::vec3(1.5f, 0.0f, 0.51f));
            t3.MoveTo(glm::vec3(0.0f, 0.0f, 0.7f));
            t4.MoveTo(glm::vec3(-0.3f, 0.0f, -2.3f));
            t5.MoveTo(glm::vec3(0.5f, 0.0f, -0.6f));
            transformers.push_back(t1);
            transformers.push_back(t2);
            transformers.push_back(t3);
            transformers.push_back(t4);
            transformers.push_back(t5);

            // Other settings
            App::GetInstance().EventPublisher.RegisterHandler(this, &BlendPipeline::OnWindowResize);
            App::GetInstance().EventPublisher.RegisterHandler(this, &BlendPipeline::OnMouseMove);
            App::GetInstance().EventPublisher.RegisterHandler(this, &BlendPipeline::OnKeyEvent);
        }

        void Tick(double deltaTimeInS) override
        {
            if (!texture2dShader)
                return;

            // Update camera
            camera.OnTick(deltaTimeInS);
            view = camera.GetView();

            // draw 5 grass
            for (size_t i = 0; i < transformers.size(); ++i)
            {
                const auto m = transformers[i].GetMatrix();

                texture2dShader->Use();
                texture2dShader->SetUniform("view", view);
                texture2dShader->SetUniform("projection", projection);
                texture2dShader->SetUniform("model", m);

                for (size_t j = 0; j < quadMeshes.size(); ++j)
                {
                    quadMeshes[j].Draw();
                }
            }
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

        std::shared_ptr<Graphics::Shader> texture2dShader;

        std::vector<Graphics::Mesh> quadMeshes;

        // box
        glm::vec3 boxAmbient = { 0.2f, 0.2f, 0.2f };
        std::shared_ptr<Graphics::Texture2d> grass;
        float boxShininess = 128.f;

        std::vector<Graphics::Scene::Transform> transformers;

        // camera
        glm::mat4 view = glm::identity<glm::mat4>();
        glm::mat4 projection = glm::identity<glm::mat4>();
        Graphics::Scene::Camera camera;
    };
}

#endif