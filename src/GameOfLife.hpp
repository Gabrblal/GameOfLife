#pragma once

#include <mutex>
#include <unordered_set>
#include <vector>

/// Type describing a tile.
struct Tile {
    int x;
    int y;
};

// Hash function for Tile.
template<>
struct std::hash<Tile>
{
    size_t operator()(const Tile& tile) const noexcept
    {
        size_t hx = hash<int>{}(tile.x);
        size_t hy = hash<int>{}(tile.y);
        return hx ^ (hy + 0x9e3779b9 + (hx << 6) + (hx >> 2));
    }
};

inline bool operator==(const Tile &left, const Tile &right) {
    return left.x == right.x && left.y == right.y;
}

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

    /// Type describing all alive tiles.
    using Space = std::unordered_set<Tile>;

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
     * @brief Advances the game of life state by one step.
     */
    void advance();

    /**
     * @brief Gets the alive tiles
     */
    std::unordered_set<Tile> space();

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

    /**
     * @brief Returns all the neighboring tiles of a tile.
     * 
     * @param tile The tile to find the neighbors of.
     * @return The neighboring tiles.
     */
    std::array<Tile, 8> neighbors(Tile tile);

    /**
     * @brief Returns the number of alive tiles around one.
     * 
     * @param tile The tile to count the number of alive tiles around.
     * @return The number of alive tiles surrounding the tile.
     */
    int count(Tile tile);

    /// Vector of width * height booleans with the state of each square.
    Space m_space;

    /// Mutex protecting concurrent access to the game space.
    std::mutex m_mutex;

    /// The width of the game space.
    int m_width;

    /// The height of the game space.
    int m_height;
};
