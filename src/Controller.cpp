#include "Controller.hpp"

#include <vector>

using namespace std::chrono;

Controller::Controller()
    : m_view()
    , m_model(m_view.width() / 20, m_view.height() / 20)
    , m_model_delta(100ms)
    , m_model_delta_minimum(1us)
    , m_model_delta_maximum(2s)
    , m_view_delta(1s / 144s)
    , m_paused(false)
    , m_left(false)
    , m_right(false)
    , m_up(false)
    , m_down(false)
{
    // Add a gilder to the centre of the game space.
    m_model.add_glider();

    m_view.set(m_model.width(), m_model.height());

    // Start the simulation thread first because the view thread depends on it.
    m_model_thread = std::jthread(
        &Controller::simulation_thread,
        this,
        m_stop.get_token()
    );

    // Start the thread updating the window.
    m_view_thread = std::jthread(
        &Controller::view_thread,
        this,
        m_stop.get_token()
    );

    // Update the view with the initial state.
    m_view.render(m_model.space());
    m_view.display();
}

void Controller::simulation_thread(std::stop_token stop)
{
    std::unique_lock<std::mutex> lock(m_model_condition_mutex);

    for (;;) {

        m_model_condition.wait_until(
            lock,
            stop,
            high_resolution_clock::now() + m_model_delta,
            [&]{ return m_paused.load(); }
        );

        // Check for exit signal
        if (stop.stop_requested())
            break;

        // Catch spurious wakeups.
        if (m_paused)
            continue;

        m_model.advance();
    }
}

void Controller::view_thread(std::stop_token stop)
{
    std::unique_lock<std::mutex> lock(m_view_condition_mutex);

    for (;;) {

        m_view_condition.wait_until(
            lock,
            stop,
            high_resolution_clock::now() + m_view_delta,
            [&]{ return false; }
        );

        // Check for exit signal
        if (stop.stop_requested())
            break;

        m_view.render(m_model.space());
        m_view.display();
    }
}

void Controller::main()
{
    // Stop signal on the main thread.
    std::stop_token stop = m_stop.get_token();
    sf::Event event;

    while (!stop.stop_requested())
    {
        m_view.window().waitEvent(event);

        switch(event.type)
        {
            case sf::Event::Closed             : exit(); return;           break;
            case sf::Event::KeyPressed         : handle_key_press(event);    break;
            case sf::Event::KeyReleased        : handle_key_release(event);  break;
            case sf::Event::Resized            : handle_resize(event);      break;
            case sf::Event::MouseButtonPressed : handle_mouse_press(event);  break;
            case sf::Event::MouseWheelScrolled : handle_mouse_scroll(event); break;
            default: break;
        }
    }
}

void Controller::exit()
{
    m_stop.request_stop();
}

void Controller::handle_key_press(sf::Event &event)
{
    switch(event.key.code)
    {
        case sf::Keyboard::Escape : exit(); break;
        case sf::Keyboard::Space  : m_paused = !m_paused;    break;
        case sf::Keyboard::W : m_up    = true; handle_movement(); break;
        case sf::Keyboard::A : m_left  = true; handle_movement(); break;
        case sf::Keyboard::S : m_down  = true; handle_movement(); break;
        case sf::Keyboard::D : m_right = true; handle_movement(); break;
        case sf::Keyboard::Up   : handle_speed(true); break;
        case sf::Keyboard::Down : handle_speed(false); break;
        default: break;
    }
}

void Controller::handle_key_release(sf::Event &event)
{
    switch(event.key.code)
    {
        case sf::Keyboard::W : m_up    = false; handle_movement(); break;
        case sf::Keyboard::A : m_left  = false; handle_movement(); break;
        case sf::Keyboard::S : m_down  = false; handle_movement(); break;
        case sf::Keyboard::D : m_right = false; handle_movement(); break;
        default: break;
    }
}

void Controller::handle_movement()
{
    if (m_left == m_right)
        m_view.pan_horisontal(View::MoveAction::STAY);
    else if (m_left)
        m_view.pan_horisontal(View::MoveAction::BACKWARD);
    else
        m_view.pan_horisontal(View::MoveAction::FORWARD);

    if (m_up == m_down)
        m_view.pan_vertical(View::MoveAction::STAY);
    else if (m_up)
        m_view.pan_vertical(View::MoveAction::FORWARD);
    else
        m_view.pan_vertical(View::MoveAction::BACKWARD);
}

void Controller::handle_resize(sf::Event &event)
{
    return;
}

void Controller::handle_mouse_press(sf::Event &event)
{
    // Get the tile clicked.
    sf::Vector2i tile = m_view.map_pixel_to_tile(
        event.mouseButton.x,
        event.mouseButton.y
    );

    // If a left click occured, set the tile, otherwise remove it.
    if (event.mouseButton.button == sf::Mouse::Left) {
        m_model.place(tile.x, tile.y);
        m_view.render(tile.x, tile.y, true);
    }
    else {
        m_model.remove(tile.x, tile.y);
        m_view.render(tile.x, tile.y, false);
    }
}

void Controller::handle_mouse_scroll(sf::Event &event)
{
    if (event.mouseWheelScroll.delta < 0) {
        m_view.zoom(View::ZoomAction::IN);
    }
    else {
        m_view.zoom(View::ZoomAction::OUT);
    }
}

void Controller::handle_speed(bool increase)
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
