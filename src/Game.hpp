#pragma once

#include <vector>
#include <tuple>

// Simple POD for square data.
struct Square {
    Square(int x, int y, bool value)
        : x(x)
        , y(y)
        , value(value)
    {}
    int x;
    int y;
    bool value;
};

class Game
{
public:

    // Create a new blank game of life with grid of width X height.
    Game(int width, int height);

    // Generate a fun initial state.
    void InitialState();

    // Advance the game by one step.
    std::vector<Square> Advance();

    // Returns all set
    std::vector<Square> Space();

    // Update the value of a square.
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