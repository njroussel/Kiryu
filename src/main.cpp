#include <iostream>
#include <fstream>

#include <tiny_obj_loader.h>
#include <nlohmann/json.hpp>

#include <kiryu/vector.h>
#include <kiryu/sensor.h>
#include <kiryu/mesh.h>
#include <kiryu/scene.h>
#include <kiryu/bruteAccel.h>
#include <kiryu/kdtree.h>
#include <kiryu/normalIntegrator.h>
#include <kiryu/render.h>

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 200
#define KIRYU_GUI_ENABLE true

using nlohmann::json;

int main() {
    std::ifstream i("../res/base.json");
    json j;
    i >> j;
    std::cout << j << std::endl;
    std::string accelType = j["accel"];
    std::cout << accelType << std::endl;

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
    tinyobj::mesh_t *tinyObjMesh0 = &(shapes0[0].mesh);

    Mesh mesh0(tinyObjMesh0->indices, attrib0.vertices, attrib0.normals,
            attrib0.texcoords, *tinyObjMesh0);
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
    tinyobj::mesh_t *tinyObjMesh1 = &(shapes1[0].mesh);

    Mesh mesh1(tinyObjMesh1->indices, attrib1.vertices, attrib1.normals,
            attrib1.texcoords, *tinyObjMesh1);
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

    Render render;

    render.render(KIRYU_GUI_ENABLE, "KiryuNormals", sensor, integrator);

    i.close();

    return EXIT_SUCCESS;
}
