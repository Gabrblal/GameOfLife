#include "Game.hpp"

#include <iostream>
#include <cmath>

Game::Game(int width, int height)
    : m_space(width * height, false)
    , m_width(width)
    , m_height(height)
{}

void Game::InitialState()
{
    Place(0, 2);
    Place(1, 3);
    Place(2, 1);
    Place(2, 2);
    Place(2, 3);
}

std::vector<Square> Game::Advance()
{
    std::vector<Square> squares;

    auto wrap = [](int a, int n) {
        return ((a % n) + n) % n;
    };

    // Loop through the center of the grid.
    for (int y = 0; y < m_height; ++y) {
        int above = wrap(y - 1, m_height);
        int below = wrap(y + 1, m_height);

        for (int x = 0; x < m_width; ++x) {
            int left = wrap(x - 1, m_width);
            int right = wrap(x + 1, m_width);

            // Count of white squares.
            int N = 0;

            // Loop through surrounding squares.
            N += m_space.at(above * m_width + left );
            N += m_space.at(above * m_width + x    );
            N += m_space.at(above * m_width + right);
            N += m_space.at(    y * m_width + left );
            N += m_space.at(    y * m_width + right);
            N += m_space.at(below * m_width + left );
            N += m_space.at(below * m_width + x    );
            N += m_space.at(below * m_width + right);

            bool alive = m_space.at(y * m_width + x);

            if (alive && (N == 2 || N == 3)) {
                continue;
            }
            else if (!alive && N == 3) {
                squares.push_back({x, y, true});
            }
            else if (alive) {
                squares.push_back({x, y, false});
            }
        }
    }

    // Apply changes, critically after the grid has been fully analysed.
    for (Square square : squares) {
        int position = square.y * m_width + square.x;
        m_space[position] = square.value;
    }

    return squares;
}

std::vector<Square> Game::Space()
{
    std::vector<Square> space;

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            space.push_back({x, y, m_space.at(y * m_width + x)});
        }
    }

    return space;
}

void Game::Update(int x, int y, bool value)
{
    if (x > m_width || x < 0) {
        return;
    }

    if (y > m_height || y < 0) {
        return;
    }

    m_space[y * m_width + x] = value;
}

void Game::Place(int x, int y)
{
    Update(x, y, true);
}

void Game::Remove(int x, int y)
{
    Update(x, y, false);
}

void Game::Reset()
{
    for (auto it = m_space.begin(); it != m_space.end(); it++) {
        *it = false;
    }
}