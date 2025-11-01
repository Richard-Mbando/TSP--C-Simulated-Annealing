#include "SolverWindow.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <SFML/System/Angle.hpp> // Required for sf::degrees
#include <SFML/System/Vector2.hpp> // Required for sf::Vector2u and sf::Vector2f

// Initialize static constants
const float SolverWindow::VISUAL_SCALE = 1.5f;
const float SolverWindow::OFFSET_X = 40.0f;
const float SolverWindow::OFFSET_Y = 40.0f;

SolverWindow::SolverWindow() 
    // SFML 3.x FIX: sf::VideoMode now takes a single sf::Vector2u argument
    : window(sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)), "TSP - Simulated Annealing Solver", sf::Style::Titlebar | sf::Style::Close),
      solver(10000.0, 0.995, 100),
      isRunning(false),
      isPaused(false),
      isAddingCity(false),
      iterationCount(0),
      // SFML 3.x FIX: Initialize all sf::Text members with the font object to satisfy the new constructor requirement.
      startButtonText(font),
      pauseButtonText(font),
      resetButtonText(font),
      addCityButtonText(font),
      removeCityButtonText(font)
{
    window.setFramerateLimit(60);
    
    // Load font - tries multiple locations
    // SFML 3.x FIX: loadFromFile is replaced by openFromFile for sf::Font
    if (!font.openFromFile("arial.ttf")) {
        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
            if (!font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
                std::cerr << "Warning: Could not load font. Using default font rendering." << std::endl;
            }
        }
    }
    
    setupButtons();
    initializeCities();
    resetSimulation();
}

void SolverWindow::setupButtons() {
    // Start Button (Green)
    startButton = createButton(800, 100, 150, 45, sf::Color(76, 175, 80));
    startButtonText = createText("START", 16, sf::Color::White, 850, 113);
    
    // Pause Button (Yellow)
    pauseButton = createButton(970, 100, 150, 45, sf::Color(255, 193, 7));
    pauseButtonText = createText("PAUSE", 16, sf::Color::White, 1020, 113);
    
    // Reset Button (Red)
    resetButton = createButton(800, 160, 320, 45, sf::Color(244, 67, 54));
    resetButtonText = createText("RESET", 16, sf::Color::White, 950, 173);
    
    // Add City Button (Blue)
    addCityButton = createButton(800, 240, 320, 40, sf::Color(33, 150, 243));
    addCityButtonText = createText("ADD CITY (Click Canvas)", 14, sf::Color::White, 850, 252);
    
    // Remove City Button (Orange)
    removeCityButton = createButton(800, 290, 320, 40, sf::Color(255, 87, 34));
    removeCityButtonText = createText("REMOVE LAST CITY", 14, sf::Color::White, 870, 302);
}

sf::RectangleShape SolverWindow::createButton(float x, float y, float width, float height, const sf::Color& color) {
    sf::RectangleShape button(sf::Vector2f(width, height));
    // SFML 3.x FIX: setPosition now requires a single sf::Vector2f argument
    button.setPosition(sf::Vector2f(x, y));
    button.setFillColor(color);
    button.setOutlineThickness(2);
    button.setOutlineColor(sf::Color(0, 0, 0, 50));
    return button;
}

sf::Text SolverWindow::createText(const std::string& str, unsigned int size, const sf::Color& color, float x, float y) {
    // SFML 3.x FIX: sf::Text requires a font argument in its constructor (done via the member initialization)
    sf::Text text(font); 
    
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    // SFML 3.x FIX: setPosition now requires a single sf::Vector2f argument
    text.setPosition(sf::Vector2f(x, y));
    return text;
}

void SolverWindow::initializeCities() {
    cityData.clear();
    cityData.push_back(City("A", 80, 150));
    cityData.push_back(City("B", 220, 140));
    cityData.push_back(City("C", 100, 200));
    cityData.push_back(City("D", 180, 220));
    cityData.push_back(City("E", 50, 260));
    cityData.push_back(City("F", 140, 280));
    cityData.push_back(City("G", 250, 250));
    cityData.push_back(City("H", 200, 180));
    cityData.push_back(City("I", 70, 320));
    cityData.push_back(City("J", 160, 340));
}

