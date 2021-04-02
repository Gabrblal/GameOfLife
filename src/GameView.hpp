#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "Game.hpp"

class GameView
{
public:

    // Constructs and initialises a new game GameView.
    GameView(int tile_width, int tile_height);

    void CreateWindow();

    void CreateTexture(int tile_width, int tile_height);
    void CreateView();

    // Update the window.
    void Update(std::vector<Square> updates);
    void Update(int x, int y, bool value);
    void Update();

    void Zoom(float factor);
    void Horisontal(float value);
    void Vertical(float value);

    // Check window state.
    sf::RenderWindow &Window();

    sf::Vector2i MapPixelToTile(int x, int y);

private:

    // Grid displaying the game state.
    std::unique_ptr<sf::RenderWindow> m_window;
    sf::RenderTexture m_texture;
    sf::View m_view;
    sf::Sprite m_sprite;

    // Dimensions of tiling.
    int m_tile_width;
    int m_tile_height;

    // Controls
    sf::Vector2f m_moving;

    static float s_padding;
    static float s_tile_size;
    static sf::Color s_colour_on;
    static sf::Color s_colour_off;
    static sf::Color s_colour_background;
};