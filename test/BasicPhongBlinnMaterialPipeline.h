#ifndef __BASICPHONGBLINNMATERIALPIPELINE_H__
#define __BASICPHONGBLINNMATERIALPIPELINE_H__

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
#include "graphics/scene/PointLight.h"
#include "graphics/scene/SpotLight.h"

namespace RyuRenderer::App::RenderPipeline
{
    class BasicPhongBlinnMaterialPipeline : public IRenderPipeline
    {
    public:
        BasicPhongBlinnMaterialPipeline() = default;

        void Init() override
        {
            // init meshes
            lightMeshes.emplace_back(Graphics::Mesh(
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
                    // bottom
                    0, 1, 5,
                    5, 4, 0,
                    // top
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

            // init light shader
            lightShader = Graphics::ShaderManager::GetInstance().Create("res/shaders/3d-basic-color.vert", "res/shaders/3d-basic-color.frag");

            // init box shader
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

            // init mvp
            view = camera.GetView();
            projection = camera.GetProjection();

            // init box objects
            glm::vec3 cubePositions[] = {
                glm::vec3(0.0f,  0.0f,  0.0f),
                glm::vec3(2.0f,  5.0f, -15.0f),
                glm::vec3(-1.5f, -2.2f, -2.5f),
                glm::vec3(-3.8f, -2.0f, -12.3f),
                glm::vec3(2.4f, -0.4f, -3.5f),
                glm::vec3(-1.7f,  3.0f, -7.5f),
                glm::vec3(1.3f, -5.0f, -2.5f),
                glm::vec3(1.5f,  2.0f, -2.5f),
                glm::vec3(1.5f,  0.2f, -1.5f),
                glm::vec3(-1.3f,  1.0f, -1.5f)
            };

            for (size_t i = 0; i < 10; ++i)
            {
                glm::mat4 model = glm::identity<glm::mat4>();

                float angle = 16.5f * i;
                glm::quat rotationX = glm::angleAxis(
                    glm::radians(angle),
                    glm::vec3(1.0f, 0.0f, 0.0f)
                );
                glm::quat rotationY = glm::angleAxis(
                    glm::radians(angle),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );
                glm::quat rotationZ = glm::angleAxis(
                    glm::radians(angle),
                    glm::vec3(0.0f, 0.0f, 1.0f)
                );
                model = model * glm::mat4_cast(rotationX * rotationY * rotationZ);

                model = glm::translate(model, cubePositions[i]);

                modelBoxs.push_back(model);
            }

            // init light objects
            //directionLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);

            pointLights.emplace_back(Graphics::Scene::PointLight(
                glm::vec3{ 1.0f, 1.0f, 1.0f },
                glm::vec3{ 0.f, 0.f, -3.f }
            ));
            pointLights.emplace_back(Graphics::Scene::PointLight(
                glm::vec3{ 0.0f, 1.0f, 0.0f },
                glm::vec3{ 0.f, 0.f, 3.f }
            ));
            pointLights.emplace_back(Graphics::Scene::PointLight(
                glm::vec3{ 0.0f, 0.0f, 1.0f },
                glm::vec3{ -3.f, 0.f, 0.f }
            ));
            pointLights.emplace_back(Graphics::Scene::PointLight(
                glm::vec3{ 1.0f, 0.0f, 0.0f },
                glm::vec3{ 3.f, 0.f, 0.f }
            ));
            pointLights.emplace_back(Graphics::Scene::PointLight(
                glm::vec3{ 0.42f, 0.167f, 0.32f },
                glm::vec3{ 0.f, 3.f, 0.f }
            ));
            pointLights.emplace_back(Graphics::Scene::PointLight(
                glm::vec3{ 0.56f, 0.0f, 0.67f },
                glm::vec3{ 0.f, -3.f, 0.f }
            ));

            // init spot lights
            //spotLights.emplace_back(Graphics::Scene::SpotLight(
            //    glm::vec3{ 1.0f, 1.0f, 1.0f },
            //    glm::vec3{ 0.f, 1.5f, 0.f },
            //    glm::vec3{ 0.f, -1.f, 0.f }
            //));

            // Other settings
            App::GetInstance().EventPublisher.RegisterHandler(this, &BasicPhongBlinnMaterialPipeline::OnWindowResize);
            App::GetInstance().EventPublisher.RegisterHandler(this, &BasicPhongBlinnMaterialPipeline::OnMouseMove);
            App::GetInstance().EventPublisher.RegisterHandler(this, &BasicPhongBlinnMaterialPipeline::OnKeyEvent);
        }

        void Tick(double deltaTimeInS) override
        {
            if (!boxShader)
                return;

            // Update camera
            camera.OnTick(deltaTimeInS);
            view = camera.GetView();

            // draw light
            lightShader->Use();
            lightShader->SetUniform("view", view);
            lightShader->SetUniform("projection", projection);

            // light lights
            size_t pointLightCount = pointLights.size();
            constexpr size_t maxPointLightCount = 32;
            pointLightCount = pointLightCount > maxPointLightCount ? 32 : pointLightCount;

            for (size_t i = 0; i < pointLightCount; ++i)
            {
                auto& l = pointLights[i];
                lightShader->SetUniform("model", l.Transformer.GetMatrix());
                lightShader->SetUniform("color", l.Color);

                for (int i = 0; i < lightMeshes.size(); ++i)
                {
                    lightMeshes[i].Draw();
                }
            }

            // spot lights
            size_t spotLightCount = spotLights.size();
            constexpr size_t maxSpotLightCount = 32;
            spotLightCount = spotLightCount > maxSpotLightCount ? 32 : spotLightCount;

            for (size_t i = 0; i < spotLightCount; ++i)
            {
                auto& l = spotLights[i];
                lightShader->SetUniform("model", l.Transformer.GetMatrix());
                lightShader->SetUniform("color", l.Color);

                for (int i = 0; i < lightMeshes.size(); ++i)
                {
                    lightMeshes[i].Draw();
                }
            }

            // handle scene objects
            boxShader->Use();
            boxShader->SetUniform("view", view);
            boxShader->SetUniform("projection", projection);

            // set directional light
            boxShader->SetUniform("directionalLight.color", directionLight.Color);
            boxShader->SetUniform("directionalLight.viewDirection", glm::transpose(glm::inverse(glm::mat3(view))) * directionLight.Transformer.GetFrontDirection());

            // set point light
            boxShader->SetUniform("activePointLightCount", (int)pointLightCount);
            for (size_t i = 0; i < pointLightCount; ++i)
            {
                auto& l = pointLights[i];

                std::string pointPrefix = "pointLights[" + std::to_string(i) + "].";
                boxShader->SetUniform(pointPrefix + "color", l.Color);
                boxShader->SetUniform(pointPrefix + "viewPos", glm::vec3(view * glm::vec4(l.Transformer.GetPosition(), 1.0f)));
                boxShader->SetUniform(pointPrefix + "attenuationConstant", l.AttenuationConstant);
                boxShader->SetUniform(pointPrefix + "attenuationLinear", l.AttenuationLinear);
                boxShader->SetUniform(pointPrefix + "attenuationQuadratic", l.AttenuationQuadratic);
            }

            // set spot light
            boxShader->SetUniform("activeSpotLightCount", (int)spotLightCount);
            for (size_t i = 0; i < spotLightCount; ++i)
            {
                auto& l = spotLights[i];

                std::string spotPrefix = "spotLights[" + std::to_string(i) + "].";
                boxShader->SetUniform(spotPrefix + "color", l.Color);
                boxShader->SetUniform(spotPrefix + "viewPos", glm::vec3(view * glm::vec4(l.Transformer.GetPosition(), 1.0f)));
                boxShader->SetUniform(spotPrefix + "viewDirection", glm::transpose(glm::inverse(glm::mat3(view))) * l.Transformer.GetFrontDirection());
                boxShader->SetUniform(spotPrefix + "innerCutOffCos", l.InnerCutOffCos);
                boxShader->SetUniform(spotPrefix + "outerCutOffCos", l.OuterCutOffCos);
                boxShader->SetUniform(spotPrefix + "attenuationConstant", l.AttenuationConstant);
                boxShader->SetUniform(spotPrefix + "attenuationLinear", l.AttenuationLinear);
                boxShader->SetUniform(spotPrefix + "attenuationQuadratic", l.AttenuationQuadratic);
            }

            // draw boxes
            for (size_t i = 0; i < modelBoxs.size(); ++i)
            {
                auto& m = modelBoxs[i];

                // caculate normalMatrix
                glm::mat3 viewNormalMatrix = glm::transpose(glm::inverse(glm::mat3(view * m)));
                boxShader->SetUniform("model", m);
                boxShader->SetUniform("viewNormalMatrix", viewNormalMatrix);

                for (size_t j = 0; j < boxMeshes.size(); ++j)
                {
                    boxMeshes[j].Draw();
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

        std::vector<Graphics::Mesh> lightMeshes;
        std::vector<Graphics::Mesh> boxMeshes;

        std::shared_ptr<Graphics::Shader> lightShader;
        std::shared_ptr<Graphics::Shader> boxShader;

        // lights
        Graphics::Scene::DirectionalLight directionLight = { glm::vec3(0.0f, 0.0f, 0.0f) };

        std::vector<Graphics::Scene::PointLight> pointLights;
        std::vector<Graphics::Scene::SpotLight> spotLights;

        // box
        glm::vec3 boxAmbient = { 0.2f, 0.2f, 0.2f };
        std::shared_ptr<Graphics::Texture2d> boxDiffuse;
        std::shared_ptr<Graphics::Texture2d> boxSpecular;
        std::shared_ptr<Graphics::Texture2d> boxEmission;
        float boxShininess = 128.f;
        std::vector<glm::mat4> modelBoxs;

        // camera
        glm::mat4 view = glm::identity<glm::mat4>();
        glm::mat4 projection = glm::identity<glm::mat4>();

        Graphics::Scene::Camera camera;
    };
}

#endif