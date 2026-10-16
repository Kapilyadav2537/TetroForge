#include <tf/ui/Application.hpp>

/**
 * @brief Entry point for TetroForge.
 *
 * Step 1: Instantiates the Application coordinator (owns the window, screens, and audio).
 * Step 2: Executes the main game loop via application.run().
 * Step 3: Returns the exit code (0 on normal shutdown).
 */
int main() {
    tf::ui::Application application;
    return application.run();
}
