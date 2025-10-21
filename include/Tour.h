#ifndef TOUR_H
#define TOUR_H

#include "City.h"
#include <vector>
#include <algorithm>

class Tour {
private:
    std::vector<City> cities;
    double totalDistance;
    
    // Calculate the total distance of the tour
    void calculateTotalDistance();

public:
    // Constructors
    Tour();
    Tour(std::vector<City> cities);
    
    // Add a city to the tour
    void addCity(const City& city);
    
    // Get city at index
    City getCity(int index) const;
    
    // Get number of cities
    int getSize() const;
    
    // Get all cities
    std::vector<City> getCities() const;
    
    // Get total distance
    double getTotalDistance();
    
    // Generate a random tour
    void generateRandomTour();
    
    // Swap two cities in the tour (for neighbor generation)
    void swapCities(int index1, int index2);
    
    // Create a copy of this tour
    Tour clone() const;
    
    // Display the tour
    void display() const;
    
    // Clear the tour
    void clear();
};

#endif // TOUR_H