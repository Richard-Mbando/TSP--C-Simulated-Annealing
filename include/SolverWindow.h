#ifndef SOLVERWINDOW_H
#define SOLVERWINDOW_H

#include <SFML/Graphics.hpp>
#include "City.h"
#include "Tour.h"
#include "SimulatedAnnealing.h"
#include <vector>
#include <string>

class SolverWindow {
private:
    sf::RenderWindow window;
    sf::Font font;
    
    // Algorithm components
    std::vector<City> cityData;
    Tour currentTour;
    Tour bestTour;
    SimulatedAnnealing solver;
    
    // State management
    bool isRunning;
    bool isPaused;
    bool isAddingCity;
    int iterationCount;
    
    // UI Constants
    static const int WINDOW_WIDTH = 1200;
    static const int WINDOW_HEIGHT = 700;
    static const int CANVAS_WIDTH = 750;
    static const int CANVAS_HEIGHT = 650;
    static const int PANEL_WIDTH = 400;
    static const float VISUAL_SCALE;
    static const float OFFSET_X;
    static const float OFFSET_Y;
    
    // Button states
    sf::RectangleShape startButton;
    sf::RectangleShape pauseButton;
    sf::RectangleShape resetButton;
    sf::RectangleShape addCityButton;
    sf::RectangleShape removeCityButton;
    
    sf::Text startButtonText;
    sf::Text pauseButtonText;
    sf::Text resetButtonText;
    sf::Text addCityButtonText;
    sf::Text removeCityButtonText;
    
    // Helper methods
    void initializeCities();
    void resetSimulation();
    void processEvents();
    void update(float deltaTime);
    void draw();
    void runAlgorithmStep();
    
    // Drawing methods
    void drawCanvas();
    void drawTour(const Tour& tour, const sf::Color& color, float thickness);
    void drawCities();
    void drawControlPanel();
    void drawButton(const sf::RectangleShape& button, const sf::Text& text);
    void drawStatistics();
    
    // UI helper methods
    void setupButtons();
    void updateButtonStates();
    bool isMouseOverButton(const sf::RectangleShape& button, const sf::Vector2i& mousePos);
    void handleButtonClick(const sf::Vector2i& mousePos);
    void handleCanvasClick(const sf::Vector2i& mousePos);
    
    // Text creation helper
    sf::Text createText(const std::string& str, unsigned int size, const sf::Color& color, float x, float y);
    sf::RectangleShape createButton(float x, float y, float width, float height, const sf::Color& color);

public:
    SolverWindow();
    void run();
};

#endif // SOLVERWINDOW_H