#ifndef __DEFAULTPIPELINE_H__
#define __DEFAULTPIPELINE_H__

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"

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

namespace RyuRenderer::App::RenderPipeline
{
    struct DirectionalLight
    {
        DirectionalLight() = default;

        DirectionalLight(
            const glm::vec3& color,
            const glm::vec3& direction = glm::vec3(0.f, -1.f, 0.f)
        )
        {
            Color = color;
            Direction = direction;
        }

        glm::vec3 Color = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Direction = { 0.0f, -1.0f, 0.0f };
    };

    struct PointLight
    {
        PointLight() = default;

        PointLight(
            const glm::vec3& color,
            const glm::vec3& worldPos = glm::zero<glm::vec3>(),
            float modelScale = 0.2f,
            float attenuationConstant = 1.f,
            float attenuationLinear = 0.09f,
            float attenuationQuadratic = 0.032f
        )
        {
            Color = color;
            WorldPos = worldPos;
            AttenuationConstant = attenuationConstant;
            AttenuationLinear = attenuationLinear;
            AttenuationQuadratic = attenuationQuadratic;
            Model = glm::translate(Model, worldPos);
            Model = glm::scale(Model, glm::vec3(modelScale));
        }

        glm::vec3 Color = { 0.f, 0.f, 0.f };
        glm::vec3 WorldPos = glm::zero<glm::vec3>();
        float AttenuationConstant = 1.f;
        float AttenuationLinear = 0.09f;
        float AttenuationQuadratic = 0.032f;
        glm::mat4 Model = glm::identity<glm::mat4>();
    };

    struct SpotLight
    {
        SpotLight() = default;

        SpotLight(
            const glm::vec3& color,
            const glm::vec3& worldPos = glm::zero<glm::vec3>(),
            const glm::vec3& direction = glm::vec3(0.f, -1.f, 0.f),
            float innerCutOffDegree = 12.5f,
            float outerCutOffDegree = 17.5f,
            float modelScale = 0.2f,
            float attenuationConstant = 1.f,
            float attenuationLinear = 0.045f,
            float attenuationQuadratic = 0.0075f
        )
        {
            Color = color;
            WorldPos = worldPos;
            Direction = direction;
            InnerCutOffCos = glm::cos(glm::radians(innerCutOffDegree));
            OuterCutOffCos = glm::cos(glm::radians(outerCutOffDegree));
            AttenuationConstant = attenuationConstant;
            AttenuationLinear = attenuationLinear;
            AttenuationQuadratic = attenuationQuadratic;
            Model = glm::translate(Model, worldPos);
            Model = glm::scale(Model, glm::vec3(modelScale));
        }

        glm::vec3 Color = { 0.f, 0.f, 0.f };
        glm::vec3 WorldPos = glm::zero<glm::vec3>();
        glm::vec3 Direction = { 0.f, -1.f, 0.f };
        float InnerCutOffCos = 0.976296f;
        float OuterCutOffCos = 0.953717f;
        float AttenuationConstant = 1.f;
        float AttenuationLinear = 0.045f;
        float AttenuationQuadratic = 0.0075f;
        glm::mat4 Model = glm::identity<glm::mat4>();
    };

    class DefaultRenderPipeline : public IRenderPipeline
    {
    public:
        DefaultRenderPipeline() = default;

