#include <iostream>
#include <nanogui/screen.h>

int main() {
    nanogui::init();

    nanogui::Screen myScreen(Eigen::Vector2i(200, 200), "Kiryu");
    myScreen.drawAll();
    myScreen.setVisible(true);

    nanogui::mainloop();

    nanogui::shutdown();

    std::cout << "Hello world!" << std::endl;
    return 0;
}
