#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <thread>

#include "GameOfLife.hpp"
#include "View.hpp"

class Controller
{
public:

    // Create a new controller, passing through the width and height of the
    // tile grid.
    Controller();
    ~Controller();

    // Start the simulation loop.
    void Loop();

private:

    void SimulationThread(std::atomic_bool &exit);
    void ViewThread(std::atomic_bool &exit);

    void Exit();

    // Event handling.
    void HandleKeyPress(sf::Event &event);
    void HandleKeyRelease(sf::Event &event);
    void HandleMovement();
    void HandleResize(sf::Event &event);
    void HandleMousePress(sf::Event &event);
    void HandleMouseScroll(sf::Event &event);

    // Increase or decrease the speed of the simulation.
    void HandleSpeed(bool increase);

    View m_view; // View of the simulation.
    GameOfLife m_model; // The simulation itself.

    // Threads for running the simulation and updating the window, and for
    // controlling each update rate.
    std::thread m_model_thread;
    std::thread m_view_thread;
    std::condition_variable m_model_cv;
    std::condition_variable m_view_cv;
    std::mutex m_model_cv_mutex;
    std::mutex m_view_cv_mutex;

    // The amount of time between model updates.
    std::atomic_int m_model_delta;
    int m_model_delta_minimum;
    int m_model_delta_maximum;

    // The amount of time between screen updates.
    std::atomic_int m_view_delta;

    // Exit signal.
    std::atomic_bool m_exit;

    // If the game is currently paused.
    std::atomic_bool m_paused;

    // The current state of the movement keys.
    bool m_left;
    bool m_right;
    bool m_up;
    bool m_down;
};