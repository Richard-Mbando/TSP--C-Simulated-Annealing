#include "SolverWindow.h"
#include <iostream>

// The main entry point of the application.
int main() {
    try {
        SolverWindow app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "An unhandled exception occurred: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
        return 1;
    }
    return 0;
}