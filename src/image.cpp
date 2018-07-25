#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <kiryu/image.h>

uint8_t* Image::loadImage(const std::string &filename, int &width, int &height,
        int &channels, const int desiredChannels)
{
    return stbi_load(filename.c_str(), &width, &height, &channels,
            desiredChannels);
}

bool Image::writePng(const std::string &filename, const int width,
        const int height, const int channels, const float *const data)
{
    uint8_t *const uint8Data = new uint8_t[width * height * channels];

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            for (int k = 0; k < channels; k++) {
                const int index = j * width * channels + i * channels + k;
                uint8Data[index] = 255 * data[index];
            }
        }
    }

    const bool noError = stbi_write_png(filename.c_str(), width, height, channels,
            uint8Data, width * channels);

    delete[] uint8Data;

    return noError;
}
