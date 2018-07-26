#pragma once

#include <string>
#include <atomic>

#include <kiryu/sensor.h>
#include <kiryu/integrator.h>
#include <kiryu/screen.h>
#include <kiryu/rng.h>

class Render {
    public:
        explicit Render() : m_pixelIndex(0) { };

        void render(const bool gui, const std::string renderName,
                const Sensor &sensor, const Integrator &integrator);

    private:
        void tracePool(const int threadIndex, const bool gui, Screen *screen,
                const Sensor &sensor, const Integrator &integrator,
                float *const outputFrame);

        std::atomic_int m_pixelIndex;
        RNG m_rng[4];
};
