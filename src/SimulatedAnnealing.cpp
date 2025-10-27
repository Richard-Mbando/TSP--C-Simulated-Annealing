#include "SimulatedAnnealing.h"
#include <iostream>
#include <cmath>

// Constructor Definition
SimulatedAnnealing::SimulatedAnnealing(double temp, double rate, int iter)
    : initialTemp(temp), coolingRate(rate), iterationsPerTemp(iter), 
      currentTemp(temp), totalIterations(0) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    generator.seed(seed);
}

// Reset Method
void SimulatedAnnealing::reset(double temp, double rate, int iter) {
    initialTemp = temp;
    coolingRate = rate;
    iterationsPerTemp = iter;
    currentTemp = initialTemp;
    totalIterations = 0;
}

// Acceptance Criterion
double SimulatedAnnealing::acceptanceProbability(double deltaEnergy, double temperature) const {
    if (deltaEnergy < 0) {
        return 1.0; 
    }
    return std::exp(-deltaEnergy / temperature);
}

// Core Execution Method (Runs one single step/iteration)
bool SimulatedAnnealing::runOneIteration(Tour& currentTour) {
    if (currentTemp <= 0.1 || currentTour.getTour().size() < 2) {
        return false; 
    }
    
    totalIterations++;
    
    // 1. Generate Neighbor
    Tour newTour = currentTour.createCopy();
    
    std::uniform_int_distribution<> cityDist(0, newTour.getTour().size() - 1);
    int index1 = cityDist(generator);
    int index2 = cityDist(generator);
    while (index1 == index2) { index2 = cityDist(generator); }
    
    newTour.swapCities(index1, index2);

    // 2. Calculate Energy Change (Delta E)
    double deltaEnergy = newTour.getTotalDistance() - currentTour.getTotalDistance();

    // 3. Decision (Metropolis Criterion)
    std::uniform_real_distribution<> dist(0.0, 1.0);
    if (acceptanceProbability(deltaEnergy, currentTemp) > dist(generator)) {
        currentTour = newTour;
        return true;
    }
    
    return false;
}

// Cooling Method
void SimulatedAnnealing::coolTemperature() {
    // Cool the temperature after iterationsPerTemp steps
    if (totalIterations > 0 && totalIterations % iterationsPerTemp == 0) {
        currentTemp *= coolingRate;
    }
}

// Display Method Definition
void SimulatedAnnealing::displayParameters() const {
    std::cout << "SA Parameters:" << std::endl;
    std::cout << " - Initial Temp: " << initialTemp << std::endl;
    std::cout << " - Cooling Rate: " << coolingRate << std::endl;
    std::cout << " - Iterations/Cooling Step: " << iterationsPerTemp << std::endl;
}