#ifndef TSP_SOLVER_H
#define TSP_SOLVER_H

#include <vector>
#include <random>
#include <chrono>
#include <iostream>

struct City {
    double x, y;
    int id;
    
    City(double x = 0, double y = 0, int id = 0) : x(x), y(y), id(id) {}
};

struct TSPSolution {
    std::vector<int> tour;
    double distance;
    
    TSPSolution() : distance(0.0) {}
};

class TSPSolver {
public:
    TSPSolver();
    
    void setCities(const std::vector<City>& cities);
    TSPSolution solve();
    TSPSolution getCurrentSolution() const;
    std::vector<City> getCities() const { return cities; }
    void reset();
    
    // Getter methods for UI
    double getTemperature() const { return temperature; }
    int getIteration() const { return iteration; }
    
    // Algorithm parameters
    void setInitialTemperature(double temp) { initialTemperature = temp; }
    void setCoolingRate(double rate) { coolingRate = rate; }
    void setMinTemperature(double temp) { minTemperature = temp; }
    void setMaxIterations(int iterations) { maxIterations = iterations; }
    
    // Control methods
    void start();
    void pause();
    void resume();
    bool isRunning() const { return running; }
    bool isFinished() const { return finished; }
    
    // For step-by-step execution
    bool step();
    
private:
    std::vector<City> cities;
    TSPSolution currentSolution;
    TSPSolution bestSolution;
    
    // Algorithm parameters
    double initialTemperature;
    double coolingRate;
    double minTemperature;
    int maxIterations;
    
    // State variables
    double temperature;
    int iteration;
    bool running;
    bool finished;
    std::mt19937 rng;
    
    // Helper methods
    double calculateDistance(const std::vector<int>& tour) const;
    double distance(const City& a, const City& b) const;
    std::vector<int> generateInitialTour() const;
    std::vector<int> generateNeighbor(const std::vector<int>& tour) const;
    double acceptanceProbability(double oldDistance, double newDistance, double temperature) const;
};

#endif // TSP_SOLVER_H