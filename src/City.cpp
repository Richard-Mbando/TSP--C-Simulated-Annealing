#include "../include/City.h"
#include <iostream>
#include <iomanip>

// Default constructor
City::City() : name(""), x(0.0), y(0.0) {}

// Parameterized constructor
City::City(std::string name, double x, double y) 
    : name(name), x(x), y(y) {}

// Getters
std::string City::getName() const {
    return name;
}

double City::getX() const {
    return x;
}

double City::getY() const {
    return y;
}

// Setters
void City::setName(std::string name) {
    this->name = name;
}

void City::setX(double x) {
    this->x = x;
}

void City::setY(double y) {
    this->y = y;
}

// Calculate Euclidean distance between two cities
double City::distanceTo(const City& other) const {
    double dx = x - other.x;
    double dy = y - other.y;
    return std::sqrt(dx * dx + dy * dy);
}

// Display city information
void City::display() const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "City: " << name 
              << " (x: " << x << ", y: " << y << ")" << std::endl;
}