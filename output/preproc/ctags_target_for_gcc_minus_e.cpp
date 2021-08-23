# 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework.ino"



# 5 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework.ino" 2


Application app;

void setup()
{
    app.Setup();
    app.Initialize();




    Serial.println("Mega");

}

void loop()
{
    app.Run();
}
