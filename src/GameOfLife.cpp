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

std::array<Tile, 8> GameOfLife::neighbors(Tile tile)
{
    // Should probably be inlined.

    // Return the number wrapped around between 0 and n. If a > n, then  repeat
    // a - n until a < n.
    auto wrap = [](int a, int n) {
        return ((a % n) + n) % n;
    };

    // Cardinal neighbors.
    int above = wrap(tile.y - 1, m_height);
    int below = wrap(tile.y + 1, m_height);
    int left = wrap(tile.x - 1, m_width);
    int right = wrap(tile.x + 1, m_width);

    // Starting with north, this is a sequence of compass directions.
    return {
        Tile(tile.x, above), // North
        Tile(right, above),  // North East
        Tile(right, tile.y), // East
        Tile(right, below),  // South East
        Tile(tile.x, below), // South
        Tile(left, below),   // South West
        Tile(left, tile.y),  // West
        Tile(left, above)    // North West
    };
}

int GameOfLife::count(Tile tile)
{
    int n = 0;

    // Increment all neighbors.
    for (auto neighbor : neighbors(tile)) {
        if (m_space.contains(neighbor)) {
            n++;
        }
    };

    return n;
}

void GameOfLife::advance()
{
    /// @todo Multithreading with a std::latch and a thread pool. This algorithm
    /// is threadable because it only reads from the current game space when
    /// calculating the next iteration.

    // Surrounding alive counts of each tile.
    std::unordered_map<Tile, int> counts;

    for (auto tile : m_space) {

        int n = 0;

        // Count the number of alive neighbors. For all the dead neighbors,
        // count their alive neighbors if they are not already counted.
        for (auto neighbor : neighbors(tile)) {
            if (m_space.contains(neighbor)) {
                n++;
            }
            else if (!counts.contains(neighbor)) {
                counts.emplace(neighbor, count(neighbor));
            }
        };

        counts.emplace(tile, n);
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
