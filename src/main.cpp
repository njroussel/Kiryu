#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <Eigen/Dense>
#include <tiny_obj_loader.h>

#include <kiryu/vector.h>
#include <kiryu/sensor.h>
#include <kiryu/mesh.h>
#include <kiryu/scene.h>
#include <kiryu/bruteAccel.h>
#include <kiryu/normalIntegrator.h>
#include <kiryu/screen.h>

using Eigen::MatrixXd;

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 480

std::atomic_int pixelIndex(0);

void tracePool(int i, Screen &screen, Sensor &sensor, Integrator &integrator,
        float *outputFrame) {
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
        screen.texChanged();
    }
}

int main() {
    std::string inputfile = "../res/models/bunny.obj";
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
    tinyobj::shape_t shape = shapes[0];
    tinyobj::mesh_t tinyObjMesh = shape.mesh;

    Mesh mesh(tinyObjMesh.indices, attrib.vertices, attrib.normals, attrib.texcoords,
            tinyObjMesh);

    Scene scene;
    scene.addMesh(mesh);

    BruteAccel accel(scene);
    NormalIntegrator integrator(accel);

    Vector3f position = {0, 0, 0};
    Vector3f target = {1, 0, 0};
    Vector3f up = {0, 0, 1};
    Float fov = 49.1343;
    fov = 2 * KIRYU_PI * fov / 360;
    Sensor sensor(position, target, up, fov, WINDOW_WIDTH, WINDOW_HEIGHT);

    float outputFrame[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
    Screen screen(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!screen.wasCreated()) {
        std::cerr << "Could not create screen!" << std::endl;
        return EXIT_FAILURE;
    }

    screen.bindTexture(outputFrame);

    glfwMakeContextCurrent(NULL);
    std::thread renderThread(&Screen::renderTextureWhileActive, &screen);

    int threadCount = std::thread::hardware_concurrency();

    std::vector<std::thread> workers;
    workers.reserve(threadCount);
    std::cout << "Using " << threadCount << " thread(s)!" << std::endl;

    auto startTime = std::chrono::system_clock::now();

    for (int p = 0 ; p < threadCount; p++) { 
        workers.push_back(
                std::thread(tracePool, p, std::ref(screen), std::ref(sensor),
                    std::ref(integrator), outputFrame));
    }
    for (int p = 0 ; p < threadCount; p++)  {
        workers[p].join();
    }

    auto endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    std::cout << "Rendering time: " << elapsed_seconds.count() << std::endl;

    renderThread.join();

    return EXIT_SUCCESS;
}
