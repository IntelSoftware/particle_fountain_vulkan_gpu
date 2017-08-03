#include <cstdio>
#include <iostream>

#include <renderer/appinstance.h>
#include <renderer/surfacewindows.h>
#include <renderer/device.h>
#include <renderer/particles_element.h>
#include <renderer/scene.h>

#include <psim/buffer.h>
#include <psim/model.h>
#include <psim/point_generator.h>
#include <psim/const_force_interactor.h>
#include <psim/point_gravity_interactor.h>
#include <psim/planar_gravity_interactor.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr int SCR_WIDTH = 1280;
constexpr int SCR_HEIGHT = 960;
constexpr std::size_t MAX_PARTICLES = 200000;
constexpr float WORLD_SIZE = 500;
constexpr int THREADS = 8;

psim::Model::GeneratorList initGeneratorsList() 
{
    psim::Model::GeneratorList genList;
    psim::generators::PointGenerator* g = new psim::generators::PointGenerator(
        { 0.0f, 250.0f, 0.0f },
        psim::generators::PointGenerator::defTheta(90, 5),
        psim::generators::PointGenerator::defPhi(-90, 5),
        psim::generators::PointGenerator::defTTL(600, 0),
        psim::generators::PointGenerator::defMass(5.0e+10f, 0),
        psim::generators::PointGenerator::defSpeed(95, 10),
        psim::generators::PointGenerator::defSpawningRate(20.0f, 0.1f)
    );
    genList.push_back(g);
    g = nullptr;

    return genList;
}

psim::Model::InteractorList initInteractorList()
{
    psim::interactors::BaseInteractor* i = nullptr;
    psim::Model::InteractorList iList;
    i = new psim::interactors::PlanarGravityInteractor(
        { 0.0f, 300.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },
        2.5e+15f
    );
    iList.push_back(i);
    i = nullptr;
    /*
    i = new psim::interactors::ConstForceInteractor(
        { 0.0f, 1.0f, 0.0f },
        10.0e2f
    );
    iList.push_back(i);
    i = nullptr;
    */
    i = new psim::interactors::PointGravityInteractor(
        { -60.0f, 100.0f, 0.0f },
        8.0e+15f
    );
    iList.push_back(i);
    i = nullptr;
    i = new psim::interactors::PointGravityInteractor(
        { 60.0f, 150.0f, 0.0f },
        4.0e+15f
    );
    iList.push_back(i);
    i = nullptr;
    /*
    i = new psim::interactors::PointGravityInteractor(
        { 60.0f, 50.0f, 0.0f },
        2.0e+15f,
        -psim::interactors::AbstractGravityInteractor::G
    );
    iList.push_back(i);
    i = nullptr;
    */

    return iList;
}

glm::mat4 initMVP() 
{
    glm::mat4 model = glm::mat4();
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    return proj * view * model;
}


int main()
{

// INIT MODEL--------------------------------------
    psim::Buffer buffer(MAX_PARTICLES);

    auto genList = initGeneratorsList();
    auto iList = initInteractorList();
    auto mvp = initMVP();

    psim::Model::size worldSize;
    worldSize[psim::axis::X] = WORLD_SIZE;
    worldSize[psim::axis::Y] = WORLD_SIZE;
    worldSize[psim::axis::Z] = WORLD_SIZE;
    psim::Model world(worldSize, genList, iList, THREADS);

    for (psim::generators::BaseGenerator* g : genList) {
        delete g;
        g = nullptr;
    }
    genList.clear();

    for(psim::interactors::BaseInteractor* i : iList) {
        delete i;
        i=nullptr;
    }
    iList.clear();

// END INIT MODEL--------------------------------------

    GLFWwindow *wnd;
    glfwInit();

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    wnd = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Particle Vulkan CPU", nullptr, nullptr);

    try {
        renderer::AppInstance appInstance;
        renderer::SurfaceWindows appSurface(appInstance, GetModuleHandle(nullptr), glfwGetWin32Window(wnd));
        renderer::Device appDevice(appInstance, appSurface.surface());
        renderer::Scene scene(appDevice);
        renderer::ParticlesElement partElem(appDevice, scene.renderPass(), buffer.dataSizeTotal());
        scene.addToScene(&partElem);

        partElem.setMVP(mvp); 

        std::chrono::system_clock::time_point prev = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point now;
        std::chrono::system_clock::time_point fpsBegin = prev;
        uint32_t framecount = 0;
        std::chrono::microseconds dt;

        while (!glfwWindowShouldClose(wnd)) {
            now = std::chrono::system_clock::now();
            dt = std::chrono::duration_cast<std::chrono::microseconds>(now - prev);
            prev = now;

            world.progress(dt, buffer);
            partElem.setVertexBufferData(buffer.activeCount(), buffer.data(), buffer.dataSizeTotal());
            scene.render();

            ++framecount;
            uint32_t fpsdt = std::chrono::duration_cast<std::chrono::seconds>(now - fpsBegin).count();
            if (fpsdt >= 1) {
                std::clog << "FPS: " << framecount << "\n";
                framecount = 0;
                fpsBegin = now;
            }

            glfwPollEvents();
        }
    } 
    catch (const std::runtime_error& e) {
        std::clog << "Exception raised: " << e.what() << std::endl;
    }
    catch (...) {
        std::clog << "Unexpected exception raised\n";
    }

    glfwDestroyWindow(wnd);
    glfwTerminate();

    system("pause");

    return 0;
}
