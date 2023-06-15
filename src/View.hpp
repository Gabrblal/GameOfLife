#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <SFML/Graphics.hpp>

#include "GameOfLife.hpp"

/**
 * @brief The view is responsible for rendering, updating the screen and getting
 * user input from the window.
 */
class View
{
public:

    /**
     * @brief Create a new window and view of GameOfLife.
     * 
     * Requires set(width, height) to be called to create the initial view.
     */
    View();

    /**
     * @brief Closes the window.
     */
    ~View();

    /**
     * @brief Set the game of life space width and height. Rendering will assume
     * these dimensions when updating tile states.
     * 
     * @param tile_width The width of the game of life space.
     * @param tile_height The height of the game of life space.
     */
    void set(int tile_width, int tile_height);

    /**
     * @brief Renders all given tiles.
     * @param updates The set of tiles to update.
     */
    void render(std::vector<Tile> updates);

    /**
     * @brief Updates a tile at position (x, y).
     * 
     * @param x The x coordinate of the tile.
     * @param y The y coordinate of the tile.
     * @param alive If the tile is alive.
     */
    void render(int x, int y, bool alive);

    /**
     * @brief Updates the display to the screen, transforming everything
     * rendered to the current texture depending on view position and updating
     * the screen.
     */
    void display();

    /**
     * @brief Get a reference to the rendering window.
     * @return A reference to the rendering window.
     */
    inline sf::RenderWindow &window() {
        return *m_window;
    }

    /**
     * @brief Get the pixel width of the window.
     * @return The pixel width of the window.
     */
    inline int width() {
        return m_pixel_width;
    }

    /**
     * @brief Get the pixel height of the window.
     * @return The pixel height of the window.
     */
    inline int height() {
        return m_pixel_height;
    }

    /// States for zooming.
    enum class ZoomAction {
        IN,
        OUT
    };

    /// States of horisontal and vertical panning.
    enum class MoveAction {
        STAY,
        FORWARD,
        BACKWARD
    };

    /**
     * @brief Zoom the view in or out.
     * @param zoom The zoom action to perform.
     */
    void zoom(ZoomAction zoom);

    /**
     * @brief Set the panning state horisontally.
     * @param action Whether to say, increase or decrease the horisontal pan.
     */
    void pan_horisontal(MoveAction action);

    /**
     * @brief Set the panning state vertically.
     * @param action Whether to say, increase or decrease the vertically pan.
     */
    void pan_vertical(MoveAction action);

    /**
     * @brief Get the game space tile coordinates from pixel coordinates.
     * 
     * @param x The pixel x position.
     * @param y The pixel y position.
     * 
     * @return The (x, y) game space tile mapped to the pixel.
     */
    sf::Vector2i map_pixel_to_tile(int x, int y) const;

private:

    /// The window containing the program graphics, and where input is
    /// retrieved.
    std::unique_ptr<sf::RenderWindow> m_window;

    /// Texture for drawing onto. A large pixel canvas of some width and height
    /// that can be updated.
    sf::RenderTexture m_texture;

    /// Mutex for prevent concurrent use of the texture and window.
    std::mutex m_mutex;

    /// The texture is frozen at a certain point captured by this sprite object.
    /// After the texture image is captured we can make some transformations and
    /// draw it into the world space.
    sf::Sprite m_sprite;

    // The view of the world space. The view defines how the world space is
    // mapped to pixels in the window.
    sf::View m_view;

    /// The tile width of the game space.
    int m_tile_width;

    /// The tile height of the game space.
    int m_tile_height;

    /// The pixel width of the window.
    std::atomic_int m_pixel_width;

    /// The pixel height of the window.
    std::atomic_int m_pixel_height;

    /// Each element is either -1 for decreasing, 0 for maintaining, or 1 for
    /// increasing the position of the view, pseudo-velocity.
    sf::Vector2f m_moving;

    /// Pixels surrounding each tile that are not part of the tile.
    static int s_padding;

    /// The radius of each displayed tile in pixels.
    static int s_tile_size;

    // Tile colour when tile is alive.
    static sf::Color s_colour_alive;

    /// Tile colour when tile is dead.
    static sf::Color s_colour_dead;

    /// Background colour.
    static sf::Color s_colour_background;
};