void SolverWindow::resetSimulation() {
    solver.reset(10000.0, 0.995, 100);
    
    if (cityData.size() >= 2) {
        currentTour = Tour(cityData);
        currentTour.generateRandomTour();
        bestTour = currentTour.createCopy();
    } else {
        currentTour = Tour();
        bestTour = Tour();
    }
    
    isRunning = false;
    isPaused = false;
    iterationCount = 0;
    updateButtonStates();
}

void SolverWindow::updateButtonStates() {
    // Update button colors based on state
    if (!isRunning) {
        startButton.setFillColor(sf::Color(76, 175, 80)); // Green
        pauseButton.setFillColor(sf::Color(150, 150, 150)); // Grey (disabled)
    } else {
        startButton.setFillColor(sf::Color(150, 150, 150)); // Grey (disabled)
        if (isPaused) {
            pauseButton.setFillColor(sf::Color(76, 175, 80)); // Green (resume)
            pauseButtonText.setString("RESUME");
        } else {
            pauseButton.setFillColor(sf::Color(255, 193, 7)); // Yellow (pause)
            pauseButtonText.setString("PAUSE");
        }
    }
    
    // Update add city button based on mode
    if (isAddingCity) {
        addCityButton.setFillColor(sf::Color(244, 67, 54)); // Red when active
        addCityButtonText.setString("CANCEL ADDING");
    } else {
        addCityButton.setFillColor(sf::Color(33, 150, 243)); // Blue
        addCityButtonText.setString("ADD CITY (Click Canvas)");
    }
}

bool SolverWindow::isMouseOverButton(const sf::RectangleShape& button, const sf::Vector2i& mousePos) {
    sf::FloatRect bounds = button.getGlobalBounds();
    // SFML 3.x FIX: contains now requires a single sf::Vector2f argument
    return bounds.contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)));
}

void SolverWindow::handleButtonClick(const sf::Vector2i& mousePos) {
    if (isMouseOverButton(startButton, mousePos) && !isRunning) {
        if (cityData.size() < 2) {
            std::cout << "Add at least 2 cities before starting!" << std::endl;
            return;
        }
        isRunning = true;
        isPaused = false;
        updateButtonStates();
    }
    else if (isMouseOverButton(pauseButton, mousePos) && isRunning) {
        isPaused = !isPaused;
        updateButtonStates();
    }
    else if (isMouseOverButton(resetButton, mousePos)) {
        resetSimulation();
    }
    else if (isMouseOverButton(addCityButton, mousePos) && !isRunning) {
        isAddingCity = !isAddingCity;
        updateButtonStates();
    }
    else if (isMouseOverButton(removeCityButton, mousePos) && !isRunning && !cityData.empty()) {
        cityData.pop_back();
        resetSimulation();
    }
}

void SolverWindow::handleCanvasClick(const sf::Vector2i& mousePos) {
    if (!isAddingCity || isRunning) return;
    
    // Check if click is within canvas bounds
    if (mousePos.x < OFFSET_X || mousePos.x > CANVAS_WIDTH || 
        mousePos.y < OFFSET_Y || mousePos.y > CANVAS_HEIGHT) {
        return;
    }
    
    // Convert screen coordinates to city coordinates
    double cityX = (mousePos.x - OFFSET_X) / VISUAL_SCALE;
    double cityY = (mousePos.y - OFFSET_Y) / VISUAL_SCALE;
    
    // Create new city
    std::string cityName = std::string(1, 'A' + static_cast<char>(cityData.size()));
    cityData.push_back(City(cityName, cityX, cityY));
    
    resetSimulation();
    isAddingCity = false;
    updateButtonStates();
}

