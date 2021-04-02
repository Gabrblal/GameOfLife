#pragma once

#include <SFML/Graphics.hpp>

#include "Game.hpp"
#include "GameView.hpp"

class Controller
{
public:

    // Create a new controller and game.
    Controller(int tile_width, int tile_height);

    // Start the main game loop.
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

    std::unique_ptr<sf::RenderWindow> m_window;
    GameView m_view;
    Game m_model;

    bool m_paused;
    bool m_left;
    bool m_right;
    bool m_up;
    bool m_down;
};