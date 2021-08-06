#include <nit3dyne/core/display.h>
#include <nit3dyne/graphics/shader.h>
#include <nit3dyne/camera/cameraFree.h>
#include <nit3dyne/core/resourceCache.h>
#include <nit3dyne/graphics/texture.h>
#include <nit3dyne/graphics/skybox.h>
#include <nit3dyne/graphics/terrain.h>


int main() {
    n3d::Display::init();
    n3d::Input::init(n3d::Display::window);

    n3d::Shader shaderTerrain("shaders/terrain.vert", "shaders/terrain.frag");
    shaderTerrain.use();
    shaderTerrain.setUniform("tex", 0);

    n3d::Shader shaderSky("shaders/skybox.vert", "shaders/skybox.frag");
    shaderSky.use();
    shaderSky.setUniform("skybox", 0);

    n3d::Shader postShader("shaders/post.vert", "shaders/post.frag");
    postShader.use();
    postShader.setUniform("tex", 0);
    postShader.setUniform("texDither", 1);

    n3d::DirectionalLight sun = n3d::DirectionalLight();
    sun.diffuse = glm::vec3(0.8, 0.8, 0.8);
    sun.ambient = glm::vec3(0.1, 0.1, 0.1);
    shaderTerrain.use();
    shaderTerrain.setDirectionalLight(sun);

    n3d::CameraFree camera(85.f, n3d::Display::viewPort);
    n3d::ResourceCache<n3d::Texture> textureCache;

    n3d::Skybox skybox("test");
    n3d::Terrain terrain = n3d::Terrain("ny40");
    std::shared_ptr<n3d::Texture> terrainTexture = textureCache.loadResource("ny40");

    while (!n3d::Display::shouldClose) {
        n3d::Display::update();

        if (n3d::Input::getKey(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(n3d::Display::window, true);

        camera.update();

        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render scene
        shaderTerrain.use();
        shaderTerrain.setUniform("dLight.direction", camera.getView() * sun.direction);

        skybox.draw(
            shaderSky,
            glm::mat4(glm::mat3(camera.getView())),
            camera.projection
        );
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, terrainTexture->handle);
        terrain.draw(shaderTerrain, camera.projection, camera.getView());

        n3d::Display::flip(postShader);
        n3d::Input::update();
    }

    n3d::Display::destroy();
}