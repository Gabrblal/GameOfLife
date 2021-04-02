#pragma once

#include <SFML/Graphics.hpp>

#include "GameOfLife.hpp"
#include "View.hpp"

class Controller
{
public:

    // Create a new controller, passing through the width and height of the
    // tile grid.
    Controller(int tile_width, int tile_height);

    // Start the simulation loop.
    void Loop();

private:

    // Event handling.
    void HandleEvents();
    void HandleKeyPress(sf::Event &event);
    void HandleKeyRelease(sf::Event &event);
    void HandleMovement();
    void HandleResize(sf::Event &event);
    void HandleMousePress(sf::Event &event);
    void HandleMouseScroll(sf::Event &event);

    // View of the simulation.
    View m_view;

    // The simulation itself.
    GameOfLife m_model;

    // If the game is currently paused.
    bool m_paused;

    // The current state of the movement keys.
    bool m_left;
    bool m_right;
    bool m_up;
    bool m_down;
};