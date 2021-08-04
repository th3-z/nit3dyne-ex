#include <nit3dyne/display.h>
#include <nit3dyne/graphics/shader.h>
#include <nit3dyne/camera/cameraOrbit.h>
#include <nit3dyne/resourceCache.h>
#include <nit3dyne/graphics/mesh.h>
#include <nit3dyne/graphics/mesh_colored.h>
#include <nit3dyne/graphics/model.h>


int main() {
    n3d::Display::init();
    n3d::Input::init(n3d::Display::window);

    n3d::Shader shader("shaders/vertex-colored.vert", "shaders/fragment-colored.frag");

    n3d::Shader postShader("shaders/post.vert", "shaders/post.frag");
    postShader.use();
    postShader.setUniform("tex", 0);
    postShader.setUniform("texDither", 1);

    n3d::DirectionalLight sun = n3d::DirectionalLight();
    sun.diffuse = glm::vec3(0.8, 0.8, 0.8);
    sun.ambient = glm::vec3(0.1, 0.1, 0.1);
    shader.use();
    shader.setDirectionalLight(sun);

    n3d::CameraOrbit camera(85.f, n3d::Display::viewPort);

    n3d::ResourceCache<n3d::MeshColored> meshCache;

    n3d::Model model(meshCache.loadResource("nit3dyne"), nullptr);

    while (!n3d::Display::shouldClose) {
        n3d::Display::update();

        if (n3d::Input::getKey(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(n3d::Display::window, true);

        camera.update();

        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render scene
        shader.use();
        shader.setUniform("dLight.direction", camera.getView() * sun.direction);

        model.draw(shader, camera.projection, camera.getView());

        n3d::Display::flip(postShader);
        n3d::Input::update();
    }

    n3d::Display::destroy();
}