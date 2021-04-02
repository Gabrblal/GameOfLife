#include "GameView.hpp"

#include <cmath>
#include <iostream>

// Dimensions in raw pixels before zoom. Any value below 1.0 will cause
// things to merge together.
float GameView::s_padding = 1.0;
float GameView::s_tile_size = 5.0;

// Various colours.
sf::Color GameView::s_colour_on = sf::Color(0xFFFFFFFF);
sf::Color GameView::s_colour_off = sf::Color(0x090909FF);
sf::Color GameView::s_colour_background = sf::Color(0x000000FF);

GameView::GameView(int tile_width, int tile_height)
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

void GameView::CreateWindow()
{
    std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
    auto mode = std::max_element(
        modes.begin(),
        modes.end(),
        [](sf::VideoMode &a, sf::VideoMode &b) {
            return (a.width * a.height) < (b.width * b.height);
        }
    );

    m_window = std::unique_ptr<sf::RenderWindow>(
        new sf::RenderWindow(
            sf::VideoMode(mode->width, mode->height),
            "Game of Life",
            // sf::Style::Default
            sf::Style::Fullscreen
        )
    );
}

void GameView::CreateTexture(int tile_width, int tile_height)
{
    // Each tile has s_padding on all four sides, plus the tile inside of
    // width and height s_tile_size.
    m_texture.create(
        (2 * s_padding + s_tile_size) * tile_width,
        (2 * s_padding + s_tile_size) * tile_height
    );

    // Drawing a texture flips it, flip it back.
    m_sprite.setScale(1.0, -1.0);
    m_sprite.setPosition(0.0, m_texture.getSize().y);
}

void GameView::CreateView()
{
    // Find the shortest dimension of the texture.
    m_view.setSize(m_window->getSize().x, m_window->getSize().y);

    sf::Vector2u texture = m_texture.getSize();
    m_view.setCenter(texture.x / 2, texture.y / 2);

    m_view.zoom((float)texture.x / (float)m_window->getSize().x);
    // m_view.zoom(0.5);
    m_window->setView(m_view);
}

void GameView::Update(std::vector<Square> squares)
{
    using namespace sf;

    RectangleShape tile;
    tile.setSize(Vector2f(s_tile_size, s_tile_size));

    for (Square square : squares) {
        tile.setPosition(Vector2f(
            (2 * square.x * s_padding) + (s_tile_size * square.x + s_tile_size / 2),
            (2 * square.y * s_padding) + (s_tile_size * square.y + s_tile_size / 2)
        ));
        square.value ? tile.setFillColor(s_colour_on)
                     : tile.setFillColor(s_colour_off);
        m_texture.draw(tile);
    }

    Update();
}

void GameView::Update(int x, int y, bool value)
{
    sf::RectangleShape tile(sf::Vector2f(s_tile_size, s_tile_size));
    tile.setPosition(sf::Vector2f(
        (2 * x * s_padding) + (s_tile_size * x + s_tile_size / 2),
        (2 * y * s_padding) + (s_tile_size * y + s_tile_size / 2)
    ));

    value ? tile.setFillColor(s_colour_on)
          : tile.setFillColor(s_colour_off);

    m_texture.draw(tile);

    Update();
}

void GameView::Update()
{
    m_view.setCenter(m_view.getCenter() + m_moving);
    m_window->setView(m_view);
    m_window->clear(s_colour_background);
    m_sprite.setTexture(m_texture.getTexture());
    m_window->draw(m_sprite);
    m_window->display();
}

sf::RenderWindow &GameView::Window() {
    return *m_window;
}

void GameView::Zoom(float factor)
{
    m_view.zoom(factor);
    m_window->setView(m_view);
}

void GameView::Horisontal(float value) {
    m_moving.x = value;
}

void GameView::Vertical(float value) {
    m_moving.y = value;
}

sf::Vector2i GameView::MapPixelToTile(int x, int y)
{
    sf::Vector2f world_position = m_window->mapPixelToCoords(sf::Vector2i(x, y));
    int tile_x = std::floor(world_position.x / (2 * s_padding + s_tile_size));
    int tile_y = std::floor(world_position.y / (2 * s_padding + s_tile_size));

    std::cout << "Pixel(" << x << ", " << y << ") -> World("
              << world_position.x << ", "
              << world_position.y
              << ") -> "
              << "Grid("
              << tile_x << ", "
              << tile_y
              << ")" << std::endl;

    return sf::Vector2i(tile_x, tile_y);
}