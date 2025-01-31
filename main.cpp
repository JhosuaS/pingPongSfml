////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <iostream>

#include <SFML/Graphics.hpp>

#include <SFML/Audio.hpp>

#include <filesystem>
#include <random>
#include <string>
#include <fstream>

#include <cmath>
#include <cstdlib>
#include <vector>
#include <cctype>

#ifdef SFML_SYSTEM_IOS
#include <SFML/Main.hpp>
#endif

using namespace std;
using namespace sf;

//constantes globales
const string nombreVentana = "Cat - Pong";
const string restartText = "\nPresione espacio para continuar\n\nEsc para salir";
const float anchoVentana = 1280;
const float altoVentana = 720;
bool puntuaciones = false;

//funcion de archivos para puntuaciones
void escribir(const string &playerName){
    ofstream scores("scores.txt", ios::app);//se crea el archivo para escribir los puntajes

    if(scores.fail()){
        cout << "Error al generar el archivo de puntuacion" << endl;
    }

    if(scores.is_open()){
        scores << "\n" << playerName;
        scores.close();
        cout << "Nombre agregado correctamente: " << playerName << endl; 
    }

    scores << playerName;
}

//funcion para leer los nombres de los jugadores
string leer() {
    ifstream scores("scores.txt", ios::in);

    if (scores.fail()) {
        cout << "Error al leer el archivo de puntuacion" << endl;
        return "";
    }

    string nombres [6];//se inicia de esta forma porque el archivo empieza con una linea en blanco
    int counter = 0;
    string linea;
    while ((getline(scores, linea)) && counter < 6) {
        nombres[counter] = linea;// se llena el vector nombres con los nombres de los jugadores
        counter++;
    }
    scores.close();

    // Construir la cadena de los nombres de los cinco mejores jugadores
    return "1. " + nombres[1] + "\n\n2. " + nombres[2] + "\n\n3. " + nombres[3] + "\n\n4. " + nombres[4] + "\n\n5. " + nombres[5];
}

