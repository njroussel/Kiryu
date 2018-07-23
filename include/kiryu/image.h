#pragma once

#include <string>

#include <kiryu/common.h>

class Image {
    public:
        static uint8_t* loadImage(const std::string &filename, int &width,
                int &height, int &channels, const int desiredChannels);

        static bool writePng(const std::string &filename, const int width,
                const int height, const int channels, const float *const data);

    private:
        explicit Image() { }
};
