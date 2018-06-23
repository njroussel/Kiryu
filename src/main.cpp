#include <iostream>
#include <nanogui/screen.h>
#include <Eigen/Dense>

using Eigen::MatrixXd;


int main() {
    nanogui::init();

    nanogui::Screen myScreen(Eigen::Vector2i(200, 200), "Kiryu");
    myScreen.drawAll();
    myScreen.setVisible(true);

    nanogui::mainloop();

    nanogui::shutdown();

    std::cout << "Hello world!" << std::endl;

    MatrixXd m(2,2);
    m(0,0) = 3;
    m(1,0) = 2.5;
    m(0,1) = -1;
    m(1,1) = m(1,0) + m(0,1);
    std::cout << m << std::endl;
    return 0;
}
