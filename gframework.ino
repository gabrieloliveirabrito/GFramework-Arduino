#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
#include "srcNano/application.hpp"
#else
#include "src/application.hpp"
#endif

Application app;

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
