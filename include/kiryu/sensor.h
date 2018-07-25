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
        const Vector3f m_position;
        const Vector3f m_target;
        const Vector3f m_up;
        const Float m_fov;
        const uint16_t m_width;
        const uint16_t m_height;
};
