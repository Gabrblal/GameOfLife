#include "View.hpp"

#include <algorithm>
#include <cmath>

int View::s_padding = 1;
int View::s_tile_size = 5;

sf::Color View::s_colour_on = sf::Color(0xFFFFFFFF);
sf::Color View::s_colour_off = sf::Color(0x090909FF);
sf::Color View::s_colour_background = sf::Color(0x000000FF);

View::View(int tile_width, int tile_height)
    : m_window()
    , m_texture()
    , m_view()
    , m_tile_width(tile_width)
    , m_tile_height(tile_height)
{
    CreateWindow();
    CreateTexture(tile_width, tile_height);
    CreateView();
}

void View::CreateWindow()
{
    // Determine the largest fullscreen mode.
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
}

void View::CreateTexture(int tile_width, int tile_height)
{
    // Each tile has s_padding on all four sides, plus the tile inside of
    // width and height s_tile_size.
    m_texture.create(
        (2 * s_padding + s_tile_size) * tile_width,
        (2 * s_padding + s_tile_size) * tile_height
    );

    // Drawing a texture flips it along the top of the screen, flip it back.
    m_sprite.setScale(1.0, -1.0);
    m_sprite.setPosition(0.0, m_texture.getSize().y);
}

void View::CreateView()
{
    // Set the dimensions of the view to match the window.
    m_view.setSize(m_window->getSize().x, m_window->getSize().y);

    // Center the view on the middle of the texture.
    sf::Vector2u texture_size = m_texture.getSize();
    m_view.setCenter(texture_size.x / 2, texture_size.y / 2);

    // Zoom in so the width is the same as the texture.
    m_view.zoom((float)texture_size.x / (float)m_window->getSize().x);

    m_window->setView(m_view);
}

void View::Update(std::vector<Tile> tiles)
{
    // Create a square for rendering.
    sf::RectangleShape square;
    square.setSize(sf::Vector2f(s_tile_size, s_tile_size));

    for (Tile tile : tiles) {
        square.setPosition(sf::Vector2f(
            (2 * tile.x * s_padding) + (s_tile_size * tile.x + s_tile_size / 2),
            (2 * tile.y * s_padding) + (s_tile_size * tile.y + s_tile_size / 2)
        ));

        // Change the colour depending on the tile state
        tile.value ? square.setFillColor(s_colour_on)
                   : square.setFillColor(s_colour_off);
        m_texture.draw(square);
    }

    Update();
}

void View::Update(int x, int y, bool value)
{
    // Create a new square and set it at the position (x, y)
    sf::RectangleShape square(sf::Vector2f(s_tile_size, s_tile_size));
    square.setPosition(sf::Vector2f(
        (2 * x * s_padding) + (s_tile_size * x + s_tile_size / 2),
        (2 * y * s_padding) + (s_tile_size * y + s_tile_size / 2)
    ));

    // Change it's colour depending on the tile state.
    value ? square.setFillColor(s_colour_on)
          : square.setFillColor(s_colour_off);

    // Change the square colour on the texture.
    m_texture.draw(square);

    Update();
}

void View::Update()
{
    // Update the view.
    m_view.setCenter(m_view.getCenter() + m_moving);
    m_window->setView(m_view);

    m_window->clear(s_colour_background);

    // Take a snapshot of the texture and put it into the sprite.
    m_sprite.setTexture(m_texture.getTexture());

    // Draw the sprite, that handles transformations of the texture.
    m_window->draw(m_sprite);

    // Display it in the window.
    m_window->display();
}

sf::RenderWindow &View::Window() {
    return *m_window;
}

void View::Zoom(ZoomAction action)
{
    if (action == ZoomAction::In)
        m_view.zoom(1.1);
    else
        m_view.zoom(0.9);

    m_window->setView(m_view);
}

void View::PanHorisontal(MoveAction action)
{
    if (action == MoveAction::Forward)
        m_moving.x = 1;
    else if (action == MoveAction::Backward)
        m_moving.x = -1;
    else
        m_moving.x = 0;
}

void View::PanVertical(MoveAction action)
{
    if (action == MoveAction::Forward)
        m_moving.y = -1;
    else if (action == MoveAction::Backward)
        m_moving.y = 1;
    else
        m_moving.y = 0;
}

sf::Vector2i View::MapPixelToTile(int x, int y) const
{
    sf::Vector2f world_position = m_window->mapPixelToCoords(sf::Vector2i(x, y));
    int tile_x = std::floor(world_position.x / (2 * s_padding + s_tile_size));
    int tile_y = std::floor(world_position.y / (2 * s_padding + s_tile_size));

    return sf::Vector2i(tile_x, tile_y);
}