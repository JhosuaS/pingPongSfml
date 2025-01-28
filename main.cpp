////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <iostream>

#include <SFML/Graphics.hpp>

#include <SFML/Audio.hpp>

#include <filesystem>
#include <random>
#include <string>

#include <cmath>
#include <cstdlib>

#ifdef SFML_SYSTEM_IOS
#include <SFML/Main.hpp>
#endif

using namespace std;
using namespace sf;

const char nombreVentana[]= "Pong";
const float anchoVentana = 1280;
const float altoVentana = 720;


int main (){
    bool jugando = false;
    Clock reloj;
    random_device rd;
    mt19937 rng(rd());

    //Se genera la ventana del juego
    RenderWindow ventana(VideoMode({static_cast<unsigned int>(anchoVentana), static_cast<unsigned int>(altoVentana)}),nombreVentana);
    ventana.setFramerateLimit(60);//limita la cantidad de fps a 60
    ventana.clear();
    
    //declaración de texturas 
    Texture pelotaT, player1T, player2T;
    
    if (!pelotaT.loadFromFile("resources/pelota.png")){
        cout << "Error al cargar textura pelota" << endl;
    }
    if (!player1T.loadFromFile("resources/player1.png")){
        cout << "Error al cargar textura jugador 1" << endl;
    }
    if (!player2T.loadFromFile("resources/player2.png")){
        cout << "Error al cargar textura jugador 2" << endl;
    }
    //Declaración de fuente
    Font fuente("resources/8bit16.ttf");

    //inicialización de fuente
    Text pauseMessage(fuente);
    pauseMessage.setCharacterSize(40);
    pauseMessage.setPosition({170.f, 200.f});
    pauseMessage.setFillColor(sf::Color::White);

    //Inicialización de Sprites
    Sprite pelotaS(pelotaT);
    pelotaS.setOrigin({static_cast<float>(pelotaT.getSize() .x) /2, static_cast<float>(pelotaT.getSize() .y) /2});
    pelotaS.setScale({static_cast<float>(0.15), static_cast<float>(0.15)});
        
    Sprite player1S(player1T);
    player1S.setOrigin({static_cast<float>(player1T.getSize() .x) /2, static_cast<float>(player1T.getSize() .y) /2});
    player1S.setScale({static_cast<float>(0.25), static_cast<float>(0.25)});
        
    Sprite player2S(player2T);
    player2S.setOrigin({static_cast<float>(player2T.getSize() .x) /2, static_cast<float>(player2T.getSize() .y) /2});
    player2S.setScale({static_cast<float>(0.25), static_cast<float>(0.25)});
        
    //mensaje
    pauseMessage.setString("Bienvenido\n\n Presione espacio para continuar");
    
    //Se definen las propiedades de los jugadores
    Clock      aiTimer;
    const Time aiTime = seconds(0.1f);
    const float velJugador = 400.f;
    float velAi = 0.f;
    const float velPelota = 400.f;
    Angle anguloPelota = degrees(0); // cambia durante la ejecucion

    //se da paso al bucle del juego
    while (ventana.isOpen())
    {
        //Se establece la lista de eventos, como cerrar o minimizar la ventana
        while(const optional evento = ventana.pollEvent()){
            if(evento->is<sf::Event::Closed>() || (evento->is<sf::Event::KeyPressed>() &&
                 evento->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)){
                ventana.close();
                break;
            }
            if(evento->is<sf::Event::KeyPressed>() &&
                 evento->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Space){
                    if(!jugando){
                        //reinicia el juego
                        jugando = true;
                        reloj.restart();

                        //reestablece las posiciones de los jugadores
                        pelotaS.setPosition({static_cast<float>(anchoVentana) /2, static_cast<float>(altoVentana) /2});
                        player1S.setPosition({static_cast<float>(anchoVentana) - anchoVentana + 40, static_cast<float>(altoVentana) /2});
                        player2S.setPosition({static_cast<float>(anchoVentana) - 40, static_cast<float>(altoVentana) /2});

                        //reestablece el angulo de la pelota, el coseno verifica que no sea vertical
                        do{
                            anguloPelota = degrees(uniform_real_distribution<float>(0, 360)(rng));
                        }while (abs(cos(anguloPelota.asRadians())) < 0.7f);                        
                    }
                 }
        }
        if(jugando)
        {
            const float deltaTime = reloj.restart().asSeconds();
            
            //movimiento del jugador
            if (Keyboard::isKeyPressed(Keyboard::Key::Up) && (player1S.getPosition().y - player1T.getSize().y / 2 > 5.f))
            {
                player1S.move({0.f, - velJugador * deltaTime});
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::Down) && (player1S.getPosition().y + player1T.getSize().y / 2 < altoVentana - 5.f))
            {
                player1S.move({0.f, velJugador * deltaTime});
            }
            
            // Movimiento del computador
            if (((velAi < 0.f) && (player2S.getPosition().y - player2T.getSize().y / 2 > 5.f)) ||
                ((velAi > 0.f) && (player2S.getPosition().y + player2T.getSize().y / 2 < altoVentana - 5.f)))
            {
                player2S.move({0.f, velAi * deltaTime});
            }

            //actualiza el movimiento según la posición de la pelota
            if (aiTimer.getElapsedTime() > aiTime)
            {
                aiTimer.restart();
                if (pelotaS.getPosition().y  > player2S.getPosition().y)
                    velAi = velJugador;
                else if (pelotaS.getPosition().y  < player2S.getPosition().y)
                    velAi = -velJugador;
                else
                    velAi = 0.f;
            }

            // Movimiento de la pelota
            pelotaS.move({velPelota * deltaTime, anguloPelota});

            //Colisiones de la pelota con la pantalla
            if (pelotaS.getPosition().y < 0.f)
            {
                anguloPelota= -anguloPelota;
                pelotaS.setPosition({pelotaS.getPosition().x, pelotaT.getSize().y / 2 + 0.1f});
            }
            if (pelotaS.getPosition().y + pelotaT.getSize().y / 2  > altoVentana)
            {
                anguloPelota= -anguloPelota;
                pelotaS.setPosition({pelotaS.getPosition().x, altoVentana - pelotaT.getSize().y / 2 - 0.1f});
            }
        }
        ventana.clear();
        ventana.draw(pelotaS);
        ventana.draw(player1S);
        ventana.draw(player2S);
        ventana.display();
    }
    return 0;
}
