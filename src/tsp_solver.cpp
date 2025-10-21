#include "tsp_solver.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>
#include <random>

TSPSolver::TSPSolver() 
    : initialTemperature(10000.0),
      coolingRate(0.995),
      minTemperature(1.0),
      maxIterations(100000),
      temperature(initialTemperature),
      iteration(0),
      running(false),
      finished(false),
      rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
}

void TSPSolver::setCities(const std::vector<City>& cities) {
    this->cities = cities;
    reset();
}

void TSPSolver::reset() {
    if (!cities.empty()) {
        currentSolution.tour = generateInitialTour();
        currentSolution.distance = calculateDistance(currentSolution.tour);
        bestSolution = currentSolution;
    } else {
        currentSolution = TSPSolution();
        bestSolution = TSPSolution();
    }
    
    temperature = initialTemperature;
    iteration = 0;
    running = false;
    finished = false;
}

TSPSolution TSPSolver::solve() {
    if (cities.size() < 2) {
        return currentSolution;
    }
    
    reset();
    running = true;
    
    while (temperature > minTemperature && iteration < maxIterations && running) {
        // Generate neighbor solution
        std::vector<int> newTour = generateNeighbor(currentSolution.tour);
        double newDistance = calculateDistance(newTour);
        
        // Decide whether to accept the new solution
        if (newDistance < currentSolution.distance || 
            acceptanceProbability(currentSolution.distance, newDistance, temperature) > std::uniform_real_distribution<double>(0.0, 1.0)(rng)) {
            
            currentSolution.tour = newTour;
            currentSolution.distance = newDistance;
            
            // Update best solution if this is better
            if (newDistance < bestSolution.distance) {
                bestSolution = currentSolution;
            }
        }
        
        // Cool down
        temperature *= coolingRate;
        iteration++;
    }
    
    running = false;
    finished = true;
    return bestSolution;
}

bool TSPSolver::step() {
    if (cities.size() < 2 || temperature <= minTemperature || iteration >= maxIterations) {
        running = false;
        finished = true;
        return false;
    }
    
    // Generate neighbor solution
    std::vector<int> newTour = generateNeighbor(currentSolution.tour);
    double newDistance = calculateDistance(newTour);
    
    // Decide whether to accept the new solution
    if (newDistance < currentSolution.distance || 
        acceptanceProbability(currentSolution.distance, newDistance, temperature) > std::uniform_real_distribution<double>(0.0, 1.0)(rng)) {
        
        currentSolution.tour = newTour;
        currentSolution.distance = newDistance;
        
        // Update best solution if this is better
        if (newDistance < bestSolution.distance) {
            bestSolution = currentSolution;
        }
    }
    
    // Cool down
    temperature *= coolingRate;
    iteration++;
    
    return true;
}

TSPSolution TSPSolver::getCurrentSolution() const {
    return bestSolution;
}

void TSPSolver::start() {
    if (cities.size() < 2) return;
    
    if (finished) {
        reset();
    }
    
    running = true;
}

void TSPSolver::pause() {
    running = false;
}

void TSPSolver::resume() {
    if (finished) {
        reset();
    }
    running = true;
}

std::vector<int> TSPSolver::generateInitialTour() const {
    std::vector<int> tour(cities.size());
    for (size_t i = 0; i < cities.size(); ++i) {
        tour[i] = static_cast<int>(i);
    }
    
    // Shuffle the tour
    std::shuffle(tour.begin(), tour.end(), rng);
    return tour;
}

std::vector<int> TSPSolver::generateNeighbor(const std::vector<int>& tour) const {
    std::vector<int> newTour = tour;
    
    // Swap two random cities
    if (newTour.size() > 1) {
        std::uniform_int_distribution<size_t> dist(0, newTour.size() - 1);
        size_t i = dist(rng);
        size_t j = dist(rng);
        
        // Ensure i != j
        while (j == i) {
            j = dist(rng);
        }
        
        std::swap(newTour[i], newTour[j]);
    }
    
    return newTour;
}

double TSPSolver::calculateDistance(const std::vector<int>& tour) const {
    if (tour.size() < 2) return 0.0;
    
    double totalDistance = 0.0;
    for (size_t i = 0; i < tour.size(); ++i) {
        const City& from = cities[tour[i]];
        const City& to = cities[tour[(i + 1) % tour.size()]];
        totalDistance += distance(from, to);
    }
    
    return totalDistance;
}

double TSPSolver::distance(const City& a, const City& b) const {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

double TSPSolver::acceptanceProbability(double oldDistance, double newDistance, double temperature) const {
    if (newDistance < oldDistance) {
        return 1.0;
    }
    return std::exp((oldDistance - newDistance) / temperature);
}