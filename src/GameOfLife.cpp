#include "GameOfLife.hpp"

#include <cmath>
#include <unordered_map>
#include <array>

GameOfLife::GameOfLife(int width, int height)
    : m_space()
    , m_width(width)
    , m_height(height)
{}

void GameOfLife::add_glider()
{
    int x = m_width / 2;
    int y = m_height / 2;

    // Glider
    place(x + 0, y + 2);
    place(x + 1, y + 3);
    place(x + 2, y + 1);
    place(x + 2, y + 2);
    place(x + 2, y + 3);
}

void GameOfLife::advance()
{
    /// @todo Multithreading with a std::latch, quad tree.
    /// @bug Alone tiles are not updated.

    // Surrounding alive counts of each tile.
    std::unordered_map<Tile, int> counts;

    // Return the number wrapped around between 0 and n. If a > n, then  repeat
    // a - n until a < n.
    auto wrap = [](int a, int n) {
        return ((a % n) + n) % n;
    };

    // Increment the count of all neighbors of each alive tile.
    for (auto tile : m_space) {

        // Cardinal neighbors.
        int above = wrap(tile.y - 1, m_height);
        int below = wrap(tile.y + 1, m_height);
        int left = wrap(tile.x - 1, m_width);
        int right = wrap(tile.x + 1, m_width);

        std::array<Tile, 8> neighbors = {
            Tile(left, above),
            Tile(tile.x, above),
            Tile(right, above),
            Tile(left, tile.y),
            Tile(right, tile.y),
            Tile(left, below),
            Tile(tile.x, below),
            Tile(right, below)
        };

        // Increment all neighbors.
        for (auto neighbor : neighbors) {
            auto it = counts.find(neighbor);

            if (it == counts.end()) {
                counts.emplace(neighbor, 1);
                continue;
            }

            it->second += 1;
        };
    }

    // Lock during write only.
    std::scoped_lock<std::mutex> lock(m_mutex);

    // These lines of code make up the logic of the simulation!
    for (auto [tile, n] : counts) {

        // If the tile was alive last iteration.
        auto it = m_space.find(tile);
        bool alive = it != m_space.end();

        if (alive && (n == 2 || n == 3)) {
            continue;
        }
        else if (!alive && n == 3) {
            m_space.insert(tile);
        }
        else if (alive) {
            m_space.erase(it);
        }
    }
}

std::unordered_set<Tile> GameOfLife::space()
{
    std::scoped_lock<std::mutex> space_lock(m_mutex);
    return m_space;
}

void GameOfLife::update(int x, int y, bool value)
{
    std::scoped_lock<std::mutex> lock(m_mutex);

    // If the position is out of bounds then do nothing.
    if (x > m_width || x < 0 || y > m_height || y < 0) {
        return;
    }

    Tile tile {x, y};

    // Try to find the tile if it exists.
    auto it = m_space.find(tile);

    if (it == m_space.end()) {
        if (value) {
            m_space.insert(it, tile);
        }
    }
    else {
        if (!value) {
            m_space.erase(it);
        }
    }
}

void GameOfLife::clear()
{
    std::scoped_lock<std::mutex> lock(m_mutex);
    m_space.clear();
}
