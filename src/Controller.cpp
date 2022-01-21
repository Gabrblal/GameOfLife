#include "Controller.hpp"

#include <vector>

using namespace std::chrono;

Controller::Controller()
    : m_view()
    , m_model(m_view.Width() / 20, m_view.Height() / 20)
    , m_model_delta(100000)
    , m_model_delta_minimum(100)
    , m_model_delta_maximum(1000000)
    , m_view_delta(6944)
    , m_exit(false)
    , m_paused(false)
    , m_left(false)
    , m_right(false)
    , m_up(false)
    , m_down(false)
{
    m_view.Set(m_model.Width(), m_model.Height());

    // Start the simulation.
    m_model_thread = std::thread(&Controller::SimulationThread, this, std::ref(m_exit));
    m_view_thread = std::thread(&Controller::ViewThread, this, std::ref(m_exit));

    // Initialise the simulation.
    m_model.InitialState();

    // Update the view with the initial state.
    m_view.Render(m_model.Space());
    m_view.Display();
}

Controller::~Controller()
{
    m_exit = true;

    if (m_model_thread.joinable())
        m_model_thread.join();

    if (m_view_thread.joinable())
        m_view_thread.join();
}

void Controller::SimulationThread(std::atomic_bool &exit)
{
    std::unique_lock<std::mutex> lock(m_model_cv_mutex);
    m_model.InitialState();

    while (true) {

        m_model_cv.wait_until(
            lock,
            high_resolution_clock::now() + microseconds(m_model_delta),
            [&]{
                // Return true for stop waiting.
                return m_exit || m_paused;
            }
        );

        // Check for exit signal
        if (m_exit)
            break;

        // Catch spurious wakeups.
        if (m_paused)
            continue;

        m_view.Render(m_model.Advance());
    }
}

void Controller::ViewThread(std::atomic_bool &exit)
{
    std::unique_lock<std::mutex> lock(m_view_cv_mutex);

    while (true) {

        using namespace std::chrono;

        m_model_cv.wait_until(
            lock,
            high_resolution_clock::now() + microseconds(m_view_delta),
            [&]{
                return m_exit.load();
            }
        );

        // Check for exit signal
        if (m_exit)
            break;

        m_view.Display();
    }
}

void Controller::Loop()
{
    sf::Event event;
    while (!m_exit)
    {
        m_view.WaitEvent(event);

        switch(event.type)
        {
            case sf::Event::Closed             : Exit(); return;           break;
            case sf::Event::KeyPressed         : HandleKeyPress(event);    break;
            case sf::Event::KeyReleased        : HandleKeyRelease(event);  break;
            case sf::Event::Resized            : HandleResize(event);      break;
            case sf::Event::MouseButtonPressed : HandleMousePress(event);  break;
            case sf::Event::MouseWheelScrolled : HandleMouseScroll(event); break;
            default: break;
        }
    }
}

void Controller::Exit()
{
    // Set exit flag.
    m_exit = true;

    // Notify all the threads if they are waiting.
    m_model_cv.notify_all();
    m_view_cv.notify_all();

    // Join them.
    m_model_thread.join();
    m_view_thread.join();
}

void Controller::HandleKeyPress(sf::Event &event)
{
    switch(event.key.code)
    {
        case sf::Keyboard::Escape : Exit(); break;
        case sf::Keyboard::Space  : m_paused = !m_paused;    break;
        case sf::Keyboard::W : m_up    = true; HandleMovement(); break;
        case sf::Keyboard::A : m_left  = true; HandleMovement(); break;
        case sf::Keyboard::S : m_down  = true; HandleMovement(); break;
        case sf::Keyboard::D : m_right = true; HandleMovement(); break;
        case sf::Keyboard::Up   : HandleSpeed(true); break;
        case sf::Keyboard::Down : HandleSpeed(false); break;
        default: break;
    }
}

void Controller::HandleKeyRelease(sf::Event &event)
{
    switch(event.key.code)
    {
        case sf::Keyboard::W : m_up    = false; HandleMovement(); break;
        case sf::Keyboard::A : m_left  = false; HandleMovement(); break;
        case sf::Keyboard::S : m_down  = false; HandleMovement(); break;
        case sf::Keyboard::D : m_right = false; HandleMovement(); break;
        default: break;
    }
}

void Controller::HandleMovement()
{
    if (m_left == m_right)
        m_view.PanHorisontal(View::MoveAction::Stay);
    else if (m_left)
        m_view.PanHorisontal(View::MoveAction::Backward);
    else
        m_view.PanHorisontal(View::MoveAction::Forward);

    if (m_up == m_down)
        m_view.PanVertical(View::MoveAction::Stay);
    else if (m_up)
        m_view.PanVertical(View::MoveAction::Forward);
    else
        m_view.PanVertical(View::MoveAction::Backward);
}

void Controller::HandleResize(sf::Event &event)
{
    return;
}

void Controller::HandleMousePress(sf::Event &event)
{
    // Get the tile clicked.
    sf::Vector2i tile = m_view.MapPixelToTile(
        event.mouseButton.x,
        event.mouseButton.y
    );

    // If a left click occured, set the tile, otherwise remove it.
    if (event.mouseButton.button == sf::Mouse::Left) {
        m_model.Place(tile.x, tile.y);
        m_view.Render(tile.x, tile.y, true);
    }
    else {
        m_model.Remove(tile.x, tile.y);
        m_view.Render(tile.x, tile.y, false);
    }
}

void Controller::HandleMouseScroll(sf::Event &event)
{
    if (event.mouseWheelScroll.delta < 0) {
        m_view.Zoom(View::ZoomAction::In);
    }
    else {
        m_view.Zoom(View::ZoomAction::Out);
    }
}

void Controller::HandleSpeed(bool increase)
{
    if (increase)
        m_model_delta = m_model_delta / 2;
    else
        m_model_delta =  m_model_delta * 2;

    if (m_model_delta < m_model_delta_minimum)
        m_model_delta = m_model_delta_minimum;

    else if (m_model_delta > m_model_delta_maximum)
        m_model_delta = m_model_delta_maximum;
}