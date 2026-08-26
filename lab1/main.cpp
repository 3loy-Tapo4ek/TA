#include "ConsoleView.hpp"
#include "AppController.hpp"

int main()
{
    ConsoleView view;

    AppController app(view);

    app.run();

    return 0;
}