#include <Arduino.h>
#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework.ino"
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
#include "srcNano/application.hpp"
#else
#include "src/application.hpp"
#endif

Application app;

#line 9 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework.ino"
void setup();
#line 21 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework.ino"
void loop();
#line 9 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework.ino"
void setup()
{
    app.Setup();
    app.Initialize();

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
    Serial.println("Nano");
#else
    Serial.println("Mega");
#endif
}

void loop()
{
    app.Run();
}

