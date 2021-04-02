#include "GameController.hpp"

#include <algorithm>
#include <vector>
#include <iostream>

Controller::Controller(int tile_width, int tile_height)
    : m_view(tile_width, tile_height)
    , m_model(tile_width, tile_height)
    , m_paused()
    , m_left(false)
    , m_right(false)
    , m_up(false)
    , m_down(false)
{
    m_model.InitialState();
    m_view.Update(m_model.Space());
}

void Controller::Loop()
{
    while (m_view.Window().isOpen())
    {
        HandleEvents();

        if (!m_paused) {
            m_view.Update(m_model.Advance());
        }
        else {
            m_view.Update();
        }
    }
}

void Controller::HandleEvents()
{
    sf::Event event;
    while (m_view.Window().pollEvent(event))
    {
        switch(event.type)
        {
            case sf::Event::Closed             : m_view.Window().close();  break;
            case sf::Event::KeyPressed         : HandleKeyPress(event);    break;
            case sf::Event::KeyReleased        : HandleKeyRelease(event);  break;
            case sf::Event::Resized            : HandleResize(event);      break;
            case sf::Event::MouseButtonPressed : HandleMousePress(event);  break;
            case sf::Event::MouseWheelScrolled : HandleMouseScroll(event); break;
            default: break;
        }
    }
}

void Controller::HandleKeyPress(sf::Event &event)
{
    switch(event.key.code)
    {
        case sf::Keyboard::Escape : m_view.Window().close(); break;
        case sf::Keyboard::Space  : m_paused = !m_paused;    break;
        case sf::Keyboard::W : m_up    = true; HandleMovement(); break;
        case sf::Keyboard::A : m_left  = true; HandleMovement(); break;
        case sf::Keyboard::S : m_down  = true; HandleMovement(); break;
        case sf::Keyboard::D : m_right = true; HandleMovement(); break;
        default: break;
    }
}

void Controller::HandleKeyRelease(sf::Event &event)
{
    switch(event.key.code)
    {
        case sf::Keyboard::W : m_up    = false; HandleMovement(); break;
        case sf::Keyboard::A : m_left  = false; HandleMovement(); break;
        case sf::Keyboard::S : m_down  = false; HandleMovement(); break;
        case sf::Keyboard::D : m_right = false; HandleMovement(); break;
        default: break;
    }
}

void Controller::HandleMovement()
{
    if (m_left == m_right)
        m_view.Horisontal(0.0);
    else if (m_left)
        m_view.Horisontal(-1.0);
    else
        m_view.Horisontal(1.0);

    if (m_up == m_down)
        m_view.Vertical(0.0);
    else if (m_up)
        m_view.Vertical(-1.0);
    else
        m_view.Vertical(1.0);
}

void Controller::HandleResize(sf::Event &event)
{
    return;
}

void Controller::HandleMousePress(sf::Event &event)
{
    sf::Vector2i tile = m_view.MapPixelToTile(
        event.mouseButton.x,
        event.mouseButton.y
    );

    if (event.mouseButton.button == sf::Mouse::Left) {
        m_model.Place(tile.x, tile.y);
        m_view.Update(tile.x, tile.y, true);
    }
    else {
        m_model.Remove(tile.x, tile.y);
        m_view.Update(tile.x, tile.y, false);
    }

}

void Controller::HandleMouseScroll(sf::Event &event)
{
    if (event.mouseWheelScroll.delta < 0) {
        m_view.Zoom(1.1);
    }
    else {
        m_view.Zoom(0.9);
    }
}