#include <iostream>
#include <Eigen/Dense>
#include <tiny_obj_loader.h>

#include <kiryu/vector.h>
#include <kiryu/sensor.h>
#include <kiryu/mesh.h>
#include <kiryu/screen.h>

using Eigen::MatrixXd;

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 480

int main() {
    std::string inputfile = "../res/models/triangle.obj";
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
    tinyobj::mesh_t mesh = shape.mesh;

    Vector3f position = {0, 0, 0};
    Vector3f target = {1, 0, 0};
    Vector3f up = {0, 0, 1};
    Float fov = 90;
    Sensor sensor(position, target, up, fov, WINDOW_WIDTH, WINDOW_HEIGHT);

    float outputFrame[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
    for (int i = 0; i < WINDOW_WIDTH; i++) {
        for (int j = 0; j < WINDOW_HEIGHT; j++) {
            Ray3f ray = sensor.generateRay(i, j, 0.5, 0.5);

            bool intersection = false;
            size_t index_offset = 0;

            for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
                int fv = mesh.num_face_vertices[f];
                if (fv != 3) {
                    std::cerr << "Mesh has non-triangle faces!" << std::endl;
                    return EXIT_FAILURE;
                }

                Vector3f vertices[3];

                for (size_t v = 0; v < 3; v++) {
                    tinyobj::index_t idx = mesh.indices[index_offset + v];

                    if (idx.vertex_index >=0) {
                        tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
                        tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
                        tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
                        vertices[v] = {vx, vy, vz};
                    }
                }
                index_offset += fv;

                Vector3f e0 = vertices[1] - vertices[0];
                Vector3f e1 = vertices[2] - vertices[0];
                Float dotRayTriangle = e0.dot(e1.cross(ray.direction));

                if (dotRayTriangle > -1e-8f && dotRayTriangle < 1e-8f) {
                    std::cout << "Ray parallel to face!" << std::endl; 
                    continue;
                }

                Vector4f v0;
                v0 << vertices[0], 1.0f;

                Vector4f v1;
                v1 << vertices[1], 1.0f;

                Vector4f v2;
                v2 << vertices[2], 1.0f;

                Vector4f d;
                d << ray.direction, 0.0f;

                Vector4f o;
                o << ray.origin, 1.0f;

                Matrix4f mat;
                mat << v0, v1, v2, -d;

                Matrix4f matInv = mat.inverse();
                Vector4f solution = matInv * o;

                if (solution[0] <= 1 && solution[1] <= 1 && solution[2] <=1) {
                    intersection = true;
                }
            }

            for (int k = 0; k < 3; k++) {
                float value = 0.0f;
                if (intersection) {
                    value = 1.0f;
                }
                int index = j * (WINDOW_WIDTH * 3) + i * 3 + k;
                outputFrame[index] = value;
            }
        }
    }

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
