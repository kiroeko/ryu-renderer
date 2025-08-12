#ifndef __MODELVIEWPHONGBLINNPIPELINE_H__
#define __MODELVIEWPHONGBLINNPIPELINE_H__

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
#include "graphics/scene/Scene.h"
#include "graphics/ShaderManager.h"

namespace RyuRenderer::App::RenderPipeline
{
    class DefaultPhongBlinnMaterialPipeline : public IRenderPipeline
    {
    public:
        DefaultPhongBlinnMaterialPipeline() = default;

        void Init() override
        {
            // init camera
            MainScene.Camera = Graphics::Scene::Camera(
                glm::vec3(0.0f, 1.0f, 6.0f),
                glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                0.01f,
                1000000.f,
                60.f,
                (float)App::GetInstance().GetWindowWidth() / App::GetInstance().GetWindowHeight(),
                true
            );
            
            // init light objects
            MainScene.DirectionLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
            MainScene.DirectionLight.Transformer.RotateTo(Graphics::Scene::Scene::GetDownDirection());
            
            MainScene.PointLights.emplace_back(Graphics::Scene::PointLight(
                glm::vec3{ 1.0f, 1.0f, 1.0f },
                glm::vec3{ 0.f, 0.f, -3.f }
            ));
            MainScene.PointLights.emplace_back(Graphics::Scene::PointLight(
                glm::vec3{ 1.0f, 1.0f, 1.0f },
                glm::vec3{ 0.f, 0.f, 3.f }
            ));
            MainScene.PointLights.emplace_back(Graphics::Scene::PointLight(
                glm::vec3{ 1.0f, 1.0f, 1.0f },
                glm::vec3{ -3.f, 0.f, 0.f }
            ));
            MainScene.PointLights.emplace_back(Graphics::Scene::PointLight(
                glm::vec3{ 1.0f, 1.0f, 1.0f },
                glm::vec3{ 3.f, 0.f, 0.f }
            ));
            MainScene.PointLights.emplace_back(Graphics::Scene::PointLight(
                glm::vec3{ 1.0f, 1.0f, 1.0f },
                glm::vec3{ 0.f, 3.f, 0.f }
            ));
            MainScene.PointLights.emplace_back(Graphics::Scene::PointLight(
                glm::vec3{ 1.0f, 1.0f, 1.0f },
                glm::vec3{ 0.f, -3.f, 0.f }
            ));

            MainScene.SpotLights.emplace_back(Graphics::Scene::SpotLight(
                glm::vec3{ 1.0f, 1.0f, 1.0f },
                glm::vec3{ 0.f, 4.0f, -0.5f },
                glm::vec3{ 0.f, -1.f, 0.f }
            ));

            // load model
            MainScene.Load(
                "res/models/backpack/backpack.obj",
                true,
                RyuRenderer::Graphics::Scene::Scene::SceneMaterialTypeEnum::PHONG_BLINN);

            // init meshes
            MainScene.LightMeshes.emplace_back(Graphics::Mesh(
                std::vector<GLuint>{
                    // front
                    0, 1, 2,
                    2, 3, 0,
                    // back
                    4, 5, 6,
                    6, 7, 4,
                    // left
                    0, 3, 7,
                    7, 4, 0,
                    // right
                    1, 5, 6,
                    6, 2, 1,
                    // down
                    0, 1, 5,
                    5, 4, 0,
                    // up
                    3, 2, 6,
                    6, 7, 3
                }, // indexes
                std::vector<std::array<float, 3>>{
                    {  -0.1f, -0.1f, -0.1f },
                    { 0.1f, -0.1f, -0.1f },
                    { 0.1f,  0.1f, -0.1f },
                    { -0.1f,  0.1f, -0.1f },
                    { -0.1f, -0.1f,  0.1f },
                    { 0.1f, -0.1f, 0.1f },
                    { 0.1f, 0.1f, 0.1f },
                    { -0.1f,  0.1f,  0.1f },
                } // Position
            ));

            // init shaders
            MainScene.LightShader = Graphics::ShaderManager::GetInstance().FindOrCreate("res/shaders/3d-basic-color.vert", "res/shaders/3d-basic-color.frag");

            // Other settings
            App::GetInstance().EventPublisher.RegisterHandler(this, &DefaultPhongBlinnMaterialPipeline::OnWindowResize);
            App::GetInstance().EventPublisher.RegisterHandler(this, &DefaultPhongBlinnMaterialPipeline::OnMouseMove);
            App::GetInstance().EventPublisher.RegisterHandler(this, &DefaultPhongBlinnMaterialPipeline::OnKeyEvent);
        }

        void Tick(double deltaTimeInS) override
        {
            MainScene.OnTick(deltaTimeInS);
            MainScene.Draw();
        }

        RyuRenderer::Graphics::Scene::Scene MainScene;
    private:
        void OnWindowResize(const Events::WindowEvent& e)
        {
            if (e.Event != Events::WindowEvent::EventType::WINDOW_RESIZE)
                return;

            float aspectRatio = (float)e.Width / e.Height;
            MainScene.OnWindowResize(aspectRatio);
        }

        void OnMouseMove(const Events::MouseEvent& e)
        {
            MainScene.OnMouseMove(e);
        }

        void OnKeyEvent(const Events::KeyEvent& e)
        {
            MainScene.OnKeyEvent(e);
        }
    };
}

#endif