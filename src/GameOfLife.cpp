#include "GameOfLife.hpp"

#include <cmath>

GameOfLife::GameOfLife(int width, int height)
    : m_space(width * height, false)
    , m_width(width)
    , m_height(height)
{}

void GameOfLife::InitialState()
{
    int x = m_width / 2;
    int y = m_height / 2;

    // Glider
    Place(x + 0, y + 2);
    Place(x + 1, y + 3);
    Place(x + 2, y + 1);
    Place(x + 2, y + 2);
    Place(x + 2, y + 3);
}

std::vector<Tile> GameOfLife::Advance()
{
    std::scoped_lock<std::mutex> lock(m_space_mutex);

    std::vector<Tile> tiles {};

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
            N += m_space[above * m_width + left ];
            N += m_space[above * m_width + x    ];
            N += m_space[above * m_width + right];
            N += m_space[    y * m_width + left ];
            N += m_space[    y * m_width + right];
            N += m_space[below * m_width + left ];
            N += m_space[below * m_width + x    ];
            N += m_space[below * m_width + right];

            bool alive = m_space[y * m_width + x];

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
    for (Tile &tile : tiles)
        m_space[tile.y * m_width + tile.x] = tile.value;

    return tiles;
}

std::vector<Tile> GameOfLife::Space()
{
    std::scoped_lock<std::mutex> space_lock(m_space_mutex);

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
    std::scoped_lock<std::mutex> lock(m_space_mutex);

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
    std::scoped_lock<std::mutex> lock(m_space_mutex);

    for (auto it = m_space.begin(); it != m_space.end(); it++) {
        *it = false;
    }
}