#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <Eigen/Dense>
#include <tiny_obj_loader.h>

#include <kiryu/vector.h>
#include <kiryu/sensor.h>
#include <kiryu/mesh.h>
#include <kiryu/screen.h>

using Eigen::MatrixXd;

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 480

bool traceRay(Sensor &sensor,tinyobj::attrib_t &attrib, Mesh &mesh,
        float *outputFrame, int p) {
    if (p >= WINDOW_WIDTH * WINDOW_HEIGHT) {
        return true;
    }

    int j = p % WINDOW_WIDTH;
    int i = p /  WINDOW_WIDTH;

    Ray3f ray = sensor.generateRay(j, i, 0.5, 0.5);

    bool intersection = false;
    Vector3f intersectionPoint;
    Vector3f closestIntersection;
    Float u, v;
    Float closestU, closestV;
    size_t closestFaceIndex;
    Float minIntersectionDistance = std::numeric_limits<float>::infinity();

    for (size_t f = 0; f < mesh.getFaceCount(); f++) {
        Vector3f vertices[3];

        bool tmpIntersection = mesh.findIntersection(ray, f,
                intersectionPoint, u, v);

        if (tmpIntersection) {
            Float distance = (intersectionPoint - ray.origin).norm();
            if (distance < minIntersectionDistance) {
                minIntersectionDistance = distance;
                closestIntersection = intersectionPoint;
                closestU = u;
                closestV = v;
                closestFaceIndex = f;
            }
        }

        intersection |= tmpIntersection;
    }

    if (intersection) {
        Vector3f normal;
        mesh.getNormal(normal, closestFaceIndex, closestU, closestV);

        Vector3f color = normal.cwiseAbs();
        for (int k = 0; k < 3; k++) {
            int index = i * (WINDOW_WIDTH * 3) + j * 3 + k;
            outputFrame[index] = color[k];
        }

    } else {
        float value = 0.0f;
        for (int k = 0; k < 3; k++) {
            int index = i * (WINDOW_WIDTH * 3) + j * 3 + k;
            outputFrame[index] = value;
        }
    }

    return false;
}

std::atomic_int pixelIndex(0);

void tracePool(int i, Sensor &sensor,tinyobj::attrib_t &attrib,
        Mesh &mesh, float *outputFrame) {
    bool finished;
    int rayCount = 1000;
    while (true) {
        int startingPixelIndex = pixelIndex.exchange(pixelIndex + rayCount);
        for (int i = 0; i < rayCount; i++) {
            finished = traceRay(sensor, attrib, mesh,
                    outputFrame, startingPixelIndex + i); 

            if (finished) {
                return;
            }
        }
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

    Vector3f position = {0, 0, 0};
    Vector3f target = {1, 0, 0};
    Vector3f up = {0, 0, 1};
    Float fov = 49.1343;
    fov = 2 * KIRYU_PI * fov / 360;
    Sensor sensor(position, target, up, fov, WINDOW_WIDTH, WINDOW_HEIGHT);

    int threadCount = std::thread::hardware_concurrency();
    //threadCount = 1;

    std::vector<std::thread> workers;
    workers.reserve(threadCount);
    std::cout << "Using " << threadCount << " thread(s)!" << std::endl;

    float outputFrame[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
    auto startTime = std::chrono::system_clock::now();

    for (int p = 0 ; p < threadCount; p++) { 
        workers.push_back(
                std::thread(tracePool, p, std::ref(sensor), std::ref(attrib),
                    std::ref(mesh), outputFrame));
    }
    for (int p = 0 ; p < threadCount; p++)  {
        workers[p].join();
    }

    auto endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    std::cout << "Rendering time: " << elapsed_seconds.count() << std::endl;

    Screen *screen = new Screen(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!screen->wasCreated()) {
        std::cerr << "Could not create screen!" << std::endl;
        return EXIT_FAILURE;
    }

    screen->bindTexture(outputFrame);
    int counter = 0;

    while (screen->isActive()) {
        screen->render();
    }

    delete screen;

    return EXIT_SUCCESS;
}
