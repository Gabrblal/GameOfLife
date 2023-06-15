#include "View.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

int View::s_padding = 1;
int View::s_tile_size = 5;

sf::Color View::s_colour_alive = sf::Color(0xFFFFFFFF);
sf::Color View::s_colour_dead = sf::Color(0x090909FF);
sf::Color View::s_colour_background = sf::Color(0x000000FF);

/**
 * @brief A thread must set the window to active in order to render and display.
 * This class locks a window with a given mutex in RAII.
 */
class WindowLock
{
public:

    /**
     * @brief Lock a window and set this thread to the active OpenGL target.
     * 
     * @param window Reference to the window.
     * @param mutex Reference to the mutex protecting the window.
     */
    WindowLock(sf::RenderWindow &window, std::mutex &mutex)
        : m_window(&window)
        , m_mutex(&mutex)
    {
        m_mutex->lock();
        m_window->setActive(true);
    }

    /**
     * @brief Unset the active thread and unlock the mutex protecting the
     * window.
     */
    ~WindowLock() {
        m_window->setActive(false);
        m_mutex->unlock();
    }

private:

    // Pointer to the window being locked.
    sf::RenderWindow *m_window;

    // Pointer to the mutex protecting the window.
    std::mutex *m_mutex;
};

View::View()
    : m_window()
    , m_texture()
    , m_view()
    , m_tile_width(0)
    , m_tile_height(0)
    , m_pixel_width(0)
    , m_pixel_height(0)
    , m_moving(0, 0)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Get the largest fullscreen mode.
    std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
    auto mode = std::max_element(
        modes.begin(),
        modes.end(),
        [](sf::VideoMode &a, sf::VideoMode &b) {
            return (a.width * a.height) < (b.width * b.height);
        }
    );

    // Create a new fullscreen window with the largest screen mode.
    m_window = std::unique_ptr<sf::RenderWindow>(
        new sf::RenderWindow(
            sf::VideoMode(mode->width, mode->height),
            "Game of Life",
            sf::Style::Fullscreen
        )
    );

    m_pixel_width = mode->width;
    m_pixel_height = mode->height;

    m_window->setActive(false);
}

View::~View()
{
    WindowLock window_lock(*m_window, m_mutex);
    m_window->close();
}

void View::set(int tile_width, int tile_height)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Each tile has s_padding on all four sides, plus the tile inside of
    // width and height s_tile_size.
    m_texture.create(
        (2 * s_padding + s_tile_size) * tile_width + 3 * s_padding,
        (2 * s_padding + s_tile_size) * tile_height + 3 * s_padding
    );

    // Drawing a texture flips it along the top of the screen, flip it back.
    m_sprite.setScale(1.0, 1.0);

    // Set the dimensions of the view to match the window.
    m_view.setSize(m_window->getSize().x, m_window->getSize().y);

    // Center the view on the middle of the texture.
    sf::Vector2u texture_size = m_texture.getSize();
    m_view.setCenter(texture_size.x / 2, texture_size.y / 2);

    // Zoom in so the width is the same as the texture.
    m_view.zoom((float)texture_size.x / (float)m_window->getSize().x);

    m_window->setView(m_view);
}

void View::render(std::vector<Tile> tiles)
{
    WindowLock lock(*m_window, m_mutex);

    // Create a square for rendering.
    sf::RectangleShape square;
    square.setSize(sf::Vector2f(s_tile_size, s_tile_size));

    for (Tile tile : tiles) {
        square.setPosition(sf::Vector2f(
            (2 * tile.x * s_padding) + (s_tile_size * tile.x + s_tile_size / 2),
            (2 * tile.y * s_padding) + (s_tile_size * tile.y + s_tile_size / 2)
        ));

        // Change the colour depending on the tile state
        tile.value ? square.setFillColor(s_colour_alive)
                   : square.setFillColor(s_colour_dead);
        m_texture.draw(square);
    }

    m_texture.display();
}

void View::render(int x, int y, bool value)
{
    WindowLock lock(*m_window, m_mutex);

    // Create a new square and set it at the position (x, y)
    sf::RectangleShape square(sf::Vector2f(s_tile_size, s_tile_size));
    square.setPosition(sf::Vector2f(
        (2 * x * s_padding) + (s_tile_size * x + s_tile_size / 2),
        (2 * y * s_padding) + (s_tile_size * y + s_tile_size / 2)
    ));

    // Change it's colour depending on the tile state.
    value ? square.setFillColor(s_colour_alive)
          : square.setFillColor(s_colour_dead);

    // Change the square colour on the texture.
    m_texture.draw(square);

    m_texture.display();
}

void View::display()
{
    WindowLock window_lock(*m_window, m_mutex);

    // Update the view.
    m_view.setCenter(m_view.getCenter() + m_moving);
    m_window->setView(m_view);

    m_window->clear(s_colour_background);

    // Draw the sprite, that handles transformations of the texture.
    m_sprite.setTexture(m_texture.getTexture());

    // Display it in the window.
    m_window->draw(m_sprite);
    m_window->display();
}

void View::zoom(ZoomAction action)
{
    if (action == ZoomAction::IN)
        m_view.zoom(1.1);
    else
        m_view.zoom(0.9);

    WindowLock window_lock(*m_window, m_mutex);
    m_window->setView(m_view);
}

void View::pan_horisontal(MoveAction action)
{
    if (action == MoveAction::FORWARD)
        m_moving.x = 1;
    else if (action == MoveAction::BACKWARD)
        m_moving.x = -1;
    else
        m_moving.x = 0;
}

void View::pan_vertical(MoveAction action)
{
    if (action == MoveAction::FORWARD)
        m_moving.y = -1;
    else if (action == MoveAction::BACKWARD)
        m_moving.y = 1;
    else
        m_moving.y = 0;
}

sf::Vector2i View::map_pixel_to_tile(int x, int y) const
{
    sf::Vector2f world_position = m_window->mapPixelToCoords(sf::Vector2i(x, y));
    int tile_x = std::floor(world_position.x / (2 * s_padding + s_tile_size));
    int tile_y = std::floor(world_position.y / (2 * s_padding + s_tile_size));

    return sf::Vector2i(tile_x, tile_y);
}
