// SolverWindow.h (SFML)
#ifndef SOLVERWINDOW_H
#define SOLVERWINDOW_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "City.h"
#include "Tour.h"
#include "SimulatedAnnealing.h"

class SolverWindow {
private:
    sf::RenderWindow window;
    
    // Algorithm State
    std::vector<City> cityData;
    SimulatedAnnealing solver; // SA Instance
    Tour currentTour;          // The current tour (mutated by SA)
    Tour bestTour;             // The best tour found so far
    
    // Simulation Control
    bool isRunning = false;
    sf::Clock updateClock;
    sf::Font font; // Font for text display

    // Core Functions
    void processEvents();
    void update(float deltaTime);
    void draw();
    void drawTour(const Tour& tour, const sf::Color& color, float thickness);
    void drawControls(const sf::Text& statusText, const sf::Text& distanceText, const sf::Text& tempText);
    
    // Simulation Logic
    void initializeCities();
    void resetSimulation();

public:
    SolverWindow();
    void run(); 
};

#endif // SOLVERWINDOW_H