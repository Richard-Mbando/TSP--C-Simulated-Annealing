#include "SolverWindow.h"
#include <sstream>

SolverWindow::SolverWindow() 
    : window(sf::VideoMode(900, 600), "TSP Simulated Annealing Solver"),
    // Initialize solver with basic parameters
    solver(10000.0, 0.995, 100) 
{
    window.setFramerateLimit(60); 
    initializeCities();
    resetSimulation();
    
    // Load a font for UI text
   if(!font.loadFromFile("arial.ttf")) {
    std::cerr << "Error loading font." << std::endl; // FIX: Use std::endl, not std::exit(1) inside the stream
    std::exit(1); // Call exit separately
}
}

void SolverWindow::initializeCities() {
    // Cities matching your 'testLargerProblem' for better visualization
    cityData.push_back(City("City_A", 60, 200));
    cityData.push_back(City("City_B", 180, 200));
    cityData.push_back(City("City_C", 80, 180));
    cityData.push_back(City("City_D", 140, 180));
    cityData.push_back(City("City_E", 20, 160));
    cityData.push_back(City("City_F", 100, 160));
    cityData.push_back(City("City_G", 200, 160));
    cityData.push_back(City("City_H", 140, 140));
    cityData.push_back(City("City_I", 40, 120));
    cityData.push_back(City("City_J", 100, 120));
}

void SolverWindow::resetSimulation() {
    // Reset SA state
    solver.reset(10000.0, 0.995, 100); 
    
    // Initialize Tours
    currentTour = Tour(cityData);
    currentTour.generateRandomTour();
    bestTour = currentTour.createCopy();
    
    isRunning = false;
}

void SolverWindow::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) {
                isRunning = !isRunning; // Start/Pause with SPACE
            } else if (event.key.code == sf::Keyboard::R) {
                resetSimulation(); // Reset with R
            }
        }
    }
}

void SolverWindow::update(float deltaTime) {
    // Run the simulation only if the temperature is high enough and the simulation is active
    if (isRunning && solver.getCurrentTemperature() > 0.1) {
        
        // Run a batch of iterations to speed up convergence
        const int ITERS_PER_FRAME = 10; 
        for (int i = 0; i < ITERS_PER_FRAME; ++i) {
            
            // 1. Run one SA iteration, mutating currentTour
            solver.runOneIteration(currentTour);

            // 2. Check and update the overall best tour
            if (currentTour.getTotalDistance() < bestTour.getTotalDistance()) {
                bestTour = currentTour.createCopy();
            }
        }
        
        // 3. Cool the system (This happens once every 'iterationsPerTemp' iterations)
        solver.coolTemperature();
        
    } else if (solver.getCurrentTemperature() <= 0.1) {
        isRunning = false; // Stop when optimization is complete
    }
}

void SolverWindow::drawTour(const Tour& tour, const sf::Color& color, float thickness) {
    const auto& path = tour.getTour();
    if (path.size() < 2) return;
    
    // Define a 2D viewport for the cities (e.g., coordinates 0-300 in the top-left area)
    const float VISUAL_SCALE = 2.0f;
    const float OFFSET_X = 50.0f;
    const float OFFSET_Y = 50.0f;

    // Draw lines
    for (size_t i = 0; i < path.size(); ++i) {
        const City& current = path[i];
        const City& next = path[(i + 1) % path.size()];

        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(current.getX() * VISUAL_SCALE + OFFSET_X, current.getY() * VISUAL_SCALE + OFFSET_Y), color),
            sf::Vertex(sf::Vector2f(next.getX() * VISUAL_SCALE + OFFSET_X, next.getY() * VISUAL_SCALE + OFFSET_Y), color)
        };
        // Use a rectangle shape for thickness
        sf::Vector2f start(line[0].position);
        sf::Vector2f end(line[1].position);

        sf::Vector2f direction = end - start;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159f;

        sf::RectangleShape lineShape(sf::Vector2f(length, thickness));
        lineShape.setFillColor(color);
        lineShape.setPosition(start);
        lineShape.setRotation(angle);
        
        window.draw(lineShape);
    }
    
    // Draw cities as circles
    for (const auto& city : path) {
        sf::CircleShape circle(5.0f);
        circle.setFillColor(sf::Color::Blue);
        
        float x_pos = city.getX() * VISUAL_SCALE + OFFSET_X;
        float y_pos = city.getY() * VISUAL_SCALE + OFFSET_Y;
        
        // Position is top-left corner of the circle bounds
        circle.setPosition(x_pos - 5, y_pos - 5); 
        window.draw(circle);
        
        // Draw city name
        sf::Text nameText;
        nameText.setFont(font);
        nameText.setString(city.getName());
        nameText.setCharacterSize(12);
        nameText.setFillColor(sf::Color::Black);
        nameText.setPosition(x_pos + 5, y_pos - 10);
        window.draw(nameText);
    }
}

void SolverWindow::drawControls(const sf::Text& statusText, const sf::Text& distanceText, const sf::Text& tempText) {
    // Draw all text elements to the right of the visualization area
    window.draw(statusText);
    window.draw(distanceText);
    window.draw(tempText);
    
    // Draw instructions
    sf::Text instructions;
    instructions.setFont(font);
    instructions.setCharacterSize(14);
    instructions.setFillColor(sf::Color::Black);
    instructions.setString("Controls:\nSPACE: Start/Pause\nR: Reset Simulation");
    instructions.setPosition(650, 400);
    window.draw(instructions);
}

void SolverWindow::draw() {
    window.clear(sf::Color(240, 240, 240)); // Light grey background

    // 1. Draw the current best tour (Red lines)
    drawTour(bestTour, sf::Color::Red, 2.0f);
    
    // 2. Prepare UI Text
    std::stringstream ss;
    
    sf::Text statusText;
    statusText.setFont(font);
    statusText.setCharacterSize(18);
    statusText.setFillColor(sf::Color::Black);
    statusText.setPosition(650, 50);
    
    if (solver.getCurrentTemperature() <= 0.1) {
        statusText.setString("Status: FINISHED");
    } else {
        statusText.setString(isRunning ? "Status: RUNNING (SPACE to PAUSE)" : "Status: PAUSED (SPACE to START)");
    }

    sf::Text distanceText = statusText;
    ss.precision(2);
    ss << std::fixed << "Best Distance: " << bestTour.getTotalDistance();
    distanceText.setString(ss.str());
    distanceText.setPosition(650, 100);
    
    ss.str(""); 
    ss.clear();
    
    sf::Text tempText = statusText;
    ss.precision(4);
    ss << "Temp: " << solver.getCurrentTemperature();
    tempText.setString(ss.str());
    tempText.setPosition(650, 150);

    // 3. Draw UI
    drawControls(statusText, distanceText, tempText);

    window.display();
}

void SolverWindow::run() {
    while (window.isOpen()) {
        processEvents();
        
        // Time management is handled implicitly by the 60 FPS limit
        // We only pass 0 as deltaTime since we run fixed number of steps per frame
        update(0); 
        
        draw();
    }
}