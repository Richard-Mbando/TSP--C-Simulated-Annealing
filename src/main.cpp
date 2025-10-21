#include "City.h"
#include "Tour.h"
#include "SimulatedAnnealing.h"

#include <iostream>
#include <vector>

void testBasicFunctionality() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "    TSP SOLVER - SIMULATED ANNEALING    " << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Create some test cities
    std::vector<City> cities;
    
    // Small test case: 6 cities
    cities.push_back(City("Blantyre", 0, 0));
    cities.push_back(City("Lilongwe", 100, 150));
    cities.push_back(City("Mzuzu", 50, 250));
    cities.push_back(City("Zomba", 20, 30));
    cities.push_back(City("Karonga", 80, 300));
    cities.push_back(City("Mangochi", 120, 50));
    
    std::cout << "Test Cities:" << std::endl;
    for (const auto& city : cities) {
        city.display();
    }
    
    // Test Tour class
    std::cout << "\n--- Testing Tour Class ---" << std::endl;
    Tour testTour(cities);
    std::cout << "Initial tour distance: " << testTour.getTotalDistance() << std::endl;
    
    testTour.generateRandomTour();
    std::cout << "Random tour distance: " << testTour.getTotalDistance() << std::endl;
    testTour.display();
    
    // Test Simulated Annealing
    std::cout << "\n--- Testing Simulated Annealing ---" << std::endl;
    SimulatedAnnealing sa(10000.0, 0.995, 100);
    sa.displayParameters();
    
    Tour bestTour = sa.solve(cities);
    
    std::cout << "\n--- Final Results ---" << std::endl;
    bestTour.display();
    
    std::cout << "\nAlgorithm Statistics:" << std::endl;
    std::cout << "Total iterations: " << sa.getTotalIterations() << std::endl;
    std::cout << "Final temperature: " << sa.getCurrentTemperature() << std::endl;
}

void testLargerProblem() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "    LARGER TEST CASE (10 CITIES)        " << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::vector<City> cities;
    
    // Generate 10 cities with more varied positions
    cities.push_back(City("City_A", 60, 200));
    cities.push_back(City("City_B", 180, 200));
    cities.push_back(City("City_C", 80, 180));
    cities.push_back(City("City_D", 140, 180));
    cities.push_back(City("City_E", 20, 160));
    cities.push_back(City("City_F", 100, 160));
    cities.push_back(City("City_G", 200, 160));
    cities.push_back(City("City_H", 140, 140));
    cities.push_back(City("City_I", 40, 120));
    cities.push_back(City("City_J", 100, 120));
    
    // Use different parameters for larger problem
    SimulatedAnnealing sa(15000.0, 0.998, 150);
    sa.displayParameters();
    
    Tour bestTour = sa.solve(cities);
    
    std::cout << "\n--- Final Results ---" << std::endl;
    bestTour.display();
}

void interactiveMode() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "        INTERACTIVE MODE                " << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::vector<City> cities;
    int numCities;
    
    std::cout << "Enter number of cities: ";
    std::cin >> numCities;
    
    if (numCities < 2) {
        std::cout << "Need at least 2 cities!" << std::endl;
        return;
    }
    
    std::cin.ignore(); // Clear input buffer
    
    for (int i = 0; i < numCities; i++) {
        std::string name;
        double x, y;
        
        std::cout << "\nCity " << (i + 1) << ":" << std::endl;
        std::cout << "  Name: ";
        std::getline(std::cin, name);
        std::cout << "  X coordinate: ";
        std::cin >> x;
        std::cout << "  Y coordinate: ";
        std::cin >> y;
        std::cin.ignore();
        
        cities.push_back(City(name, x, y));
    }
    
    // Get algorithm parameters
    double initialTemp, coolingRate;
    int iterations;
    
    std::cout << "\n--- Algorithm Parameters ---" << std::endl;
    std::cout << "Initial temperature (default 10000): ";
    std::cin >> initialTemp;
    std::cout << "Cooling rate (default 0.995): ";
    std::cin >> coolingRate;
    std::cout << "Iterations per temperature (default 100): ";
    std::cin >> iterations;
    
    // Solve
    SimulatedAnnealing sa(initialTemp, coolingRate, iterations);
    Tour bestTour = sa.solve(cities);
    
    std::cout << "\n--- Final Results ---" << std::endl;
    bestTour.display();
}

int main() {
    int choice;
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  TRAVELING SALESMAN PROBLEM SOLVER    " << std::endl;
    std::cout << "      Simulated Annealing Algorithm     " << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::cout << "Select mode:" << std::endl;
    std::cout << "1. Basic test (6 cities)" << std::endl;
    std::cout << "2. Larger test (10 cities)" << std::endl;
    std::cout << "3. Interactive mode (custom input)" << std::endl;
    std::cout << "4. Run all tests" << std::endl;
    std::cout << "\nEnter choice (1-4): ";
    std::cin >> choice;
    
    switch (choice) {
        case 1:
            testBasicFunctionality();
            break;
        case 2:
            testLargerProblem();
            break;
        case 3:
            interactiveMode();
            break;
        case 4:
            testBasicFunctionality();
            testLargerProblem();
            break;
        default:
            std::cout << "Invalid choice!" << std::endl;
            return 1;
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "           PROGRAM COMPLETE             " << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    return 0;
}