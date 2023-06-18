#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <thread>

#include "GameOfLife.hpp"
#include "View.hpp"

/**
 * @brief The controller class owns the thread incrementing the game of life
 * model, and the thread responsible for rendering the model. The controller
 * handles user input and updates the model or view accordingly, or exits the
 * application.
 */
class Controller
{
public:

    /**
     * @brief Create the controller, the main singleton class of the
     * application.
     * 
     * Starts the model and view threads, sets the model to the initial state,
     * and displays the entire model space.
     */
    Controller();

    /**
     * @brief The user input loop fetching events and performing actions
     * depending on their values.
     */
    void main();

private:

    /**
     * @brief Joinable thread responsible for updating the simulation.
     * 
     * @param stop The stop signal issued to the thread to exit.
     */
    void simulation_thread(std::stop_token stop);

    /**
     * @brief Joinable thread for updating the view of the simulation.
     * 
     * @param stop The stop signal issued to the thread to exit.
     */
    void view_thread(std::stop_token stop);

    /**
     * @brief Issues a stop request to the model and view threads.
     */
    void exit();

    /**
     * @brief Get the number of model iterations in the past second.
     * @return The number of model iterations.
     */
    std::size_t performance();

    /// Event handling.
    void handle_key_press(sf::Event &event);
    void handle_key_release(sf::Event &event);
    void handle_movement();
    void handle_resize(sf::Event &event);
    void handle_mouse_press(sf::Event &event);
    void handle_mouse_scroll(sf::Event &event);

    // Increase or decrease the speed of the simulation.
    void handle_speed(bool increase);

    /// View of the simulation.
    View m_view;

    // The game of life simulation that increments continuously.
    GameOfLife m_model;

    /// Thread updating the model.
    std::jthread m_model_thread;

    /// Thread updating the view of the model.
    std::jthread m_view_thread;

    /// Condition variable for notifying the model to perform an update.
    std::condition_variable_any m_model_condition;

    /// Condition variable for notifying the view to render.
    std::condition_variable_any m_view_condition;

    /// Mutex protecting the model condition variable.
    std::mutex m_model_condition_mutex;

    /// Mutex protecting the view condition variable.
    std::mutex m_view_condition_mutex;

    /// The amount of time between model updates.
    std::chrono::microseconds m_model_delta;

    /// Minimum increment between model increments.
    std::chrono::microseconds m_model_delta_minimum;

    /// Maximum increment between model increments.
    std::chrono::microseconds m_model_delta_maximum;

    /// The amount of time between screen updates.
    std::chrono::microseconds m_view_delta;

    /// Stop signal to stop and join the model and view threads.
    std::stop_source m_stop;

    /// If the game is currently paused.
    std::atomic_bool m_paused;

    /// The model iterations used to track performance.
    std::atomic_size_t m_iterations;

    /// The average number of model iterations.
    double m_performance;

    /// If left key is currently pressed.
    bool m_left;

    /// If the right key is currently pressed.
    bool m_right;

    /// If the up key is currently pressed.
    bool m_up;

    /// If the down key is currently pressed.
    bool m_down;
};
