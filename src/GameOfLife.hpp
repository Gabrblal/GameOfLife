#pragma once

#include <mutex>
#include <vector>

/**
 * @brief Data of a tile.
 */
struct Tile {
    int x;
    int y;
    bool value;
};

/**
 * @brief Class implementing Conways game of life.
 * 
 * Each tile in the game space is either dead or alive, and is updated on each
 * iteration depending on the states of the surrounding tiles.
 * - A tile stays alive 2 or 3 immediate tiles are alive.
 * - A tile becomes alive if exactly 3 immediate tiles are alive.
 * - A tile dies otherwise.
 */
class GameOfLife
{
public:

    /**
     * @brief Instantiate a game of life with a grid of provided width and 
     * height.
     * 
     * @param width The width of the grid in tiles.
     * @param height The height of the grid in tiles.
     */
    GameOfLife(int width, int height);

    /**
     * @brief Add a glider to the centre of the game space.
     */
    void add_glider();

    /**
     * @brief Advances the game of life state by one step, returning all the
     * files that changed state.
     * 
     * @return The tiles that changed state that step.
     */
    std::vector<Tile> advance();

    /**
     * @brief Gets the state of all tiles.
     * 
     * @return The state of all tiles.
     */
    std::vector<Tile> space();

    /**
     * @brief Updates the value of a tile at position (x, y) to alive or not. If
     * the position is out of bounds then does nothing.
     * 
     * @param x The x position in the grid.
     * @param y The y position in the grid.
     * @param alive If that position is alive.
     */
    void update(int x, int y, bool alive);

    /**
     * @brief Sets a tile to alive at (x, y). If the position is out of bounds
     * then does nothing.
     * 
     * @param x The x position in the grid.
     * @param y The y position in the grid.
     */
    inline void place(int x, int y) {
        update(x, y, true);
    }

    /**
     * @brief Clears a tile to dead at (x, y). If the position is out of bounds
     * then does nothing.
     * 
     * @param x The x position in the grid.
     * @param y The y position in the grid.
     */
    inline void remove(int x, int y) {
        update(x, y, false);
    }

    /**
     * @brief Sets all tiles to dead.
     */
    void clear();

    /**
     * @brief Get the width of the simulation space.
     * @return The width of the simulation space.
     */
    inline int width() {
        return m_width;
    }

    /**
     * @brief Get the height of the simulation space.
     * @return The height of the simulation space.
     */
    inline int height() {
        return m_height;
    }

private:

    /// Vector of width * height booleans with the state of each square.
    std::vector<bool> m_space;

    /// Mutex protecting concurrent access to the game space.
    std::mutex m_space_mutex;

    /// The width of the game space.
    int m_width;

    /// The height of the game space.
    int m_height;
};