        void init() override
        {
            // init meshes
            lightMeshes.emplace_back(Graphics::Mesh(
                std::vector<GLuint>{
                    // 前面
                    0, 1, 2,
                    2, 3, 0,
                    // 后面
                    4, 5, 6,
                    6, 7, 4,
                    // 左面
                    0, 3, 7,
                    7, 4, 0,
                    // 右面
                    1, 5, 6,
                    6, 2, 1,
                    // 底面
                    0, 1, 5,
                    5, 4, 0,
                    // 顶面
                    3, 2, 6,
                    6, 7, 3
                }, // indexes
                std::vector<std::array<float, 3>>{
                    {  -0.5f, -0.5f, -0.5f },
                    { 0.5f, -0.5f, -0.5f },
                    { 0.5f,  0.5f, -0.5f },
                    { -0.5f,  0.5f, -0.5f },
                    { -0.5f, -0.5f,  0.5f },
                    { 0.5f, -0.5f, 0.5f },
                    { 0.5f, 0.5f, 0.5f },
                    { -0.5f,  0.5f,  0.5f },
                } // Position
            ));

            boxMeshes.emplace_back(Graphics::Mesh(
                std::vector<GLuint>{
                    // 前面
                    0, 1, 2,
                    2, 3, 0,
                    // 后面
                    4, 5, 6,
                    6, 7, 4,
                    // 左面
                    8, 9, 10,
                    10, 11, 8,
                    // 右面
                    12, 13, 14,
                    14, 15, 12,
                    // 底面
                    16, 17, 18,
                    18, 19, 16,
                    // 顶面
                    20, 21, 22,
                    22, 23, 20
                }, // indexes
                std::vector<std::array<float, 3>>{
                    // 前面
                    { -0.5f, -0.5f, -0.5f },  // 0
                    { 0.5f, -0.5f, -0.5f },   // 1
                    { 0.5f,  0.5f, -0.5f },   // 2
                    { -0.5f,  0.5f, -0.5f },  // 3
                    // 后面
                    { -0.5f, -0.5f,  0.5f },  // 4
                    { 0.5f, -0.5f,  0.5f },   // 5
                    { 0.5f,  0.5f,  0.5f },   // 6
                    { -0.5f,  0.5f,  0.5f },  // 7
                    // 左面
                    { -0.5f,  0.5f,  0.5f },  // 8
                    { -0.5f,  0.5f, -0.5f },  // 9
                    { -0.5f, -0.5f, -0.5f },  // 10
                    { -0.5f, -0.5f,  0.5f },  // 11
                    // 右面
                    { 0.5f,  0.5f,  0.5f },   // 12
                    { 0.5f,  0.5f, -0.5f },   // 13
                    { 0.5f, -0.5f, -0.5f },   // 14
                    { 0.5f, -0.5f,  0.5f },   // 15
                    // 底面
                    { -0.5f, -0.5f, -0.5f },  // 16
                    { 0.5f, -0.5f, -0.5f },   // 17
                    { 0.5f, -0.5f,  0.5f },   // 18
                    { -0.5f, -0.5f,  0.5f },  // 19
                    // 顶面
                    { -0.5f,  0.5f, -0.5f },  // 20
                    { 0.5f,  0.5f, -0.5f },   // 21
                    { 0.5f,  0.5f,  0.5f },   // 22
                    { -0.5f,  0.5f,  0.5f }   // 23
                }, // Position
                std::vector<std::array<float, 3>>{
                    // 前面 (法线)
                    { 0.0f, 0.0f, -1.0f },   // 0
                    { 0.0f,  0.0f, -1.0f },  // 1
                    { 0.0f,  0.0f, -1.0f },  // 2
                    { 0.0f,  0.0f, -1.0f },  // 3
                    // 后面 (法线)
                    { 0.0f,  0.0f,  1.0f },  // 4
                    { 0.0f,  0.0f,  1.0f },  // 5
                    { 0.0f,  0.0f,  1.0f },  // 6
                    { 0.0f,  0.0f,  1.0f },  // 7
                    // 左面 (法线)
                    { -1.0f,  0.0f,  0.0f },  // 8
                    { -1.0f,  0.0f,  0.0f },  // 9
                    { -1.0f,  0.0f,  0.0f },  // 10
                    { -1.0f,  0.0f,  0.0f },  // 11
                    // 右面 (法线)
                    { 1.0f,  0.0f,  0.0f },  // 12
                    { 1.0f,  0.0f,  0.0f },  // 13
                    { 1.0f,  0.0f,  0.0f },  // 14
                    { 1.0f,  0.0f,  0.0f },  // 15
                    // 底面 (法线)
                    { 0.0f, -1.0f,  0.0f },  // 16
                    { 0.0f, -1.0f,  0.0f },  // 17
                    { 0.0f, -1.0f,  0.0f },  // 18
                    { 0.0f, -1.0f,  0.0f },  // 19
                    // 顶面 (法线)
                    { 0.0f,  1.0f,  0.0f },  // 20
                    { 0.0f,  1.0f,  0.0f },  // 21
                    { 0.0f,  1.0f,  0.0f },  // 22
                    { 0.0f,  1.0f,  0.0f }   // 23
                }, // Normal
                std::vector<std::array<float, 2>>{
                    // 前面
                    { 0.0f, 0.0f },
                    { 1.0f, 0.0f },
                    { 1.0f, 1.0f },
                    { 0.0f, 1.0f },
                    // 后面
                    { 0.0f, 0.0f },
                    { 1.0f, 0.0f },
                    { 1.0f, 1.0f },
                    { 0.0f, 1.0f },
                    // 左面
                    { 0.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 0.0f },
                    { 0.0f, 0.0f },
                    // 右面
                    { 0.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 0.0f },
                    { 0.0f, 0.0f },
                    // 底面
                    { 0.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 0.0f },
                    { 0.0f, 0.0f },
                    // 顶面
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
                boxShader->SetUniform("material.diffuse", 0);
                boxShader->SetUniform("material.specular", 1);
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

            pointLights.emplace_back(PointLight(
                glm::vec3{ 1.0f, 1.0f, 1.0f },
                glm::vec3{ 0.f, 0.f, -3.f }
            ));
            pointLights.emplace_back(PointLight(
                glm::vec3{ 0.0f, 1.0f, 0.0f },
                glm::vec3{ 0.f, 0.f, 3.f }
            ));
            pointLights.emplace_back(PointLight(
                glm::vec3{ 0.0f, 0.0f, 1.0f },
                glm::vec3{ -3.f, 0.f, 0.f }
            ));
            pointLights.emplace_back(PointLight(
                glm::vec3{ 1.0f, 0.0f, 0.0f },
                glm::vec3{ 3.f, 0.f, 0.f }
            ));
            pointLights.emplace_back(PointLight(
                glm::vec3{ 0.42f, 0.167f, 0.32f },
                glm::vec3{ 0.f, 3.f, 0.f }
            ));
            pointLights.emplace_back(PointLight(
                glm::vec3{ 0.56f, 0.0f, 0.67f },
                glm::vec3{ 0.f, -3.f, 0.f }
            ));

            // init spot lights
            //spotLights.emplace_back(SpotLight(
            //    glm::vec3{ 1.0f, 1.0f, 1.0f },
            //    glm::vec3{ 0.f, 1.5f, 0.f },
            //    glm::vec3{ 0.f, -1.f, 0.f }
            //));

            // Other settings
            App::GetInstance().EventPublisher.RegisterHandler(this, &DefaultRenderPipeline::OnWindowResize);
            App::GetInstance().EventPublisher.RegisterHandler(this, &DefaultRenderPipeline::OnMouseMove);
            App::GetInstance().EventPublisher.RegisterHandler(this, &DefaultRenderPipeline::OnKeyEvent);
        }

        void tick(double deltaTimeInS) override
        {
            if (!boxShader)
                return;

            // Update camear
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
                lightShader->SetUniform("model", l.Model);
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
                lightShader->SetUniform("model", l.Model);
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
            boxShader->SetUniform("directionalLight.viewDirection", glm::transpose(glm::inverse(glm::mat3(view))) * directionLight.Direction);

            // set point light
            boxShader->SetUniform("activePointLightCount", (int)pointLightCount);
            for (size_t i = 0; i < pointLightCount; ++i)
            {
                auto& l = pointLights[i];

                std::string pointPrefix = "pointLights[" + std::to_string(i) + "].";
                boxShader->SetUniform(pointPrefix + "color", l.Color);
                boxShader->SetUniform(pointPrefix + "viewPos", glm::vec3(view * glm::vec4(l.WorldPos, 1.0f)));
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
                boxShader->SetUniform(spotPrefix + "viewPos", glm::vec3(view * glm::vec4(l.WorldPos, 1.0f)));
                boxShader->SetUniform(spotPrefix + "viewDirection", glm::transpose(glm::inverse(glm::mat3(view))) * l.Direction);
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

        std::vector<RyuRenderer::Graphics::Mesh> lightMeshes;
        std::vector<RyuRenderer::Graphics::Mesh> boxMeshes;

        std::shared_ptr<RyuRenderer::Graphics::Shader> lightShader;
        std::shared_ptr<RyuRenderer::Graphics::Shader> boxShader;

        // lights
        DirectionalLight directionLight = { glm::vec3(0.0f, 0.0f, 0.0f) };

        std::vector<PointLight> pointLights;
        std::vector<SpotLight> spotLights;

        // box
        glm::vec3 boxAmbient = { 0.2f, 0.2f, 0.2f };
        std::shared_ptr<Graphics::ITexture> boxDiffuse;
        std::shared_ptr<Graphics::ITexture> boxSpecular;
        std::shared_ptr<Graphics::ITexture> boxEmission;
        float boxShininess = 128.f;
        std::vector<glm::mat4> modelBoxs;

        // camera
        glm::mat4 view = glm::identity<glm::mat4>();
        glm::mat4 projection = glm::identity<glm::mat4>();

        Graphics::Scene::Camera camera;
    };
}

#endif