void SolverWindow::processEvents() {
    // SFML 3.x FIX: pollEvent no longer takes an argument and returns std::optional<sf::Event>
    while (auto eventOpt = window.pollEvent()) {
        
        // 1. Handle Closed Event (if the optional contains a value)
        if (eventOpt->is<sf::Event::Closed>()) { 
            window.close();
        }
        
        // 2. Handle Mouse Button Press
        else if (eventOpt->is<sf::Event::MouseButtonPressed>()) {
            // SFML 3.x FIX: Use getIf<T>() and dereference the pointer 
            const auto& mouseEvent = *eventOpt->getIf<sf::Event::MouseButtonPressed>();
            
            // SFML 3.x FIX: Mouse position is accessed via the 'position' member.
            if (mouseEvent.button == sf::Mouse::Button::Left) {
                // Use position data from the event struct
                // OLD: sf::Vector2i mousePos(mouseEvent.x, mouseEvent.y);
                sf::Vector2i mousePos = mouseEvent.position; 
                
                // Check if click is in control panel area
                if (mousePos.x > CANVAS_WIDTH) {
                    handleButtonClick(mousePos);
                } else {
                    handleCanvasClick(mousePos);
                }
            }
        }
        
        // 3. Handle Key Press
        else if (eventOpt->is<sf::Event::KeyPressed>()) {
            // SFML 3.x FIX: Use getIf<T>() and dereference the pointer
            const auto& keyEvent = *eventOpt->getIf<sf::Event::KeyPressed>();
            
            // SFML 3.x FIX: Key code is accessed directly via the 'code' member, not 'key.code'.
            if (keyEvent.code == sf::Keyboard::Key::Space && cityData.size() >= 2) { 
                if (!isRunning) {
                    isRunning = true;
                    isPaused = false;
                } else {
                    isPaused = !isPaused;
                }
                updateButtonStates();
            }
            // SFML 3.x FIX: Key code is accessed directly via the 'code' member, not 'key.code'.
            else if (keyEvent.code == sf::Keyboard::Key::R) {
                resetSimulation();
            }
        }
    }
}

void SolverWindow::runAlgorithmStep() {
    const int ITERS_PER_FRAME = 10;
    for (int i = 0; i < ITERS_PER_FRAME; ++i) {
        solver.runOneIteration(currentTour);
        iterationCount++;
        
        if (currentTour.getTotalDistance() < bestTour.getTotalDistance()) {
            bestTour = currentTour.createCopy();
        }
    }
    
    solver.coolTemperature();
    
    if (solver.getCurrentTemperature() <= 0.1) {
        isRunning = false;
        updateButtonStates();
    }
}

void SolverWindow::update(float deltaTime) {
    if (isRunning && !isPaused && solver.getCurrentTemperature() > 0.1) {
        runAlgorithmStep();
    }
}

void SolverWindow::drawCanvas() {
    // Draw canvas background
    sf::RectangleShape canvas(sf::Vector2f(CANVAS_WIDTH, CANVAS_HEIGHT));
    // SFML 3.x FIX: setPosition now requires a single sf::Vector2f argument
    canvas.setPosition(sf::Vector2f(0, 0));
    canvas.setFillColor(sf::Color::White);
    canvas.setOutlineThickness(3);
    canvas.setOutlineColor(sf::Color(200, 200, 200));
    window.draw(canvas);
    
    // Draw title on canvas
    sf::Text title = createText("TSP - Simulated Annealing", 24, sf::Color(50, 50, 50), 20, 10);
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    
    // Draw instruction if adding city
    if (isAddingCity) {
        sf::RectangleShape instructionBox(sf::Vector2f(280, 40));
        // SFML 3.x FIX: setPosition now requires a single sf::Vector2f argument
        instructionBox.setPosition(sf::Vector2f(CANVAS_WIDTH / 2 - 140, 50));
        instructionBox.setFillColor(sf::Color(33, 150, 243, 220));
        instructionBox.setOutlineThickness(2);
        instructionBox.setOutlineColor(sf::Color::White);
        window.draw(instructionBox);
        
        sf::Text instruction = createText("Click on canvas to add city", 14, sf::Color::White, CANVAS_WIDTH / 2 - 110, 60);
        instruction.setStyle(sf::Text::Bold);
        window.draw(instruction);
    }
}

void SolverWindow::drawTour(const Tour& tour, const sf::Color& color, float thickness) {
    const auto& path = tour.getTour();
    if (path.size() < 2) return;
    
    // Draw lines between cities
    for (size_t i = 0; i < path.size(); ++i) {
        const City& current = path[i];
        const City& next = path[(i + 1) % path.size()];
        
        sf::Vector2f start(current.getX() * VISUAL_SCALE + OFFSET_X, current.getY() * VISUAL_SCALE + OFFSET_Y);
        sf::Vector2f end(next.getX() * VISUAL_SCALE + OFFSET_X, next.getY() * VISUAL_SCALE + OFFSET_Y);
        
        sf::Vector2f direction = end - start;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159f;
        
        sf::RectangleShape line(sf::Vector2f(length, thickness));
        line.setFillColor(color);
        line.setPosition(start);
        // SFML 3.x FIX: setRotation now requires sf::Angle (or sf::degrees())
        line.setRotation(sf::degrees(angle)); 
        
        window.draw(line);
    }
}

