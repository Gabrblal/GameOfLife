#include "GameOfLife.hpp"

#include <cmath>

GameOfLife::GameOfLife(int width, int height)
    : m_space(width * height, false)
    , m_width(width)
    , m_height(height)
{}

void GameOfLife::InitialState()
{
    // Glider
    Place(0, 2);
    Place(1, 3);
    Place(2, 1);
    Place(2, 2);
    Place(2, 3);
}

std::vector<Tile> GameOfLife::Advance()
{
    std::vector<Tile> tiles;

    // Return the number wrapped around between 0 and n. If a > n, then
    // repeat a - n until a < n.
    auto wrap = [](int a, int n) {
        return ((a % n) + n) % n;
    };

    // TODO: Optimise by looping through set squares only.
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

            // These 6 lines of code make up the entire logic of this
            // simulation!
            if (alive && (N == 2 || N == 3)) {
                continue;
            }
            else if (!alive && N == 3) {
                tiles.push_back({x, y, true});
            }
            else if (alive) {
                tiles.push_back({x, y, false});
            }
        }
    }

    // Apply changes, after the grid has been fully analysed.
    for (Tile tile : tiles) {
        int position = tile.y * m_width + tile.x;
        m_space[position] = tile.value;
    }

    return tiles;
}

std::vector<Tile> GameOfLife::Space()
{
    std::vector<Tile> space;
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            space.push_back({x, y, m_space.at(y * m_width + x)});
        }
    }

    return space;
}

void GameOfLife::Update(int x, int y, bool value)
{
    if (x > m_width || x < 0) {
        return;
    }

    if (y > m_height || y < 0) {
        return;
    }

    m_space[y * m_width + x] = value;
}

void GameOfLife::Place(int x, int y) {
    Update(x, y, true);
}

void GameOfLife::Remove(int x, int y) {
    Update(x, y, false);
}

void GameOfLife::Reset()
{
    for (auto it = m_space.begin(); it != m_space.end(); it++) {
        *it = false;
    }
}