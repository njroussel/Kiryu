#pragma once

#include <string>
#include <nlohmann/json.hpp>

#include <kiryu/integrator.h>
#include <kiryu/sensor.h>

class Parser {
    public:
        explicit Parser(const std::string fileName);

        ~Parser();

        bool parse();

        inline const Sensor& getSensor() const { return *m_sensor; }

        inline const Integrator& getIntegrator() const { return *m_integrator; }

    private:
        bool parseMeshes(const std::vector<nlohmann::json> &jsonMeshes);

        bool parseAccel(const std::string &accelType);

        bool parseIntegrator(const std::string &integratorType);

        bool parseSensor(const nlohmann::json &jsonSensor);

        const std::string m_fileName;
        Scene m_scene;
        Accel *m_accel;
        const Integrator *m_integrator;
        const Sensor *m_sensor;
};
