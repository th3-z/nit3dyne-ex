#include <nit3dyne/core/display.h>
#include <nit3dyne/graphics/shader.h>
#include <nit3dyne/camera/cameraFree.h>
#include <nit3dyne/core/resourceCache.h>
#include <nit3dyne/graphics/billboard.h>


int main() {
    n3d::Display::init();
    n3d::Input::init(n3d::Display::window);

    n3d::Shader shader("shaders/billboard.vert", "shaders/billboard.frag");
    shader.use();
    shader.setUniform("tex", 0);

    n3d::Shader postShader("shaders/post.vert", "shaders/post.frag");
    postShader.use();
    postShader.setUniform("tex", 0);
    postShader.setUniform("texDither", 1);

    n3d::CameraFree camera(85.f, n3d::Display::viewPort);

    n3d::ResourceCache<n3d::Texture> textureCache;
    n3d::Billboard billboard(std::pair<float, float>(1.f, 1.f), textureCache.loadResource("error"));

    while (!n3d::Display::shouldClose) {
        n3d::Display::update();

        if (n3d::Input::getKey(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(n3d::Display::window, true);

        camera.update();

        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getView();

        // Render scene
        shader.use();
        shader.setUniform("vp", camera.projection * view);
        shader.setUniform("camera_right", glm::vec3(view[0][0], view[1][0], view[2][0]));
        shader.setUniform("camera_up", glm::vec3(view[0][1], view[1][1], view[2][1]));
        shader.setUniform("position", billboard.position);
        shader.setUniform("size", billboard.size);
        shader.setUniform("viewScale", billboard.viewScale);
        billboard.draw(shader);

        n3d::Display::flip(postShader);
        n3d::Input::update();
    }

    n3d::Display::destroy();
}