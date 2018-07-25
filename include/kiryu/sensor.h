#pragma once

#include <kiryu/common.h>
#include <kiryu/ray.h>

class Sensor {
    public:
        Sensor(const Vector3f position, const Vector3f target,
                const Vector3f up, const Float fov, const uint16_t width,
                const uint16_t height);

        Ray3f generateRay(const uint16_t xPixel, const uint16_t yPixel,
                const Float xSample, const Float ySample);

    private:
        const Vector3f m_position;
        const Vector3f m_target;
        const Vector3f m_up;
        const Float m_fov;
        const uint16_t m_width;
        const uint16_t m_height;
};
