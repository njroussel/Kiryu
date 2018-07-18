#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <Eigen/Dense>
#include <tiny_obj_loader.h>

#include <kiryu/aabb.h>
#include <kiryu/vector.h>
#include <kiryu/sensor.h>
#include <kiryu/mesh.h>
#include <kiryu/scene.h>
#include <kiryu/bruteAccel.h>
#include <kiryu/normalIntegrator.h>
#include <kiryu/screen.h>

#define WINDOW_WIDTH 360
#define WINDOW_HEIGHT 240
#define KIRYU_GUI_ENABLE true

std::atomic_int pixelIndex(0);

void tracePool(int i, Screen *screen, Sensor &sensor, Integrator &integrator,
        float *outputFrame)
{
    bool finished;
    int rayCount = 1000;
    while (true) {
        int startingPixelIndex = pixelIndex.exchange(pixelIndex + rayCount);
        for (int i = 0; i < rayCount; i++) {
            if ((startingPixelIndex + i) >= WINDOW_WIDTH * WINDOW_HEIGHT) {
                return;
            }

            int px = (startingPixelIndex + i) % WINDOW_WIDTH;
            int py = (startingPixelIndex + i) /  WINDOW_WIDTH;

            Ray3f ray = sensor.generateRay(px, py, 0.5, 0.5);

            Color3f color = integrator.Li(ray);

            for (int k = 0; k < 3; k++) {
                int index = py * (WINDOW_WIDTH * 3) + px * 3 + k;
                outputFrame[index] = color[k];
            }
        }

        if (KIRYU_GUI_ENABLE) {
            screen->texChanged();
        }
    }
}

int main() {
    std::string inputfile = "../res/models/kiryu.obj";
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

    if (!err.empty()) { // `err` may contain warning message.
        std::cerr << err << std::endl;
    }
    if (!ret) {
        std::cerr << "Could not load obj!" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Shapes: " << shapes.size() << std::endl;
    tinyobj::shape_t shape0 = shapes[0];
    tinyobj::shape_t shape1 = shapes[1];
    tinyobj::mesh_t tinyObjMesh0 = shape0.mesh;
    tinyobj::mesh_t tinyObjMesh1 = shape1.mesh;

    Mesh mesh0(tinyObjMesh0.indices, attrib.vertices, attrib.normals, attrib.texcoords,
            tinyObjMesh0);
    Mesh mesh1(tinyObjMesh1.indices, attrib.vertices, attrib.normals, attrib.texcoords,
            tinyObjMesh1);

    Scene scene;
    scene.addMesh(mesh0);
    scene.addMesh(mesh1);

    BruteAccel accel(scene);
    NormalIntegrator integrator(accel);

    Vector3f position = {0, 0, 0};
    position = {1.6577621698379517, -3.6390209197998047, 0.9906208515167236};

    Vector3f target = {1, 0, 0};
    target = {-0.413979709148407, 0.9098556637763977, -0.027995765209197998};

    Vector3f up = {0, 0, 1};
    up = {-0.011594235897064209, 0.02548210881650448, 0.9996081590652466};

    Float fov = 49.1343;
    fov = 2 * KIRYU_PI * fov / 360;
    Sensor sensor(position, target, up, fov, WINDOW_WIDTH, WINDOW_HEIGHT);

    float outputFrame[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
    for (int i = 0; i < WINDOW_WIDTH; i++) {
        for (int j = 0; j < WINDOW_HEIGHT; j++) {
            for (int k = 0; k < 3; k++) {
                int index = j * (WINDOW_WIDTH * 3) + i * 3 + k;
                outputFrame[index] = 1;
            }
        }
    }

    Screen *screenPtr = nullptr;
    std::thread *renderThreadPtr = nullptr;

    if (KIRYU_GUI_ENABLE) {
        Screen *screen = new Screen(WINDOW_WIDTH, WINDOW_HEIGHT);
        screenPtr = screen;

        if (!screen->wasCreated()) {
            std::cerr << "Could not create screen!" << std::endl;
            return EXIT_FAILURE;
        }

        screen->bindTexture(outputFrame);

        glfwMakeContextCurrent(NULL);
        std::cout << "Before" << std::endl;
        std::thread *renderThread = new std::thread(&Screen::renderTextureWhileActive, screen);
        std::cout << "After" << std::endl;
        renderThreadPtr = renderThread;
    }

    int threadCount = std::thread::hardware_concurrency();

    std::vector<std::thread> workers;
    workers.reserve(threadCount);
    std::cout << "Using " << threadCount << " thread(s)!" << std::endl;

    auto startTime = std::chrono::system_clock::now();

    for (int p = 0 ; p < threadCount; p++) { 
        workers.push_back(
                std::thread(tracePool, p, screenPtr, std::ref(sensor),
                    std::ref(integrator), outputFrame));
    }
    for (int p = 0 ; p < threadCount; p++)  {
        workers[p].join();
    }

    auto endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    std::cout << "Rendering time: " << elapsed_seconds.count() << std::endl;

    if (KIRYU_GUI_ENABLE) {
        renderThreadPtr->join();


        Screen *screen = static_cast<Screen *>(screenPtr);
        delete screen;
    }



    return EXIT_SUCCESS;
}
