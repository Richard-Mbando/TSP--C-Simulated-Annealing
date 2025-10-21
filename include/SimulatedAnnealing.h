#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include "Tour.h"
#include <vector>
#include <functional>

class SimulatedAnnealing {
private:
    double initialTemperature;
    double coolingRate;
    int iterationsPerTemp;
    
    Tour currentTour;
    Tour bestTour;
    
    double currentTemperature;
    int totalIterations;
    
    // Callback function for visualization updates (optional)
    std::function<void(Tour, double, int)> updateCallback;
    
    // Calculate acceptance probability
    double acceptanceProbability(double currentEnergy, double newEnergy, double temperature);
    
    // Generate a neighbor solution
    Tour generateNeighbor(const Tour& tour);

public:
    // Constructor
    SimulatedAnnealing(double initialTemp = 10000.0, 
                      double coolingRate = 0.995, 
                      int iterationsPerTemp = 100);
    
    // Set parameters
    void setInitialTemperature(double temp);
    void setCoolingRate(double rate);
    void setIterationsPerTemp(int iterations);
    
    // Set callback for updates
    void setUpdateCallback(std::function<void(Tour, double, int)> callback);
    
    // Solve TSP
    Tour solve(std::vector<City> cities);
    
    // Get best tour found
    Tour getBestTour() const;
    
    // Get current statistics
    double getCurrentTemperature() const;
    int getTotalIterations() const;
    
    // Display algorithm parameters
    void displayParameters() const;
};

#endif // SIMULATED_ANNEALING_H