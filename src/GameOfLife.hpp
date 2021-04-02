#pragma once

#include <vector>

// Simple POD for tile data.
struct Tile {
    Tile(int x, int y, bool value)
        : x(x)
        , y(y)
        , value(value)
    {}
    int x;
    int y;
    bool value;
};

class GameOfLife
{
public:

    // Create a new blank simulation with a grid of given width and height.
    GameOfLife(int width, int height);

    // Generate a fun initial state.
    void InitialState();

    // Advance the game by one step. Returns a vector of all the tiles that
    // changed state.
    std::vector<Tile> Advance();

    // Returns a vector of all tiles in the simulation space.
    std::vector<Tile> Space();

    // Update the value of a square at grid position (x, y) to the given value.
    // If the position (x, y) is out of bounds, does nothing.
    void Update(int x, int y, bool value);

    // Set a square.
    void Place(int x, int y);

    // Unset a square.
    void Remove(int x, int y);

    // Reset the game.
    void Reset();

    // Return the height of the game space.
    inline int Width() { return m_width; }

    // Return the width of the game space.
    inline int Height() { return m_height; }

private:
    // Vector of width * height bools describing the state of each square.
    std::vector<bool> m_space;

    // Width and height of the game space
    int m_width;
    int m_height;
};