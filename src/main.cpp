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

bool findIntersection(Vector3f *vertices, Ray3f *ray,
        Vector3f &outIntersectionPoint, Float &u, Float &v)
{
    Vector3f edge1 = vertices[1] - vertices[0];
    Vector3f edge2 = vertices[2] - vertices[0];
    Vector3f h = ray->direction.cross(edge2);
    Float a = edge1.dot(h);
    if (a > -KIRYU_EPSILON && a < KIRYU_EPSILON){
        return false;
    }
    Float l = 1 / a;
    Vector3f s = ray->origin - vertices[0];
    u = l * (s.dot(h));
    if (u < 0.0 || u > 1.0) {
        return false;
    }
    Vector3f q = s.cross(edge1);
    v = l * ray->direction.dot(q);
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }

    Float t = l * edge2.dot(q);
    if (t > KIRYU_EPSILON) {
        outIntersectionPoint = ray->origin + ray->direction * t;
        return true;
    }

    return false;
}

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

    Mesh myMesh(mesh.indices.size(), mesh.indices.data(),
            attrib.vertices.data(),attrib.normals.data(),
            attrib.texcoords.data());

    Vector3f position = {0, 0, 0};
    Vector3f target = {1, 0, 0};
    Vector3f up = {0, 0, 1};
    Float fov = 90;
    Sensor sensor(position, target, up, fov, WINDOW_WIDTH, WINDOW_HEIGHT);

    float outputFrame[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
    for (int i = 0; i < WINDOW_WIDTH; i++) {
        for (int j = 0; j < WINDOW_HEIGHT; j++) {
            std::cout << i * WINDOW_HEIGHT + j << " / " <<
                WINDOW_WIDTH * WINDOW_HEIGHT << std::endl;

            Ray3f ray = sensor.generateRay(i, j, 0.5, 0.5);

            bool intersection = false;
            Vector3f intersectionPoint;
            Vector3f closestIntersection;
            Float u, v;
            Float closestU, closestV;
            size_t closestFaceIndex;
            Float minIntersectionDistance = std::numeric_limits<float>::infinity();

            size_t index_offset = 0;

            for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
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

                index_offset += 3;

                intersection |= findIntersection(vertices, &ray,
                        intersectionPoint, u, v);

                if (intersection) {
                    Float distance = (intersectionPoint - ray.origin).norm();
                    if (distance < minIntersectionDistance) {
                        closestIntersection = intersectionPoint;
                        closestU = u;
                        closestV = v;
                        closestFaceIndex = index_offset - 3;
                    }
                }
            }

            if (intersection) {
                Vector3f normals[3];

                for (size_t v = 0; v < 3; v++) {
                    tinyobj::index_t idx = mesh.indices[closestFaceIndex + v];

                    if (idx.vertex_index >=0) {
                        tinyobj::real_t nx = attrib.normals[3*idx.vertex_index+0];
                        tinyobj::real_t ny = attrib.normals[3*idx.vertex_index+1];
                        tinyobj::real_t nz = attrib.normals[3*idx.vertex_index+2];
                        normals[v] = {nx, ny, nz};
                        std::cout << normals[1] << std::endl;
                        std::cout << "------------" << std::endl;

                    }
                }

                Vector3f color = closestU * normals[1] + closestV * normals[2] +
                    (1 - closestU - closestV) * normals[0];

                for (int k = 0; k < 3; k++) {
                    int index = j * (WINDOW_WIDTH * 3) + i * 3 + k;
                    outputFrame[index] = color[k];
                }

            } else {
                float value = 0.0f;
                for (int k = 0; k < 3; k++) {
                    int index = j * (WINDOW_WIDTH * 3) + i * 3 + k;
                    outputFrame[index] = value;
                }
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
