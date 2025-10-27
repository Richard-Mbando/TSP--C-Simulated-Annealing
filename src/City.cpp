#include "City.h"
#include <cmath>
#include <iostream>

// Constructor Definition
City::City(std::string name, double x, double y) : name(name), x(x), y(y) {}

// Getter Definitions
std::string City::getName() const {
    return name;
}

double City::getX() const {
    return x;
}

double City::getY() const {
    return y;
}

// Distance Calculation Definition
double City::distanceTo(const City& other) const {
    double dx = other.x - x;
    double dy = other.y - y;
    return std::sqrt(dx * dx + dy * dy);
}

// Display Method Definition
void City::display() const {
    std::cout << "City: " << name << " (" << x << ", " << y << ")" << std::endl;
}

