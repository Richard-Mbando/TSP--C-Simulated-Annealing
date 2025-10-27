#ifndef CITY_H
#define CITY_H

#include <string>
#include <cmath>
#include <iostream>

class City {
private:
    std::string name;
    double x;
    double y;

public:
    // DECLARATION only (Implementation moved to City.cpp)
    City(std::string name, double x, double y); 

    // DECLARATIONS only
    std::string getName() const; 
    double getX() const;
    double getY() const;

    // DECLARATION only
    double distanceTo(const City& other) const;

    // DECLARATION only
    void display() const;
    
    // (Optional) Remove the default constructor you tried to define in City.cpp,
    // as it's not needed if every city is initialized with coordinates.
};

#endif // CITY_H