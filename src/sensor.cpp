#include <Eigen/Geometry>
#include <iostream>

#include <kiryu/sensor.h>

Sensor::Sensor(Vector3f position, Vector3f target, Vector3f up, Float fov,
        uint16_t width, uint16_t height) :
    m_position(position), m_target(target.normalized()), m_up(up.normalized()),
    m_fov(fov), m_width(width), m_height(height) { }

Ray3f Sensor::generateRay(uint16_t xPixel, uint16_t yPixel, Float xSample,
        Float ySample)
{
    Float sensorEpsilon = 1e-4;

    Float worldPixelSize = sensorEpsilon * std::atan(m_fov / 2.0f)  * 2.0f / m_width;

    Vector3f nearPlaneHorizon = m_target.cross(m_up);

    Vector3f worldTopLeftNearPlane =
        m_position +
        m_target * sensorEpsilon +
        nearPlaneHorizon * worldPixelSize * -m_width / 2.0f +
        m_up * worldPixelSize * m_height / 2.0f;

    Vector3f rayOrigin =
        worldTopLeftNearPlane +
        (xPixel + xSample) * worldPixelSize * nearPlaneHorizon +
        (yPixel + ySample) * worldPixelSize * -m_up; 

    Vector3f rayDirection = (rayOrigin - m_position).normalized();

    return Ray3f(rayOrigin, rayDirection);
}
