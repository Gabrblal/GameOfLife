#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <SFML/Graphics.hpp>

#include "GameOfLife.hpp"

class View
{
public:

    // Constructs and initialises a new game of life view.
    View();
    ~View();

    void Set(int tile_width, int tile_height);

    // Check window state.
    bool WaitEvent(sf::Event &event);

    // Update the texture, view and window, if many tiles have changed state.
    // Changes the colours of the tiles in the texture depending on their new
    // value.
    void Render(std::vector<Tile> updates);

    // Update the texture, view and window, if a specific tile has changed
    // value.
    void Render(int x, int y, bool value);

    // Update the texture, view and window. For changes that depend on current
    // state (e.g. panning).
    void Display();

    // Get a pointer to the rendering window.
    inline sf::RenderWindow &Window() { return *m_window; }

    // Get the pixel width of the view.
    inline int Width() { return m_pixel_width; }

    // Get the pixel height of the view.
    inline int Height() { return m_pixel_height; }

    // States for zooming.
    enum class ZoomAction {
        In, Out
    };

    // States of horisontal and vertical panning.
    enum class MoveAction {
        Stay,
        Forward,
        Backward
    };

    // Zoom the view by a given factor.
    void Zoom(ZoomAction zoom);

    // Pan the view horisontally.
    void PanHorisontal(MoveAction action);

    // Pan the view vertically.
    void PanVertical(MoveAction action);

    // Returns the tile coordates on top of the provided pixel coordinates.
    sf::Vector2i MapPixelToTile(int x, int y) const;

private:
    // The actual window containing the program graphics.
    std::unique_ptr<sf::RenderWindow> m_window;

    // Texture for rendering the simulation. Think of this as a large pixel
    // canvas of some width and height that we can change the colours of.
    sf::RenderTexture m_texture;

    // Mutex for prevent concurrent use of the texture and window.
    // TODO: Apparently the window needs to be active in the current thread to
    // write to the texture, but shouldn't. Work out independent texture and
    // window updates.
    std::mutex m_resource_mutex;

    // The texture is frozen at a certain point captured by this sprite object.
    // After the texture image is captured we can make some transformations and
    // draw it into the world space.
    sf::Sprite m_sprite;

    // The view of the world space. The view defines how the world space is
    // mapped to pixels in the window.
    sf::View m_view;

    // Dimensions of tiling.
    int m_tile_width;
    int m_tile_height;
    std::atomic_int m_pixel_width;
    std::atomic_int m_pixel_height;

    // Controls
    sf::Vector2f m_moving;

    // Dimensions describing each tile, in raw pixels on the texture image.
    static int s_padding;
    static int s_tile_size;

    // Colours for things.
    static sf::Color s_colour_on;
    static sf::Color s_colour_off;
    static sf::Color s_colour_background;
};