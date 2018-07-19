#pragma once

#include <kiryu/common.h>
#include <kiryu/ray.h>

class Sensor {
    public:
        Sensor(Vector3f position, Vector3f target, Vector3f up, Float fov,
                uint16_t width, uint16_t height);

        Ray3f generateRay(uint16_t xPixel, uint16_t yPixel, Float xSample,
                Float ySample);

    private:
        Vector3f m_position;
        Vector3f m_target;
        Vector3f m_up;
        Float m_fov;
        uint16_t m_width;
        uint16_t m_height;
};
