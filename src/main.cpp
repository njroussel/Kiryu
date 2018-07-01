#include <iostream>
#include <Eigen/Dense>

#include <kiryu/screen.h>

using Eigen::MatrixXd;

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 480


static float CUSTOM_FRAME1[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
static float CUSTOM_FRAME2[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
static const int CUSTOM_FRAME1_ROW = 40;
static const int CUSTOM_FRAME1_COL = 40;
static const int CUSTOM_FRAME2_ROW = 100;
static const int CUSTOM_FRAME2_COL = 100;


int main() {
    for (int i = 0; i < WINDOW_WIDTH; i++) {
        for (int j = 0; j < WINDOW_HEIGHT; j++) {
            for (int k = 0; k < 3; k++) {
                float value = 0.0f;
                if (j == CUSTOM_FRAME1_ROW || i == CUSTOM_FRAME1_COL) {
                    value = 1.0f;
                }
                int index = j * (WINDOW_WIDTH * 3) + i * 3 + k;
                CUSTOM_FRAME1[index] = value;
            }
        }
    }

    for (int i = 0; i < WINDOW_WIDTH; i++) {
        for (int j = 0; j < WINDOW_HEIGHT; j++) {
            for (int k = 0; k < 3; k++) {
                float value = 0.0f;
                if (j == CUSTOM_FRAME2_ROW || i == CUSTOM_FRAME2_COL) {
                    value = 1.0f;
                }
                int index = j * (WINDOW_WIDTH * 3) + i * 3 + k;
                CUSTOM_FRAME2[index] = value;
            }
        }
    }

    KiryuScreen *screen = new KiryuScreen(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!screen->wasCreated()) {
        std::cerr << "Could not create screen!" << std::endl;
        return EXIT_FAILURE;
    }

    screen->bindTexture(CUSTOM_FRAME2);
    int counter = 0;

    while (screen->isActive()) {
        screen->render();
        switch (counter) {
            case 0:
                screen->bindTexture(CUSTOM_FRAME1);
                counter = 1;
                break;
            case 1:
                screen->bindTexture(CUSTOM_FRAME2);
                counter = 0;
                break;
            default:
                counter = 0;
                break;
        }
    }

    delete screen;

    MatrixXd m(2,2);
    m(0,0) = 3;
    m(1,0) = 2.5;
    m(0,1) = -1;
    m(1,1) = m(1,0) + m(0,1);
    std::cout << m << std::endl;

    return EXIT_SUCCESS;
}
