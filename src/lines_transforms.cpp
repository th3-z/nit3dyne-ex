#include <nit3dyne/core/display.h>
#include <nit3dyne/graphics/shader.h>
#include <nit3dyne/camera/cameraOrbit.h>
#include <nit3dyne/core/resourceCache.h>
#include <nit3dyne/graphics/mesh.h>
#include <nit3dyne/graphics/model.h>
#include <nit3dyne/graphics/lines.h>


int main() {
    n3d::Display::init();
    n3d::Input::init(n3d::Display::window);

    n3d::Shader shader("shaders/vertex.vert", "shaders/fragment.frag");
    shader.use();
    shader.setUniform("tex", 0);

    n3d::Shader postShader("shaders/post.vert", "shaders/post.frag");
    postShader.use();
    postShader.setUniform("tex", 0);
    postShader.setUniform("texDither", 1);

    n3d::Shader shaderLines("shaders/line.vert", "shaders/line.frag");

    n3d::DirectionalLight sun = n3d::DirectionalLight();
    sun.diffuse = glm::vec3(0.8, 0.8, 0.8);
    sun.ambient = glm::vec3(0.1, 0.1, 0.1);
    shader.use();
    shader.setDirectionalLight(sun);

    n3d::CameraOrbit camera(85.f, n3d::Display::viewPort);

    n3d::ResourceCache<n3d::Texture> textureCache;
    n3d::ResourceCache<n3d::MeshStatic> meshCache;

    std::vector<n3d::Model> models;

    for (int i = 0; i < 4; ++i)
        models.emplace_back(
            n3d::Model(meshCache.loadResource("sphere"), textureCache.loadResource("error"))
        );

    models[1].translate(5.f, 0.f, 0.f);
    models[1].scale(.5f, 1.f, 1.f);
    models[2].translate(0.f, 5.f, 0.f);
    models[2].scale(1.f, .5f, 1.f);
    models[3].translate(0.f, 0.f, 5.f);
    models[3].scale(1.f, 1.f, .5f);

    std::vector<n3d::Line> axisData;
    axisData.push_back(n3d::Line{
            {0.f, 0.f, 0.f}, {1.f, 0.f, 0.f},
            {50.f, 0.f, 0.f}, {0.f, 0.f, 0.f}
    });
    axisData.push_back(n3d::Line{
            {0.f, 0.f, 0.f}, {0.f, 1.f, 0.f},
            {0.f, 50.f, 0.f}, {0.f, 0.f, 0.f}
    });
    axisData.push_back(n3d::Line{
            {0.f, 0.f, 0.f}, {0.f, 0.f, 1.f},
            {0.f, 0.f, 50.f}, {0.f, 0.f, 0.f}
    });
    n3d::Lines axis(axisData);

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

        models[0].rotate(
            (360.f * 1.f) * ((float) n3d::Display::timeDelta / 10.f),
            0.f, 1.f, 0.f,
            false
        );

        for (auto model : models)
            model.draw(shader, camera.projection, camera.getView());

        axis.draw(shaderLines, camera.projection, camera.getView());

        n3d::Display::flip(postShader);
        n3d::Input::update();
    }

    n3d::Display::destroy();
}