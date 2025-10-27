#ifndef TOUR_H
#define TOUR_H

#include "City.h"
#include <vector>
#include <algorithm>

class Tour {
private:
    std::vector<City> tour;
    double totalDistance;

    // Helper to calculate total distance (Private method)
    void calculateDistance();

public:
    // Constructor
    Tour(const std::vector<City>& initialCities);

    // Getters (Declarations)
    const std::vector<City>& getTour() const;
    double getTotalDistance() const;

    // Path modification methods (Declarations)
    void generateRandomTour();
    void swapCities(int i, int j);
    Tour createCopy() const;

    // Display method (Declaration)
    void display() const;
    
    Tour();
};

#endif // TOUR_H