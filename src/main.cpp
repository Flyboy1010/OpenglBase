#include "Application.h"

int main()
{
    Application app;

    if (app.Init("Game", 1280, 720) != -1)
    {
        app.Run();
    }
    
    return 0;
}
