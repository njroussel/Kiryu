#include <kiryu/parser.h>

#include <fstream>
#include <iostream>

#include <kiryu/bruteAccel.h>
#include <kiryu/kdtree.h>

#include <kiryu/normalIntegrator.h>

using nlohmann::json;

typedef json Json;

Parser::Parser(const std::string fileName) : m_fileName(fileName) { }

Parser::~Parser() {
    delete m_accel;
    delete m_integrator;
}

bool Parser::parse() {
    Json json;

    std::ifstream inputStream(m_fileName);
    inputStream >> json;
    inputStream.close();

    const std::vector<Json> jsonMeshes = json["meshes"];
    bool errParseMeshes = parseMeshes(jsonMeshes);
    if (errParseMeshes) {
        return true;
    }

    const std::string accelType = json["accel"];
    bool errParseAccel = parseAccel(accelType);
    if (errParseAccel) {
        return true;
    }

    const std::string integratorType = json["integrator"];
    bool errParseIntegrator = parseIntegrator(integratorType);
    if (errParseIntegrator) {
        return true;
    }

    const Json jsonSensor = json["sensor"];
    bool errParseSensor = parseSensor(jsonSensor);
    if (errParseSensor) {
        return true;
    }

    return false;
}

bool Parser::parseMeshes(const std::vector<Json> &jsonMeshes) {

    size_t index = m_fileName.find_last_of("/");
    std::string path = m_fileName.substr(0, index);

    for (Json jsonMesh : jsonMeshes) {
        // TODO: Dynamically allocate Mesh here. They can be potentially very
        // large and the implementaion copies them in the Mesh aswell as the
        // Scene class (change both classes accordingly). The bulk of their data
        // is already on the heap due to the internal std::vector Mesh and Scene
        // use. Trying to keep the wrappers on the stack doesn't help. 

        const std::string meshFile = jsonMesh["file"];
        const std::string meshPath = path + meshFile.substr(1, meshFile.size());

        std::cout << meshPath << std::endl;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err,
                meshPath.c_str());

        if (!err.empty()) { // `err` may contain warning message.
            std::cerr << err << std::endl;
            return true;
        }
        if (!ret) {
            std::cerr << "Could not load obj!" << std::endl;
            return true;
        }

        std::cout << "Shapes: " << shapes.size() << std::endl;
        tinyobj::mesh_t *tinyObjMesh = &(shapes[0].mesh);

        m_scene.addMesh(Mesh(tinyObjMesh->indices, attrib.vertices, attrib.normals,
                    attrib.texcoords, *tinyObjMesh));
    }

    return false;
}

bool Parser::parseAccel(const std::string &accelType) {
    if (accelType == "brute") {
        m_accel = new BruteAccel(m_scene);
    } else if (accelType == "kdtree") {
        m_accel = new KDTree(m_scene);
    } else {
        return true;
    }

    return false;
}

bool Parser::parseIntegrator(const std::string &integratorType) {
    if (integratorType == "normal") {
        m_integrator = new NormalIntegrator(*m_accel);
    } else {
        return true;
    }

    return false;
}

bool Parser::parseSensor(const Json &jsonSensor) {
    std::vector<Float> position = jsonSensor["position"];
    std::vector<Float> target = jsonSensor["target"];
    std::vector<Float> up = jsonSensor["up"];
    Float fov = jsonSensor["fov"];
    uint64_t width = jsonSensor["width"];
    uint16_t height = jsonSensor["height"];

    m_sensor = new Sensor(
            Vector3f(position.data()), 
            Vector3f(target.data()), 
            Vector3f(up.data()), 
            2 * KIRYU_PI * fov / 360,
            width, 
            height);

    return false;
}