void SolverWindow::drawCities() {
    for (const auto& city : cityData) {
        float x = city.getX() * VISUAL_SCALE + OFFSET_X;
        float y = city.getY() * VISUAL_SCALE + OFFSET_Y;
        
        // Draw city circle with glow effect
        sf::CircleShape glow(10.0f);
        glow.setFillColor(sf::Color(33, 150, 243, 100));
        // SFML 3.x FIX: setPosition now requires a single sf::Vector2f argument
        glow.setPosition(sf::Vector2f(x - 10, y - 10));
        window.draw(glow);
        
        sf::CircleShape circle(7.0f);
        circle.setFillColor(sf::Color(33, 150, 243));
        circle.setOutlineThickness(2);
        circle.setOutlineColor(sf::Color::White);
        // SFML 3.x FIX: setPosition now requires a single sf::Vector2f argument
        circle.setPosition(sf::Vector2f(x - 7, y - 7));
        window.draw(circle);
        
        // Draw city name
        sf::Text nameText = createText(city.getName(), 13, sf::Color(20, 20, 20), x + 10, y - 8);
        nameText.setStyle(sf::Text::Bold);
        window.draw(nameText);
    }
}

void SolverWindow::drawControlPanel() {
    // Draw panel background
    sf::RectangleShape panel(sf::Vector2f(PANEL_WIDTH, WINDOW_HEIGHT));
    // SFML 3.x FIX: setPosition now requires a single sf::Vector2f argument
    panel.setPosition(sf::Vector2f(CANVAS_WIDTH, 0));
    panel.setFillColor(sf::Color(240, 240, 240));
    window.draw(panel);
    
    // Draw panel title
    sf::Text panelTitle = createText("Control Panel", 20, sf::Color(50, 50, 50), 820, 50);
    panelTitle.setStyle(sf::Text::Bold);
    window.draw(panelTitle);
    
    // Draw section separator
    sf::RectangleShape separator(sf::Vector2f(320, 2));
    // SFML 3.x FIX: setPosition now requires a single sf::Vector2f argument
    separator.setPosition(sf::Vector2f(800, 85));
    separator.setFillColor(sf::Color(200, 200, 200));
    window.draw(separator);
}

void SolverWindow::drawButton(const sf::RectangleShape& button, const sf::Text& text) {
    window.draw(button);
    window.draw(text);
}

