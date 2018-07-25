#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

#include <Eigen/Dense>
#include <tiny_obj_loader.h>
#include <nlohmann/json.hpp>

#include <kiryu/image.h>
#include <kiryu/vector.h>
#include <kiryu/ray.h>
#include <kiryu/aabb.h>
#include <kiryu/rng.h>
#include <kiryu/sensor.h>
#include <kiryu/mesh.h>
#include <kiryu/scene.h>
#include <kiryu/bruteAccel.h>
#include <kiryu/kdtree.h>
#include <kiryu/normalIntegrator.h>
#include <kiryu/screen.h>

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 720
#define SAMPLE_COUNT 1
#define KIRYU_GUI_ENABLE true

using nlohmann::json;

static std::atomic_int pixelIndex(0);
static RNG rng[4];

static void tracePool(int thredIndex, Screen *screen, Sensor &sensor, Integrator &integrator,
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
            int py = (startingPixelIndex + i) / WINDOW_WIDTH;

            Color3f color(0.0);

            for (size_t j = 0; j < SAMPLE_COUNT; j++) {
                Ray3f ray = sensor.generateRay(px, py,
                        rng[thredIndex].nextFloat(),
                        rng[thredIndex].nextFloat());
                color += integrator.Li(ray);
            }

            color /= SAMPLE_COUNT;

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
    Scene scene;

    // MESH0
    std::string inputfile0("../res/models/teapot.obj");
    tinyobj::attrib_t attrib0;
    std::vector<tinyobj::shape_t> shapes0;
    std::vector<tinyobj::material_t> materials0;

    std::string err0;
    bool ret0 = tinyobj::LoadObj(&attrib0, &shapes0, &materials0, &err0, inputfile0.c_str());

    if (!err0.empty()) { // `err` may contain warning message.
        std::cerr << err0 << std::endl;
    }
    if (!ret0) {
        std::cerr << "Could not load obj!" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Shapes: " << shapes0.size() << std::endl;
    tinyobj::shape_t shape0 = shapes0[0];
    tinyobj::mesh_t tinyObjMesh0 = shape0.mesh;

    Mesh mesh0(tinyObjMesh0.indices, attrib0.vertices, attrib0.normals,
            attrib0.texcoords, tinyObjMesh0);
    std::cout << "Face count: " << mesh0.getFaceCount() << std::endl;

    scene.addMesh(mesh0);

    // MESH1
    std::string inputfile1("../res/models/kiryu.obj");
    tinyobj::attrib_t attrib1;
    std::vector<tinyobj::shape_t> shapes1;
    std::vector<tinyobj::material_t> materials1;

    std::string err1;
    bool ret1 = tinyobj::LoadObj(&attrib1, &shapes1, &materials1, &err1, inputfile1.c_str());

    if (!err1.empty()) { // `err` may contain warning message.
        std::cerr << err1 << std::endl;
    }
    if (!ret1) {
        std::cerr << "Could not load obj!" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Shapes: " << shapes1.size() << std::endl;
    tinyobj::shape_t shape1 = shapes1[0];
    tinyobj::mesh_t tinyObjMesh1 = shape1.mesh;

    Mesh mesh1(tinyObjMesh1.indices, attrib1.vertices, attrib1.normals, attrib1.texcoords,
            tinyObjMesh1);
    std::cout << "Face count: " << mesh1.getFaceCount() << std::endl;

    scene.addMesh(mesh1);

    // Accel, integrator, camera
    KDTree accel(scene);
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

    float* outputFrame = new float[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
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
        std::thread *renderThread = new std::thread(&Screen::renderTextureWhileActive, screen);
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
    std::cout << "Rendering time: " << elapsed_seconds.count() << " seconds" <<
        std::endl;

    if (KIRYU_GUI_ENABLE) {
        renderThreadPtr->join();

        delete renderThreadPtr;

        Screen *screen = static_cast<Screen *>(screenPtr);
        delete screen;
    }

    bool noError = Image::writePng("out.png", WINDOW_WIDTH, WINDOW_HEIGHT,
            3, outputFrame);
    if (!noError) {
        std::cerr << "Could not wirte output image properly!" << std::endl;
    }

    delete[] outputFrame;

    return EXIT_SUCCESS;
}
