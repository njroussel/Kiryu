#include <kiryu/render.h>

#include <chrono>
#include <thread>

#include <kiryu/vector.h>
#include <kiryu/image.h>

#define SAMPLE_COUNT 1

void Render::render(const bool gui, const std::string renderName,
        const Sensor &sensor, const Integrator &integrator)
{
    const uint16_t sensorWidth = sensor.getWidth();
    const uint16_t sensorHeight = sensor.getHeight();

    float *const outputFrame = new float[sensorWidth * sensorHeight * 3];
    for (int i = 0; i < sensorWidth; i++) {
        for (int j = 0; j < sensorHeight; j++) {
            for (int k = 0; k < 3; k++) {
                const int index = j * (sensorWidth * 3) + i * 3 + k;
                outputFrame[index] = 1;
            }
        }
    }

    Screen *screen = nullptr;
    std::thread *renderThread = nullptr;

    if (gui) {
        screen = new Screen(sensorWidth, sensorHeight);

        if (!screen->wasCreated()) {
            std::cerr << "Could not create screen!" << std::endl;
            return;
        }

        screen->bindTexture(outputFrame);

        glfwMakeContextCurrent(NULL);
        renderThread = new std::thread(&Screen::renderTextureWhileActive, screen);
    }

    const int threadCount = std::thread::hardware_concurrency();

    std::vector<std::thread> workers;
    workers.reserve(threadCount);
    std::cout << "Using " << threadCount << " thread(s)!" << std::endl;

    const auto startTime = std::chrono::system_clock::now();

    for (int p = 0 ; p < threadCount; p++) {
        workers.push_back(
                std::thread(&Render::tracePool, this, p, gui, screen, std::ref(sensor),
                    std::ref(integrator), outputFrame));
    }
    for (int p = 0 ; p < threadCount; p++)  {
        workers[p].join();
    }

    const auto endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    std::cout << "Rendering time: " << elapsed_seconds.count() << " seconds" <<
        std::endl;

    if (gui) {
        renderThread->join();

        delete renderThread;
        delete screen;
    }

    const bool noError = Image::writePng(renderName + ".png", sensorWidth,
            sensorHeight, 3, outputFrame);
    if (!noError) {
        std::cerr << "Could not wirte output image properly!" << std::endl;
    }

    delete[] outputFrame;
}

void Render::tracePool(const int threadIndex, const bool gui, Screen *screen,
        const Sensor &sensor, const Integrator &integrator,
        float *const outputFrame)
{
    const uint16_t sensorWidth = sensor.getWidth();
    const uint16_t sensorHeight = sensor.getHeight();

    const int rayCount = 1000;
    while (true) {
        int startingPixelIndex = m_pixelIndex.exchange(m_pixelIndex + rayCount);
        for (int i = 0; i < rayCount; i++) {
            if ((startingPixelIndex + i) >= sensorWidth * sensorHeight) {
                if (gui) {
                    screen->texChanged();
                }
                return;
            }

            const int px = (startingPixelIndex + i) % sensorWidth;
            const int py = (startingPixelIndex + i) / sensorWidth;

            Color3f color(0.0);

            for (size_t j = 0; j < SAMPLE_COUNT; j++) {
                const Ray3f ray = sensor.generateRay(px, py,
                        m_rng[threadIndex].nextFloat(),
                        m_rng[threadIndex].nextFloat());
                color += integrator.Li(ray);
            }

            color /= SAMPLE_COUNT;

            const int index = py * (sensorWidth * 3) + px * 3;
            for (int k = 0; k < 3; k++) {
                outputFrame[index + k] = color[k];
            }
        }

        if (gui) {
            screen->texChanged();
        }
    }
}
