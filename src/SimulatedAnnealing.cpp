#include "../include/SimulatedAnnealing.h"
#include <iostream>
#include <cmath>
#include <random>
#include <iomanip>

// Constructor
SimulatedAnnealing::SimulatedAnnealing(double initialTemp, 
                                       double coolingRate, 
                                       int iterationsPerTemp)
    : initialTemperature(initialTemp),
      coolingRate(coolingRate),
      iterationsPerTemp(iterationsPerTemp),
      currentTemperature(initialTemp),
      totalIterations(0) {}

// Set parameters
void SimulatedAnnealing::setInitialTemperature(double temp) {
    initialTemperature = temp;
    currentTemperature = temp;
}

void SimulatedAnnealing::setCoolingRate(double rate) {
    coolingRate = rate;
}

void SimulatedAnnealing::setIterationsPerTemp(int iterations) {
    iterationsPerTemp = iterations;
}

// Set callback for updates
void SimulatedAnnealing::setUpdateCallback(std::function<void(Tour, double, int)> callback) {
    updateCallback = callback;
}

// Calculate acceptance probability
double SimulatedAnnealing::acceptanceProbability(double currentEnergy, 
                                                 double newEnergy, 
                                                 double temperature) {
    // If new solution is better, always accept
    if (newEnergy < currentEnergy) {
        return 1.0;
    }
    
    // If new solution is worse, accept with probability based on temperature
    // Higher temperature = more likely to accept worse solutions
    return std::exp(-(newEnergy - currentEnergy) / temperature);
}

// Generate a neighbor solution by swapping two random cities
Tour SimulatedAnnealing::generateNeighbor(const Tour& tour) {
    Tour neighbor = tour.clone();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, tour.getSize() - 1);
    
    // Swap two random cities
    int index1 = dis(gen);
    int index2 = dis(gen);
    
    // Make sure we're swapping different cities
    while (index1 == index2) {
        index2 = dis(gen);
    }
    
    neighbor.swapCities(index1, index2);
    
    return neighbor;
}

// Main solving algorithm
Tour SimulatedAnnealing::solve(std::vector<City> cities) {
    if (cities.size() < 2) {
        std::cout << "Error: Need at least 2 cities to solve TSP" << std::endl;
        return Tour();
    }
    
    std::cout << "\n=== Starting Simulated Annealing ===" << std::endl;
    std::cout << "Number of cities: " << cities.size() << std::endl;
    std::cout << "Initial temperature: " << initialTemperature << std::endl;
    std::cout << "Cooling rate: " << coolingRate << std::endl;
    std::cout << "Iterations per temperature: " << iterationsPerTemp << std::endl;
    
    // Initialize with random tour
    currentTour = Tour(cities);
    currentTour.generateRandomTour();
    bestTour = currentTour.clone();
    
    currentTemperature = initialTemperature;
    totalIterations = 0;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    double bestDistance = bestTour.getTotalDistance();
    std::cout << "Initial tour distance: " << std::fixed << std::setprecision(2) 
              << bestDistance << std::endl;
    
    // Main annealing loop
    while (currentTemperature > 1.0) {
        for (int i = 0; i < iterationsPerTemp; i++) {
            totalIterations++;
            
            // Generate neighbor solution
            Tour neighborTour = generateNeighbor(currentTour);
            
            double currentEnergy = currentTour.getTotalDistance();
            double neighborEnergy = neighborTour.getTotalDistance();
            
            // Calculate acceptance probability
            double probability = acceptanceProbability(currentEnergy, 
                                                      neighborEnergy, 
                                                      currentTemperature);
            
            // Decide whether to accept the neighbor
            if (dis(gen) < probability) {
                currentTour = neighborTour;
                
                // Update best tour if we found a better one
                if (neighborEnergy < bestDistance) {
                    bestTour = neighborTour.clone();
                    bestDistance = neighborEnergy;
                    
                    std::cout << "New best distance found: " 
                              << std::fixed << std::setprecision(2) 
                              << bestDistance 
                              << " (Iteration: " << totalIterations 
                              << ", Temp: " << currentTemperature << ")" 
                              << std::endl;
                }
            }
            
            // Call update callback if set (for GUI updates)
            if (updateCallback) {
                updateCallback(currentTour, currentTemperature, totalIterations);
            }
        }
        
        // Cool down
        currentTemperature *= coolingRate;
    }
    
    std::cout << "\n=== Simulated Annealing Complete ===" << std::endl;
    std::cout << "Total iterations: " << totalIterations << std::endl;
    std::cout << "Final best distance: " << std::fixed << std::setprecision(2) 
              << bestDistance << std::endl;
    
    return bestTour;
}

// Get best tour
Tour SimulatedAnnealing::getBestTour() const {
    return bestTour;
}

// Get current temperature
double SimulatedAnnealing::getCurrentTemperature() const {
    return currentTemperature;
}

// Get total iterations
int SimulatedAnnealing::getTotalIterations() const {
    return totalIterations;
}

// Display parameters
void SimulatedAnnealing::displayParameters() const {
    std::cout << "\n=== Simulated Annealing Parameters ===" << std::endl;
    std::cout << "Initial Temperature: " << initialTemperature << std::endl;
    std::cout << "Cooling Rate: " << coolingRate << std::endl;
    std::cout << "Iterations per Temperature: " << iterationsPerTemp << std::endl;
    std::cout << "====================================\n" << std::endl;
}