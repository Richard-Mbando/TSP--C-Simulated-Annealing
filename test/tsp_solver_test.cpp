#include <iostream>
#include <vector>
#include <cassert>
#include "../include/tsp_solver.h"

void testBasicFunctionality() {
    std::cout << "Testing basic TSP solver functionality..." << std::endl;
    
    TSPSolver solver;
    
    // Create a simple test case with 4 cities forming a square
    std::vector<City> cities = {
        City(0, 0, 0),    // Bottom-left
        City(0, 1, 1),    // Top-left
        City(1, 1, 2),    // Top-right
        City(1, 0, 3)     // Bottom-right
    };
    
    solver.setCities(cities);
    
    // Test that cities are set correctly
    std::vector<City> retrievedCities = solver.getCities();
    assert(retrievedCities.size() == 4);
    
    // Test initial solution
    TSPSolution initialSolution = solver.getCurrentSolution();
    assert(initialSolution.tour.size() == 4);
    
    // Test solving
    TSPSolution solution = solver.solve();
    
    // Verify solution has correct number of cities
    assert(solution.tour.size() == 4);
    
    // Verify all cities are visited exactly once
    std::vector<bool> visited(4, false);
    for (int cityId : solution.tour) {
        assert(cityId >= 0 && cityId < 4);
        assert(!visited[cityId]);  // Should not have been visited before
        visited[cityId] = true;
    }
    
    // All cities should be visited
    for (bool v : visited) {
        assert(v);
    }
    
    // Distance should be positive
    assert(solution.distance > 0);
    
    std::cout << "Basic functionality test passed!" << std::endl;
}

void testParameterSetting() {
    std::cout << "Testing parameter setting..." << std::endl;
    
    TSPSolver solver;
    
    // Test setting parameters
    solver.setInitialTemperature(5000.0);
    solver.setCoolingRate(0.95);
    solver.setMinTemperature(0.5);
    solver.setMaxIterations(50000);
    
    // Create simple test case
    std::vector<City> cities = {
        City(0, 0, 0),
        City(1, 1, 1)
    };
    
    solver.setCities(cities);
    
    // Test that we can start and stop
    solver.start();
    assert(solver.isRunning());
    
    solver.pause();
    assert(!solver.isRunning());
    
    std::cout << "Parameter setting test passed!" << std::endl;
}

int main() {
    std::cout << "Running TSP Solver Tests" << std::endl;
    std::cout << "========================" << std::endl;
    
    try {
        testBasicFunctionality();
        testParameterSetting();
        
        std::cout << std::endl << "All tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}