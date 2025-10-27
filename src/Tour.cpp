#include "Tour.h"
#include <iostream>
#include <random>
#include <chrono>

// NEW: Default Constructor Definition
// This allows the Tour members in SolverWindow to be initialized before cityData is ready.
Tour::Tour() : totalDistance(0.0) {
    // tour vector is initialized as empty.
}

// Constructor Definition (Existing)
Tour::Tour(const std::vector<City>& initialCities) : tour(initialCities), totalDistance(0.0) {
    // Only calculate distance if there are cities.
    if (!tour.empty()) {
        calculateDistance();
    }
}

// Distance Calculation Definition: Sums distances between adjacent cities, plus the return to start.
void Tour::calculateDistance() {
    // If the tour is empty or has only one city, the distance is 0.
    if (tour.size() < 2) {
        totalDistance = 0.0;
        return;
    }
    
    totalDistance = 0.0;
    for (size_t i = 0; i < tour.size(); ++i) {
        // (i + 1) % tour.size() handles the wrap-around back to the first city
        const City& nextCity = tour[(i + 1) % tour.size()];
        totalDistance += tour[i].distanceTo(nextCity);
    }
}

// Generates a random initial tour using std::shuffle.
void Tour::generateRandomTour() {
    if (tour.size() < 2) return;
    
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(tour.begin(), tour.end(), std::default_random_engine(seed));
    calculateDistance();
}

// Swaps two cities (the basic move for Simulated Annealing).
void Tour::swapCities(int i, int j) {
    // Sanity check
    if (i >= 0 && i < tour.size() && j >= 0 && j < tour.size()) {
        std::swap(tour[i], tour[j]);
        calculateDistance();
    }
}

// Creates a deep copy of the tour.
Tour Tour::createCopy() const {
    // Use the parameterized constructor to create the copy
    Tour copy(tour);
    // Explicitly copy the totalDistance (though the constructor will recalculate it)
    copy.totalDistance = totalDistance; 
    return copy;
}

// Getters Definition
const std::vector<City>& Tour::getTour() const { return tour; }
double Tour::getTotalDistance() const { return totalDistance; }

// Display Method Definition
void Tour::display() const {
    if (tour.empty()) {
        std::cout << "Tour Path: Empty" << std::endl;
        std::cout << "Total Distance: 0.0" << std::endl;
        return;
    }
    
    std::cout << "Tour Path: ";
    for (const auto& city : tour) {
        std::cout << city.getName() << " -> ";
    }
    std::cout << tour[0].getName() << " (Start/End)" << std::endl;
    std::cout << "Total Distance: " << totalDistance << std::endl;
}