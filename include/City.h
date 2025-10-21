#ifndef CITY_H
#define CITY_H

#include <string>
#include <cmath>

class City {
private:
    std::string name;
    double x;
    double y;

public:
    // Constructors
    City();
    City(std::string name, double x, double y);
    
    // Getters
    std::string getName() const;
    double getX() const;
    double getY() const;
    
    // Setters
    void setName(std::string name);
    void setX(double x);
    void setY(double y);
    
    // Calculate Euclidean distance to another city
    double distanceTo(const City& other) const;
    
    // Display city information
    void display() const;
};

#endif // CITY_H