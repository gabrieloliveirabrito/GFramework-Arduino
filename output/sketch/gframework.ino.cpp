#include <Arduino.h>
#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework.ino"
#include "src/application.hpp"

Application app;
#line 4 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework.ino"
void setup();
#line 9 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework.ino"
void loop();
#line 4 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework.ino"
void setup()
{
    app.Initialize();
}

void loop()
{
    app.Run();
}

