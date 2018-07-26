#include <Eigen/Geometry>
#include <iostream>

#include <kiryu/sensor.h>

Sensor::Sensor(const Vector3f position, const Vector3f target, const Vector3f up,
        const Float fov, const uint16_t width, const uint16_t height) :
    m_position(position), m_target(target.normalized()), m_up(up.normalized()),
    m_fov(fov), m_width(width), m_height(height) { }

Ray3f Sensor::generateRay(const uint16_t xPixel, const uint16_t yPixel,
        const Float xSample, const Float ySample) const
{
    const Float sensorEpsilon = 1e-4;

    const Float worldPixelSize =
        sensorEpsilon * std::atan(m_fov / 2.0f)  * 2.0f / m_width;

    const Vector3f nearPlaneHorizon = m_target.cross(m_up);

    const Vector3f worldTopLeftNearPlane =
        m_position +
        m_target * sensorEpsilon +
        nearPlaneHorizon * worldPixelSize * -m_width / 2.0f +
        m_up * worldPixelSize * m_height / 2.0f;

    const Vector3f rayOrigin =
        worldTopLeftNearPlane +
        (xPixel + xSample) * worldPixelSize * nearPlaneHorizon +
        (yPixel + ySample) * worldPixelSize * -m_up; 

    const Vector3f rayDirection = (rayOrigin - m_position).normalized();

    return Ray3f(rayOrigin, rayDirection);
}
