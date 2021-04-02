#include "Controller.hpp"

int main(int argc, char **argv)
{
    Controller controller(100, 100);
    controller.Loop();
    return 0;
}