//funcion principal
int main (){
    bool jugando = false;
    bool perdiste = false;
    bool ingresaNombre = false;
    bool creditos = false;
    String playerName = " "; //String especial de SFML para manejar texto
    Clock reloj;
    random_device rd;
    mt19937 rng(rd());

    //Se genera la ventana del juego
    RenderWindow ventana(VideoMode({static_cast<unsigned int>(anchoVentana), static_cast<unsigned int>(altoVentana)}),nombreVentana);
    ventana.setFramerateLimit(60);//limita la cantidad de fps a 60
    ventana.clear();
    
    //declaración de texturas 
    Texture pelotaT, player1T, player2T, mainScreenT, standbyT, playMapT;
    
    if (!pelotaT.loadFromFile("resources/pelota.png")){
        cout << "Error al cargar textura pelota" << endl;
    }
    if (!player1T.loadFromFile("resources/player1.png")){
        cout << "Error al cargar textura jugador 1" << endl;
    }
    if (!player2T.loadFromFile("resources/player2.png")){
        cout << "Error al cargar textura jugador 2" << endl;
    }
    if (!mainScreenT.loadFromFile("resources/mainScreen.png")){
        cout << "Error al cargar textura de la pantalla principal" << endl;
    }
    if (!standbyT.loadFromFile("resources/standby.png")){
        cout << "Error al cargar textura de la pantalla de espera" << endl;
    }
    if (!playMapT.loadFromFile("resources/playMap.png")){
        cout << "Error al cargar textura del tablero de juego" << endl;
    }

    //Inicializacion de sonidos
    const SoundBuffer sonidoPelota("resources/pelota.wav");
    Sound bounce(sonidoPelota);

    //Declaración de fuente
    Font fuente("resources/8bit16.ttf");
    
    //inicialización de mensajes
    Text pauseMessage(fuente);
    pauseMessage.setCharacterSize(32);
    pauseMessage.setPosition({340.f, 200.f});
    pauseMessage.setFillColor(sf::Color::White);

    Text creditsText(fuente);
    creditsText.setPosition({340.f, 200.f});
    creditsText.setFillColor(sf::Color::White);

    Text winnerName(fuente);
    winnerName.setCharacterSize(32);
    winnerName.setPosition({static_cast<float>(anchoVentana) / 2, static_cast<float>(altoVentana) / 2});
    winnerName.setFillColor(sf::Color::White);
    
    //mensajes
    pauseMessage.setString("Bienvenido\n\nPresione:\n\n Espacio para continuar\n\n Esc para salir\n\n Tab para ver puntuaciones \n\n Enter para creditos");
    
    //Inicialización de Sprites
    Sprite pelotaS(pelotaT);
    pelotaS.setOrigin({static_cast<float>(pelotaT.getSize() .x) /2, static_cast<float>(pelotaT.getSize() .y) /2});
    pelotaS.setScale({static_cast<float>(0.15), static_cast<float>(0.15)});
    const float pelotaRadio = ((pelotaT.getSize().y) * (pelotaS.getScale().y)) / 2;
        
    Sprite player1S(player1T);
    player1S.setOrigin({static_cast<float>(player1T.getSize() .x) /2, static_cast<float>(player1T.getSize() .y) /2});
    player1S.setScale({static_cast<float>(0.25), static_cast<float>(0.25)});
    const float player1Radio = ((player1T.getSize().y) * (player1S.getScale().y)) / 2;
        
    Sprite player2S(player2T);
    player2S.setOrigin({static_cast<float>(player2T.getSize() .x) /2, static_cast<float>(player2T.getSize() .y) /2});
    player2S.setScale({static_cast<float>(0.25), static_cast<float>(0.25)});
    const float player2Radio = ((player2T.getSize().y) * (player2S.getScale().y)) / 2;

    Sprite mainScreenS(mainScreenT);
    mainScreenS.setOrigin({static_cast<float>(mainScreenT.getSize() .x) /2, static_cast<float>(mainScreenT.getSize() .y) /2});
    mainScreenS.setPosition({static_cast<float>(anchoVentana) /2, static_cast<float>(altoVentana) /2});

    Sprite standbyS(standbyT);
    standbyS.setOrigin({static_cast<float>(standbyT.getSize() .x) /2, static_cast<float>(standbyT.getSize() .y) /2});
    standbyS.setPosition({static_cast<float>(anchoVentana) /2, static_cast<float>(altoVentana) /2});

    Sprite playMapS(playMapT);
    playMapS.setOrigin({static_cast<float>(playMapT.getSize() .x) /2, static_cast<float>(playMapT.getSize() .y) /2});
    playMapS.setPosition({static_cast<float>(anchoVentana) /2, static_cast<float>(altoVentana) /2});
        
    //Se definen las propiedades de los jugadores
    Clock      aiTimer;
    const Time aiTime = seconds(0.1f);
    const float velJugador = 400.f;
    float velAi = 300.f;
    const float velPelota = 400.f;
    Angle anguloPelota = degrees(0); // cambia durante la ejecucion

    //se da paso al bucle del juego
    while (ventana.isOpen())
    {
        //Se establece la lista de eventos, como cerrar o minimizar la ventana
        while(const optional evento = ventana.pollEvent()){
            if(evento->is<Event::Closed>() || (evento->is<Event::KeyPressed>() &&
                 evento->getIf<Event::KeyPressed>()->code == Keyboard::Key::Escape)){
                ventana.close();
                break;
            }

            //vista para mostar puntuaciones
            if((evento->is<Event::KeyPressed>() &&
                 evento->getIf<Event::KeyPressed>()->code == Keyboard::Key::Tab)){
                ventana.clear();
                puntuaciones = true;
            }

            //vista para mostar creditos
            if((evento->is<Event::KeyPressed>() &&
                 evento->getIf<Event::KeyPressed>()->code == Keyboard::Key::Enter)){
                creditos = true;
            }

            //ingresar nombre ganadores
            if(ingresaNombre && evento ->is<Event::TextEntered>()){
                    char tecla = evento ->getIf<Event::TextEntered>()->unicode;
                    if (tecla == '\b' && playerName.getSize() > 0) { // Manejo de backspace
                        playerName.erase(playerName.getSize() - 1, 1);
                    } else if (tecla == '\r' || tecla == ' ') { // Al presionar Enter o espacio, se guarda el nombre
                        escribir(playerName.toAnsiString());
                        ingresaNombre = false; // Salir del modo de ingreso
                    } else if (tecla < 128) { // Rango de caracteres imprimibles en ASCII
                        playerName += static_cast<char>(tecla);
                    }
                    float writingX = winnerName.getPosition().x + winnerName.getCharacterSize();
                    winnerName.setFillColor(Color::Black);
                    winnerName.setString(playerName);
                }
            
            //inicio del juego
            if(evento->is<Event::KeyPressed>() &&
                 evento->getIf<Event::KeyPressed>()->code == Keyboard::Key::Space){
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
            if (Keyboard::isKeyPressed(Keyboard::Key::Up) && (player1S.getPosition().y - player1Radio > 5.f))
            {
                player1S.move({0.f, - velJugador * deltaTime});
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::Down) && (player1S.getPosition().y + player1Radio < altoVentana - 5.f))
            {
                player1S.move({0.f, velJugador * deltaTime});
            }
            
            // Movimiento del computador
            if (((velAi < 0.f) && (player2S.getPosition().y - player2Radio > 5.f)) ||
                ((velAi > 0.f) && (player2S.getPosition().y + player2Radio < altoVentana - 5.f)))
            {
                player2S.move({0.f, velAi * deltaTime});
            }

            //actualiza el movimiento según la posición de la pelota
            if (aiTimer.getElapsedTime() > aiTime)
            {
                aiTimer.restart();
                if (pelotaS.getPosition().y + pelotaRadio > player2S.getPosition().y + player2Radio)
                    velAi = 0.65 * velJugador;
                else if (pelotaS.getPosition().y - pelotaRadio < player2S.getPosition().y - player2Radio)
                    velAi = - 0.65* velJugador;
                else
                    velAi = 0.f;
            }

            // Movimiento de la pelota
            pelotaS.move({velPelota * deltaTime, anguloPelota});

            //Colisiones de la pelota con la pantalla
            //Colisiones eje y
            if (pelotaS.getPosition().y - pelotaRadio < 0.f)
            {
                bounce.play();
                anguloPelota= -anguloPelota;
                pelotaS.setPosition({pelotaS.getPosition().x, pelotaRadio + 0.1f});
            }
            if (pelotaS.getPosition().y + pelotaRadio > altoVentana)
            {
                bounce.play();
                anguloPelota= -anguloPelota;
                pelotaS.setPosition({pelotaS.getPosition().x, altoVentana - pelotaRadio - 0.1f});
            }

            //Colisiones eje x (derrota y victoria)
            if (pelotaS.getPosition().x - pelotaRadio < 0.f)
            {
                jugando = false;
                perdiste = true;
            }
            if (pelotaS.getPosition().x + pelotaRadio > anchoVentana)
            {
                jugando = false;
                ingresaNombre = true;
            }

            //Colisiones de los jugadores
            uniform_real_distribution<float> dist(0, 20); //distribucion para aleatorizar el angulo de rebote de la pelota

            //colision jugador 1 (humano - izquierda)
            if (pelotaS.getPosition().x - pelotaRadio < player1S.getPosition().x + player1Radio &&
                pelotaS.getPosition().x - pelotaRadio > player1S.getPosition().x &&
                pelotaS.getPosition().y + pelotaRadio >= player1S.getPosition().y - player1Radio &&
                pelotaS.getPosition().y - pelotaRadio <= player1S.getPosition().y + player1Radio)
            {
                if (pelotaS.getPosition().y > player1S.getPosition().y)
                    anguloPelota = degrees(180) - anguloPelota + degrees(dist(rng));
                else
                    anguloPelota = degrees(180) - anguloPelota - degrees(dist(rng));

                bounce.play();
                pelotaS.setPosition({player1S.getPosition().x + pelotaRadio + player1Radio + 0.1f, pelotaS.getPosition().y});
            }

            //colision jugador 2 (maquina - derecha)
            if (pelotaS.getPosition().x + pelotaRadio > player2S.getPosition().x - player2Radio &&
                pelotaS.getPosition().x + pelotaRadio < player2S.getPosition().x &&
                pelotaS.getPosition().y + pelotaRadio >= player2S.getPosition().y - player2Radio &&
                pelotaS.getPosition().y - pelotaRadio <= player2S.getPosition().y + player2Radio)
            {
                if (pelotaS.getPosition().y > player2S.getPosition().y)
                    anguloPelota = degrees(180) - anguloPelota + degrees(dist(rng));
                else
                    anguloPelota = degrees(180) - anguloPelota - degrees(dist(rng));

                bounce.play();
                pelotaS.setPosition({player2S.getPosition().x - pelotaRadio - player2Radio - 0.1f, pelotaS.getPosition().y});
            }
        }
        
        //se dibuja en pantalla
        //limpia la ventana
        ventana.clear();

        if(jugando)
        {
            //mientras se encuentre jugando se dibujan los sprites
            ventana.draw(playMapS);
            ventana.draw(pelotaS);
            ventana.draw(player1S);
            ventana.draw(player2S);
        }
        
        //muestra el nombre del ganador
        else if (ingresaNombre)
        {
            ventana.draw(standbyS);
            pauseMessage.setString("Ganaste!\n\nIngresa tu nombre\n\nsin espacios: ");
            pauseMessage.setFillColor(Color::Black);
            ventana.draw(pauseMessage);
            ventana.draw(winnerName);
        }

        //muestra las puntuaciones
        else if(puntuaciones){
            ventana.draw(standbyS);
            pauseMessage.setCharacterSize(26);
            pauseMessage.setString("Ganadores\n\n" + leer() + "\n\n" + restartText);
            pauseMessage.setFillColor(Color::Black);
            ventana.draw(pauseMessage);
        }

        //muestra los creditos
        else if(creditos){
            ventana.draw(standbyS);
            creditsText.setCharacterSize(28);
            creditsText.setString("Proyecto para la asignatura de programacion\n\nIntegrantes\n\n\tJhosua Saa\n\n\tErick Calapi\n\n" + restartText);
            creditsText.setFillColor(Color::Black);
            ventana.draw(creditsText);
        }

        //muestra que el jugador perdió y debe reiniciar
        else if (perdiste)
        {
            ventana.draw(standbyS);
            pauseMessage.setString("Perdiste!\n\n" + restartText);
            pauseMessage.setFillColor(Color::Black);
            ventana.draw(pauseMessage);
        }
        
        //muestra el nombre del ganador
        else if (ingresaNombre)
        {
            ventana.draw(standbyS);
            pauseMessage.setString("Ganaste!\n\nIngresa tu nombre: ");
            pauseMessage.setFillColor(Color::Black);
            ventana.draw(pauseMessage);
            ventana.draw(winnerName);
        }
        
        else
        {   
            //mensaje de bienvenida
            ventana.draw(mainScreenS);
            pauseMessage.setCharacterSize(28);
            pauseMessage.setFillColor(Color::Black);
            ventana.draw(pauseMessage);
        }
        
        //muestra las cosas en pantalla
        ventana.display();
    }
    return 0;
}