void SolverWindow::drawStatistics() {
    float startY = 360;
    float lineHeight = 70;
    
    // Statistics section title
    sf::Text statsTitle = createText("Algorithm Statistics", 18, sf::Color(50, 50, 50), 820, 350);
    statsTitle.setStyle(sf::Text::Bold);
    window.draw(statsTitle);
    
    // Status
    sf::RectangleShape statusBox(sf::Vector2f(320, 55));
    // SFML 3.x FIX: setPosition now requires a single sf::Vector2f argument
    statusBox.setPosition(sf::Vector2f(800, startY));
    statusBox.setFillColor(sf::Color::White);
    statusBox.setOutlineThickness(2);
    statusBox.setOutlineColor(sf::Color(200, 200, 200));
    window.draw(statusBox);
    
    sf::Text statusLabel = createText("Status:", 12, sf::Color(100, 100, 100), 810, startY + 8);
    window.draw(statusLabel);
    
    std::string statusStr;
    sf::Color statusColor;
    if (solver.getCurrentTemperature() <= 0.1) {
        statusStr = "FINISHED";
        statusColor = sf::Color(76, 175, 80);
    } else if (isRunning && !isPaused) {
        statusStr = "RUNNING";
        statusColor = sf::Color(76, 175, 80);
    } else if (isPaused) {
        statusStr = "PAUSED";
        statusColor = sf::Color(255, 193, 7);
    } else {
        statusStr = "READY";
        statusColor = sf::Color(100, 100, 100);
    }
    sf::Text statusValue = createText(statusStr, 18, statusColor, 810, startY + 28);
    statusValue.setStyle(sf::Text::Bold);
    window.draw(statusValue);
    
    // Best Distance
    startY += lineHeight;
    sf::RectangleShape distBox(sf::Vector2f(320, 55));
    // SFML 3.x FIX: setPosition now requires a single sf::Vector2f argument
    distBox.setPosition(sf::Vector2f(800, startY));
    distBox.setFillColor(sf::Color::White);
    distBox.setOutlineThickness(2);
    distBox.setOutlineColor(sf::Color(200, 200, 200));
    window.draw(distBox);
    
    sf::Text distLabel = createText("Best Distance:", 12, sf::Color(100, 100, 100), 810, startY + 8);
    window.draw(distLabel);
    
    std::ostringstream distStream;
    distStream << std::fixed << std::setprecision(2) << bestTour.getTotalDistance();
    sf::Text distValue = createText(distStream.str(), 20, sf::Color(76, 175, 80), 810, startY + 26);
    distValue.setStyle(sf::Text::Bold);
    window.draw(distValue);
    
    // Temperature
    startY += lineHeight;
    sf::RectangleShape tempBox(sf::Vector2f(320, 55));
    // SFML 3.x FIX: setPosition now requires a single sf::Vector2f argument
    tempBox.setPosition(sf::Vector2f(800, startY));
    tempBox.setFillColor(sf::Color::White);
    tempBox.setOutlineThickness(2);
    tempBox.setOutlineColor(sf::Color(200, 200, 200));
    window.draw(tempBox);
    
    sf::Text tempLabel = createText("Temperature:", 12, sf::Color(100, 100, 100), 810, startY + 8);
    window.draw(tempLabel);
    
    std::ostringstream tempStream;
    tempStream << std::fixed << std::setprecision(2) << solver.getCurrentTemperature() << " °";
    sf::Text tempValue = createText(tempStream.str(), 18, sf::Color(255, 87, 34), 810, startY + 28);
    tempValue.setStyle(sf::Text::Bold);
    window.draw(tempValue);
    
    // Iterations
    startY += lineHeight;
    sf::RectangleShape iterBox(sf::Vector2f(320, 55));
    // SFML 3.x FIX: setPosition now requires a single sf::Vector2f argument
    iterBox.setPosition(sf::Vector2f(800, startY));
    iterBox.setFillColor(sf::Color::White);
    iterBox.setOutlineThickness(2);
    iterBox.setOutlineColor(sf::Color(200, 200, 200));
    window.draw(iterBox);
    
    sf::Text iterLabel = createText("Iterations:", 12, sf::Color(100, 100, 100), 810, startY + 8);
    window.draw(iterLabel);
    
    sf::Text iterValue = createText(std::to_string(iterationCount), 18, sf::Color(33, 150, 243), 810, startY + 28);
    iterValue.setStyle(sf::Text::Bold);
    window.draw(iterValue);
    
    // City Count
    startY += lineHeight;
    sf::RectangleShape cityBox(sf::Vector2f(320, 55));
    // SFML 3.x FIX: setPosition now requires a single sf::Vector2f argument
    cityBox.setPosition(sf::Vector2f(800, startY));
    cityBox.setFillColor(sf::Color::White);
    cityBox.setOutlineThickness(2);
    cityBox.setOutlineColor(sf::Color(200, 200, 200));
    window.draw(cityBox);
    
    sf::Text cityLabel = createText("Cities:", 12, sf::Color(100, 100, 100), 810, startY + 8);
    window.draw(cityLabel);
    
    sf::Text cityValue = createText(std::to_string(cityData.size()), 18, sf::Color(156, 39, 176), 810, startY + 28);
    cityValue.setStyle(sf::Text::Bold);
    window.draw(cityValue);
}

void SolverWindow::draw() {
    window.clear(sf::Color(245, 245, 245));
    
    // Draw canvas and tour
    drawCanvas();
    drawTour(bestTour, sf::Color(76, 175, 80), 3.0f);
    drawCities();
    
    // Draw control panel
    drawControlPanel();
    drawButton(startButton, startButtonText);
    drawButton(pauseButton, pauseButtonText);
    drawButton(resetButton, resetButtonText);
    drawButton(addCityButton, addCityButtonText);
    drawButton(removeCityButton, removeCityButtonText);
    drawStatistics();
    
    window.display();
}

void SolverWindow::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        
        processEvents();
        update(deltaTime);
        draw();
    }
}