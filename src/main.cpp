#include <iostream>

#include <kiryu/render.h>
#include <kiryu/parser.h>

#define KIRYU_GUI_ENABLE true

int main() {
    Parser parser("../res/base.json");
    parser.parse();

    Render render;

    render.render(KIRYU_GUI_ENABLE, "KiryuNormals", parser.getSensor(),
            parser.getIntegrator());

    return EXIT_SUCCESS;
}
