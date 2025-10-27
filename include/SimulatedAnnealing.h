#ifndef SIMULATEDANNEALING_H
#define SIMULATEDANNEALING_H

#include "Tour.h"
#include <random>
#include <chrono>

class SimulatedAnnealing {
private:
    double initialTemp;
    double coolingRate;
    int iterationsPerTemp; 

    double currentTemp;
    long totalIterations;
    
    std::mt19937 generator;

    double acceptanceProbability(double deltaEnergy, double temperature) const;

public:
    SimulatedAnnealing(double temp, double rate, int iter);

    // --- Methods for Stepwise Execution (The core refactoring) ---
    bool runOneIteration(Tour& currentTour); 
    void coolTemperature();
    void reset(double initialTemp, double coolingRate, int iter);

    // --- Getters fully defined in header (FIX: Removed from .cpp) ---
    long getTotalIterations() const { return totalIterations; }
    double getCurrentTemperature() const { return currentTemp; }

    void displayParameters() const; 
};

#endif // SIMULATEDANNEALING_H