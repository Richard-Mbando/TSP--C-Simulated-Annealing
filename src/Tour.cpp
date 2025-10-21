#include "../include/Tour.h"
#include <iostream>
#include <random>
#include <iomanip>

// Default constructor
Tour::Tour() : totalDistance(0.0) {}

// Parameterized constructor
Tour::Tour(std::vector<City> cities) : cities(cities), totalDistance(0.0) {
    calculateTotalDistance();
}

// Add a city to the tour
void Tour::addCity(const City& city) {
    cities.push_back(city);
    calculateTotalDistance();
}

// Get city at index
City Tour::getCity(int index) const {
    if (index >= 0 && index < cities.size()) {
        return cities[index];
    }
    return City(); // Return empty city if index out of bounds
}

// Get number of cities
int Tour::getSize() const {
    return cities.size();
}

// Get all cities
std::vector<City> Tour::getCities() const {
    return cities;
}

// Calculate total distance of the tour
void Tour::calculateTotalDistance() {
    totalDistance = 0.0;
    
    if (cities.size() < 2) {
        return;
    }
    
    // Calculate distance between consecutive cities
    for (size_t i = 0; i < cities.size() - 1; i++) {
        totalDistance += cities[i].distanceTo(cities[i + 1]);
    }
    
    // Add distance from last city back to first city (complete the tour)
    totalDistance += cities[cities.size() - 1].distanceTo(cities[0]);
}

// Get total distance
double Tour::getTotalDistance() {
    calculateTotalDistance();
    return totalDistance;
}

// Generate a random tour by shuffling cities
void Tour::generateRandomTour() {
    if (cities.empty()) return;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(cities.begin(), cities.end(), gen);
    
    calculateTotalDistance();
}

// Swap two cities in the tour
void Tour::swapCities(int index1, int index2) {
    if (index1 >= 0 && index1 < cities.size() && 
        index2 >= 0 && index2 < cities.size()) {
        std::swap(cities[index1], cities[index2]);
        calculateTotalDistance();
    }
}

// Create a copy of this tour
Tour Tour::clone() const {
    return Tour(cities);
}

// Display the tour
void Tour::display() const {
    std::cout << "\n=== Tour Information ===" << std::endl;
    std::cout << "Number of cities: " << cities.size() << std::endl;
    std::cout << "Total distance: " << std::fixed << std::setprecision(2) 
              << totalDistance << std::endl;
    std::cout << "\nRoute:" << std::endl;
    
    for (size_t i = 0; i < cities.size(); i++) {
        std::cout << (i + 1) << ". " << cities[i].getName() << std::endl;
    }
    
    if (!cities.empty()) {
        std::cout << (cities.size() + 1) << ". " << cities[0].getName() 
                  << " (return to start)" << std::endl;
    }
    
    std::cout << "========================\n" << std::endl;
}

// Clear the tour
void Tour::clear() {
    cities.clear();
    totalDistance = 0.0;
}