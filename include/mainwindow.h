#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTableWidget>
#include <QTimer>
#include "tsp_solver.h"
#include "visualization_widget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onStartClicked();
    void onPauseClicked();
    void onResetClicked();
    void onAddCityClicked();
    void onRemoveCityClicked();
    void onAlgorithmStep();
    void onSpeedChanged(int value);
    void onParameterChanged();

private:
    void setupUI();
    void updateUI();
    void updateCityTable();
    
    // UI Components
    QPushButton* startButton;
    QPushButton* pauseButton;
    QPushButton* resetButton;
    QPushButton* addCityButton;
    QPushButton* removeCityButton;
    
    QSpinBox* cityCountSpinBox;
    QTableWidget* cityTable;
    
    QDoubleSpinBox* initialTempSpinBox;
    QDoubleSpinBox* coolingRateSpinBox;
    QDoubleSpinBox* minTempSpinBox;
    QSpinBox* maxIterationsSpinBox;
    
    QSlider* speedSlider;
    QLabel* distanceLabel;
    QLabel* iterationLabel;
    QLabel* temperatureLabel;
    QLabel* statusLabel;
    
    // Core components
    TSPSolver solver;
    VisualizationWidget* visualizationWidget;
    
    // Timer for algorithm execution
    QTimer* algorithmTimer;
    int timerInterval; // milliseconds
};

#endif // MAINWINDOW_H