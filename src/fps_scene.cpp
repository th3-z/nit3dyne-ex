#include <iostream>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <soloud/include/soloud.h>
#include <soloud/include/soloud_wav.h>
#include <entt/entt.hpp>
#include <memory>
#include <array>

#include <nit3dyne/camera/cameraFps.h>
#include <nit3dyne/graphics/mesh_static.h>
#include <nit3dyne/graphics/shader.h>
#include <nit3dyne/graphics/texture.h>
#include <nit3dyne/graphics/terrain.h>
#include <nit3dyne/core/input.h>
#include <nit3dyne/graphics/skybox.h>
#include <nit3dyne/core/resourceCache.h>
#include <nit3dyne/core/math.h>

#include <nit3dyne/core/font.h>
#include <nit3dyne/graphics/model.h>
#include <nit3dyne/graphics/lines.h>


int main() {
    n3d::Display::init();
    n3d::Input::init(n3d::Display::window);

    // TODO: Move into screen.cpp
    n3d::Shader postShader("shaders/post.vert", "shaders/post.frag");
    postShader.use();
    postShader.setUniform("tex", 0);
    postShader.setUniform("texDither", 1);
    n3d::Texture textureDither("dith");

    // Skybox shader
    n3d::Shader shaderSkybox("shaders/skybox.vert", "shaders/skybox.frag");
    shaderSkybox.use();
    shaderSkybox.setUniform("skybox", 0);

    // Test font renderer
    n3d::Font font("This text should not be upside down anymore!");

    // Main shader program
    n3d::Shader shaderAnim("shaders/vertex-skinned.vert", "shaders/fragment.frag");
    shaderAnim.use();
    shaderAnim.setUniform("tex", 0);

    n3d::Shader shaderUnlit("shaders/vertex.vert", "shaders/fragment-unlit.frag");
    shaderAnim.use();
    shaderAnim.setUniform("tex", 0);

    n3d::Shader shaderTerrain("shaders/terrain.vert", "shaders/terrain.frag");
    shaderTerrain.use();
    shaderTerrain.setUniform("tex", 0);

    n3d::Shader shaderNormals("shaders/normals.vert", "shaders/normals.frag", "shaders/normals.geo");

    n3d::Shader shaderStatic("shaders/vertex.vert", "shaders/fragment.frag");
    shaderStatic.use();
    shaderStatic.setUniform("tex", 0);

    n3d::Shader shaderLine("shaders/line.vert", "shaders/line.frag");

    n3d::CameraFps camera(85.f, n3d::Display::viewPort);

    // Scene
    n3d::DirectionalLight dLight = n3d::DirectionalLight();
    shaderStatic.use();
    shaderStatic.setDirectionalLight(dLight);
    shaderAnim.use();
    shaderAnim.setDirectionalLight(dLight);
    shaderTerrain.use();
    shaderTerrain.setDirectionalLight(dLight);

    n3d::ResourceCache<n3d::Texture> textureCache;
    n3d::ResourceCache<n3d::MeshAnimated> meshAnimCache;
    n3d::ResourceCache<n3d::MeshStatic> meshCache;

    n3d::SpotLight sLight = n3d::SpotLight();
    shaderStatic.use();
    shaderStatic.setSpotLight(sLight);
    shaderAnim.use();
    shaderAnim.setSpotLight(sLight);

    n3d::Skybox skybox("test");
    n3d::Terrain test = n3d::Terrain("ny40");
    std::shared_ptr<n3d::Texture> terrainTexture = textureCache.loadResource("ny40");

    std::vector<n3d::Line> axisData;
    axisData.push_back(n3d::Line{
            {0.f, 0.f, 0.f}, {1.f, 0.f, 0.f},
            {500.f, 0.f, 0.f}, {0.f, 0.f, 0.f}
    });
    axisData.push_back(n3d::Line{
            {0.f, 0.f, 0.f}, {0.f, 1.f, 0.f},
            {0.f, 500.f, 0.f}, {0.f, 0.f, 0.f}
    });
    axisData.push_back(n3d::Line{
            {0.f, 0.f, 0.f}, {0.f, 0.f, 1.f},
            {0.f, 0.f, 500.f}, {0.f, 0.f, 0.f}
    });
    n3d::Lines axis(axisData);

    std::array<std::string, 8> propNames = {
        "desk", "chair", "axe", "speaker", "switch", "stg44", "m4a1", "cube"};
    std::vector<std::unique_ptr<n3d::Model>> props;

    for (size_t i = 0; i < propNames.size(); ++i) {
        props.emplace_back(std::make_unique<n3d::Model>(
                n3d::Model(meshCache.loadResource(propNames[i]), textureCache.loadResource(propNames[i]))));
        props[i]->translate(i * 5.f, 2.f, -2.f);
    }

    n3d::Model plane(meshCache.loadResource("plane"), textureCache.loadResource("red"));
    plane.setMaterial(n3d::Materials::metallic);
    plane.scale(7.f, 0.f, 7.f);

    n3d::Model sphere(meshCache.loadResource("sphere"), textureCache.loadResource("red"));
    sphere.translate(0.f, 5.f, 0.f);
    sphere.scale(1.f, 1.f, 1.f);

    n3d::Model stg(meshAnimCache.loadResource("stg44"), textureCache.loadResource("stg44"));
    stg.translate(2.f, 2.f, 0.f);

    n3d::Model viewModel(meshCache.loadResource("m4a1"), textureCache.loadResource("m4a1"));
    viewModel.translate(0.45f, -0.25f, -0.65f);
    viewModel.rotate(95.f, 0.f, 1.f, 0.f);
    viewModel.rotate(-5.f, 0.f, 0.f, 1.f);
    viewModel.rotate(-5.f, 0.f, 1.f, 0.f);
    viewModel.scale(.45f, .45f, .45f);

    // Audio
    SoLoud::Soloud soloud; // Engine core
    SoLoud::Wav sample;    // One sample

    // Initialize SoLoud, miniaudio backend
    soloud.init(SoLoud::Soloud::CLIP_ROUNDOFF | SoLoud::Soloud::ENABLE_VISUALIZATION,
                SoLoud::Soloud::BACKENDS::MINIAUDIO);
    soloud.setGlobalVolume(.9f);

    sample.load("res/sound/avril14.ogg");
    sample.setLooping(true);
    sample.set3dMinMaxDistance(1, 30);
    sample.set3dAttenuation(SoLoud::AudioSource::EXPONENTIAL_DISTANCE, 0.5);
    SoLoud::handle sampleHandle = soloud.play3d(sample, 0.f, 0.f, 0.f);
    soloud.set3dSourceParameters(sampleHandle, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f);

    while (!n3d::Display::shouldClose) {
        n3d::Display::update();

        if (n3d::Input::getKey(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(n3d::Display::window, true);

        camera.update();

        // Clear FB
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render scene
        shaderStatic.use();
        shaderStatic.setUniform("dLight.direction", camera.getView() * dLight.direction);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, terrainTexture->handle);
        test.draw(shaderTerrain, camera.projection, camera.getView());
        plane.draw(shaderStatic, camera.projection, camera.getView());
        sphere.draw(shaderStatic, camera.projection, camera.getView());

        for (auto &prop : props) {
            prop->rotate((360.f * 1.) * (n3d::Display::timeDelta / 10.), 0.f, 1.f, 0.f, false);
            prop->draw(shaderStatic, camera.projection, camera.getView());
        }

        // FIXME: Lighting calculations still need the real camera view mat
        viewModel.draw(shaderUnlit, n3d::mat4(1.f), camera.projection);

        // Update audio
        n3d::vec4 sourcePosView =  (camera.getView() * n3d::mat4(1.f)) * n3d::vec4(0.f,0.f,0.f,1.f);
        float w = sourcePosView.w;
        soloud.set3dSourceParameters(sampleHandle, sourcePosView.x/w, sourcePosView.y/w, sourcePosView.z/w, 0.f, 0.f, 0.f);

        soloud.set3dListenerPosition(0,0,0);
        soloud.update3dAudio();

        // Render skybox
        skybox.draw(shaderSkybox,
                    n3d::mat4(n3d::mat3(camera.getView())),
                    camera.projection);

        axis.draw(shaderLine, camera.projection, camera.getView());

        // Render UI
        font.draw();

        // Animated mesh
        stg.draw(shaderAnim, camera.projection, camera.getView());

        n3d::Display::flip(postShader);
        n3d::Input::update();
    }

    n3d::Display::destroy();

    return 0;
}
