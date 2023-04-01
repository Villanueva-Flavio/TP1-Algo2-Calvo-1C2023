#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <unordered_map>
#define SIZE 20
#define VACIO ' '
#define INACTIVO 'X'
#define MINA '#'
#define JUGADOR_1 '1'
#define JUGADOR_2 '2'
#define MAX_MINAS 100
#define MAX_SOLDADOS 4
#define JUGADORES 2

using namespace std;

typedef struct Coordenada{
    int x;
    int y;
} Coordenada_t;

typedef struct CasilleroInactivo{
    Coordenada_t posicion;
    int turnosRestantes;
} CasilleroInactivo_t;

typedef struct Jugador{
    string nombre;
    Coordenada_t minas[MAX_MINAS];
    Coordenada_t soldados[MAX_SOLDADOS];
    int soldadosRestantes;
    int minasRestantes;
} Jugador_t;

typedef struct Tablero{
    CasilleroInactivo_t casillerosInactivos[MAX_MINAS];
    Jugador_t jugadores[JUGADORES];
    int turno;
    int estado;
    int cantidadInactivos;
}Tablero_t;


const string FILE_NAME[JUGADORES] = {"jugador1.txt", "jugador2.txt"};

const unordered_map<string, Coordenada_t> coordenadas_nuevas = {
    {"W", {0, -1}},
    {"A", {-1, 0}},
    {"S", {0, 1}},
    {"D", {1, 0}},
    {"W-A", {-1, -1}},
    {"W-D", {1, -1}},
    {"S-A", {-1, 1}},
    {"S-D", {1, 1}}
};

bool estaEnRango(int x, int y){
    return (x >= 0 && x < SIZE && y >= 0 && y < SIZE);
}

bool estaEnArray(Coordenada_t* array, int tope, Coordenada_t posicion){
    for(int i = 0; i < tope; i++){
        if(array[i].x == posicion.x && array[i].y == posicion.y){
            return true;
        }
    }
    return false;
}

void preguntarNombre(string* nombre, int jugador){
    cout << "Jugador " << jugador + 1 << " ingrese su nombre (MAX: 10 caracteres): ";
    cin >> *nombre;
    cout << endl;
    while(nombre->length() > 10){
        cout << "Por favor no exceda los 10 caracteres: ";
        cin >> *nombre;
    }
}

void preguntarCantidadMinas(int* minas){
    cout << "A continuación ingrese la cantidad de minas (MAX: 100): " ;
    cin >> *minas;
    cout << endl;
    while (*minas > MAX_MINAS || *minas < 1) {
        cout << "Por favor eliga entre 1 y 100: ";
        cin >> *minas;
        cout << endl;
    }
}

void preguntarMetodoInsercion(bool* metodo, int jugador){
    cout << "Jugador " << jugador + 1 << " desea que las minas y soldados se inserten de forma automatica? (Y/N): ";
    char respuesta;
    cin >> respuesta;
    cout << endl;
    while(respuesta != 'Y' && respuesta != 'y' && respuesta != 'N' && respuesta != 'n'){
        cout << "Por favor ingrese Y o N: ";
        cin >> respuesta;
        cout << endl;
    }
    *metodo = (respuesta =='Y' || respuesta == 'y')? true : false;    
}

void iniciarParametros(Tablero_t* tablero, bool* automatico){
    system("clear");
    preguntarCantidadMinas(&tablero->jugadores[0].minasRestantes);
    tablero->jugadores[1].minasRestantes = tablero->jugadores[0].minasRestantes;
    for(int i = 0; i < JUGADORES; i++){
        system("clear");
        preguntarNombre(&tablero->jugadores[i].nombre, i);
        preguntarMetodoInsercion(automatico, i);
        tablero->jugadores[i].soldadosRestantes = 4;
    }
}

void mensajeVictoria(int jugador, string nombre){
    cout << "##############################################" << endl;
    cout << "El jugador " << jugador << ": " << nombre << " ha ganado la partida" << endl;
    cout << "##############################################" << endl;
}

void mensajeFinal(int estado, string nombre1, string nombre2){
    switch (estado) {
        case 1:
            mensajeVictoria(1, nombre1);
            break;
        case 2:
            mensajeVictoria(2, nombre2);
            break;
        case 3:
            cout << "Empate" << endl;
            break;
        default:
            cout << "Error" << endl;
            break;
    }
}

Coordenada_t coordenadaAleatoria(){
    return {rand() % SIZE, rand() % SIZE};
}

bool coordenadaLibre(Tablero_t tablero, Coordenada_t posicion){
    return !estaEnArray(tablero.jugadores[0].minas, tablero.jugadores[0].minasRestantes, posicion) &&
            !estaEnArray(tablero.jugadores[1].minas, tablero.jugadores[1].minasRestantes, posicion) &&
            !estaEnArray(tablero.jugadores[0].soldados, tablero.jugadores[0].soldadosRestantes, posicion) &&
            !estaEnArray(tablero.jugadores[1].soldados, tablero.jugadores[1].soldadosRestantes, posicion);
}

Coordenada_t coordenadaLibreAleatoria(Tablero_t tablero){
    Coordenada_t posicion = coordenadaAleatoria();
    while(!coordenadaLibre(tablero, posicion)){
        posicion = coordenadaAleatoria();
    }
    return posicion;
}

void preguntarCoordenada(Tablero_t tablero, Coordenada_t* posicion){
    cout << "Ingrese las 2 coordenadas en el siguiente formato -> x y: ";
    cin >> posicion->x >> posicion->y;
    while(!estaEnRango(posicion->x, posicion->y) || !coordenadaLibre(tablero, *posicion)){
        cout << "Por favor ingrese coordenadas validas" << endl;
        cout << (estaEnRango(posicion->x, posicion->y)? "": "< Fuera de rango >") << endl;
        cout << (coordenadaLibre(tablero, *posicion)? "": "< Coordenada ocupada >") << endl;
        cin >> posicion->x >> posicion->y;
    }
}

void cargaAutomatica(Tablero_t* tablero){
    for(int i = 0; i < JUGADORES; i++){
        for(int j = 0; j < tablero->jugadores[i].minasRestantes; j++){
            tablero->jugadores[i].minas[j] = coordenadaLibreAleatoria(*tablero);
        }
        for(int j = 0; j < tablero->jugadores[i].soldadosRestantes; j++){
            tablero->jugadores[i].soldados[j] = coordenadaLibreAleatoria(*tablero);
        }
    }
}

void cargaManual(Tablero_t* tablero){
    for(int i = 0; i < JUGADORES; i++){
        for(int j = 0; j < tablero->jugadores[i].minasRestantes; j++){
            system("clear");
            cout << "Jugador " << i+1 << endl;
            preguntarCoordenada(*tablero, &tablero->jugadores[i].minas[j]);
        }
        for(int j = 0; j < tablero->jugadores[i].soldadosRestantes; j++){
            system("clear");
            cout << "Jugador " << i+1 << endl;
            preguntarCoordenada(*tablero, &tablero->jugadores[i].soldados[j]);
        }
    }
}

void cargarJuego(Tablero_t* tablero, bool automatico){
    tablero->estado = 0;
    tablero->turno = 1;
    tablero->cantidadInactivos = 0;
    if(automatico){
        cargaAutomatica(tablero);
    }else{
        cargaManual(tablero);
    }   
}

void mostrarTablero(Tablero_t tablero){
    FILE* file = fopen(FILE_NAME[0].c_str(), "w");
    FILE* file2 = fopen(FILE_NAME[1].c_str(), "w");
    FILE* fileActual;
    char equipoSoldado;

    for(int i = 0; i < JUGADORES; i++){
        fileActual = i == 0? file : file2;
        
        for(int y = SIZE-1; y >= 0; y--){
            fprintf(fileActual, "| ");
            for(int x = 0; x < SIZE; x++){
                if(estaEnArray(tablero.jugadores[i].minas, tablero.jugadores[i].minasRestantes, {x, y})){
                    fprintf(fileActual, "M ");
                }else if(estaEnArray(tablero.jugadores[i].soldados, tablero.jugadores[i].soldadosRestantes, {x, y})){
                    equipoSoldado = (i == 0)? '1' : '2';
                    fprintf(fileActual, "%c ", equipoSoldado);
                }else if(estaEnArray(tablero.jugadores[1-i].soldados, tablero.jugadores[1-i].soldadosRestantes, {x, y})){
                    equipoSoldado = (i == 0)? '2' : '1';
                    fprintf(fileActual, "%c ", equipoSoldado);
                }else{
                    fprintf(fileActual, "  ");
                }
            }
            fprintf(fileActual, "|\n");
        }
        fprintf(fileActual, "|_________________________________________|\n");
        fprintf(fileActual, "Jugador %s:\n", tablero.jugadores[i].nombre.c_str());
        fprintf(fileActual, "Minas restantes: %i\n", tablero.jugadores[i].minasRestantes);
        fprintf(fileActual, "Soldados restantes: %i\n\n", tablero.jugadores[i].soldadosRestantes);
    }
    fclose(file);
    fclose(file2);
}

void comprobarEstado(Jugador_t jug1, Jugador_t jug2, int* estado){
    if(jug1.soldadosRestantes == 0 && jug2.soldadosRestantes == 0){
        *estado = 3;
    }else if(jug1.soldadosRestantes == 0){
        *estado = 2;
    }else if(jug2.soldadosRestantes == 0){
        *estado = 1;
    } else {
        *estado = 0;
    }
}

int seleccionarSoldado(Jugador_t jugador){
    int resultado;
    cout << "Seleccione el soldado que desea mover: ";
    cout << "Sus soldados: " << endl;
    for(int i = 0; i < jugador.soldadosRestantes; i++){
        cout << i+1 << " - " << jugador.soldados[i].x << " " << jugador.soldados[i].y << endl;
    }
    cin >> resultado;
    cout << endl;
    while(resultado < 1 || resultado > jugador.soldadosRestantes){
        cout << "Por favor ingrese un numero valido: ";
        cin >> resultado;
        cout << endl;
    }
    return resultado;
}

void obtenerDireccion(string* direccion){
    cin >> *direccion;
    cout << endl;
    while(*direccion != "W" && *direccion != "A" && *direccion != "S" && *direccion != "D" && *direccion != "WA" && *direccion != "WD" && *direccion != "SA" && *direccion != "SD"){
        cout << "Por favor ingrese una direccion valida: ";
        cin >> *direccion;
        cout << endl;
    }
}

Coordenada_t obtenerCoordenadaNueva(Coordenada_t soldado, string direccion){
    auto it = coordenadas_nuevas.find(direccion);
    return it != coordenadas_nuevas.end() ? Coordenada_t{soldado.x + it->second.x, soldado.y - it->second.y} : soldado;
}

bool colisionaConInactivo(Coordenada_t soldado, CasilleroInactivo_t inactivos[MAX_MINAS], int inactivosRestantes){
    for(int i = 0; i < inactivosRestantes; i++){
        if(inactivos[i].posicion.x == soldado.x && inactivos[i].posicion.y == soldado.y){
            return true;
        }
    }
    return false;
}

bool coordenadaMovible(Tablero_t* tablero, Coordenada_t posicion, int jugador){
    bool resultado = true;
    for(int i = 0; i < JUGADORES; i++){
        if(!estaEnRango(posicion.x, posicion.y) || 
        estaEnArray(tablero->jugadores[i].minas, tablero->jugadores[i].minasRestantes, posicion) ||
        colisionaConInactivo(posicion, tablero->casillerosInactivos, tablero->cantidadInactivos) ||
        estaEnArray(tablero->jugadores[i].soldados, tablero->jugadores[i].soldadosRestantes, posicion)) {
            cout << "Coordenada Invalida" << endl;
            resultado = false;
        }
    }
    return resultado;
}

void copiarCoordenada(Coordenada_t* destino, Coordenada_t fuente){
    destino->x = fuente.x;
    destino->y = fuente.y;
}

void actualizarCasillerosInactivos(Coordenada_t fuente, Tablero_t* tablero){
    copiarCoordenada(&tablero->casillerosInactivos[tablero->cantidadInactivos].posicion, fuente);
    tablero->casillerosInactivos[tablero->cantidadInactivos].turnosRestantes = 5;
    tablero->cantidadInactivos++;
}

void swapCoordenada(int indice, int tope, Coordenada_t* posiciones){
    Coordenada_t aux;
    copiarCoordenada(&aux, posiciones[indice]);
    copiarCoordenada(&posiciones[indice], posiciones[tope]);
    copiarCoordenada(&posiciones[tope], aux);
}

void eliminarSoldado(Coordenada_t soldados[MAX_SOLDADOS], int* soldadosRestantes, int soldado){
    soldados[soldado].x = -1;
    soldados[soldado].y = -1;
    swapCoordenada(soldado, *soldadosRestantes, soldados);
    (*soldadosRestantes)--;
}

int findCoordenadaMatch(Coordenada_t soldados[MAX_SOLDADOS], int soldadosRestantes, Coordenada_t posicion){
    int resultado = -1;
    for(int i = 0; i < soldadosRestantes; i++){
        if(soldados[i].x == posicion.x && soldados[i].y == posicion.y){
            resultado = i;
        }
    }
    return resultado;
}

void efectuarColision(Tablero_t* tablero, int jugador, int soldado){
    Coordenada_t posicion = tablero->jugadores[jugador].soldados[soldado];
    if(estaEnArray(tablero->jugadores[jugador].minas, tablero->jugadores[jugador].minasRestantes, posicion)){
        actualizarCasillerosInactivos(posicion, tablero);
        tablero->jugadores[jugador].minasRestantes--;
        eliminarSoldado(tablero->jugadores[jugador].soldados, &tablero->jugadores[jugador].soldadosRestantes, soldado);
    } else if (estaEnArray(tablero->jugadores[1-jugador].soldados, tablero->jugadores[1-jugador].soldadosRestantes, posicion)){
        tablero->jugadores[1-jugador].soldadosRestantes--;
        eliminarSoldado(tablero->jugadores[1-jugador].soldados, &tablero->jugadores[1-jugador].soldadosRestantes, findCoordenadaMatch(tablero->jugadores[1-jugador].soldados, tablero->jugadores[1-jugador].soldadosRestantes, posicion));
        eliminarSoldado(tablero->jugadores[jugador].soldados, &tablero->jugadores[jugador].soldadosRestantes, soldado);
    }
}

void comprobarInactivas(CasilleroInactivo_t* inactivos, int* cantidadInactivos){
    CasilleroInactivo aux;
    for(int i = 0; i < *cantidadInactivos; i++){
        if(inactivos[i].turnosRestantes == 0){
            inactivos[i].posicion = {-1, -1};
            aux = inactivos[(*cantidadInactivos)-1];
            inactivos[(*cantidadInactivos)-1] = inactivos[i];
            inactivos[i] = aux;
            (*cantidadInactivos)--;
        } else {
            inactivos[i].turnosRestantes--;
        }
    }
}

void moverSoldado(Tablero_t* tablero, int jugador, int soldadoAMover){
    string direccion;
    Coordenada_t aux;
    cout << "Seleccione la direccion a la que desea moverlo (W A S D W-A W-D S-A S-D): " << endl;
    do{
        obtenerDireccion(&direccion);
        aux = obtenerCoordenadaNueva(tablero->jugadores[jugador].soldados[soldadoAMover], direccion);
    } while(!coordenadaMovible(tablero, aux, jugador));
    copiarCoordenada(&tablero->jugadores[jugador].soldados[soldadoAMover], aux);
}

void turnoJugador(Tablero_t* tablero, int jugador){
    system("clear");
    cout << "Turno del jugador " << jugador+1 << endl;
    int soldadoAMover = seleccionarSoldado(tablero->jugadores[jugador]) -1;
    moverSoldado(tablero, jugador, soldadoAMover);
    efectuarColision(tablero, jugador, soldadoAMover);
    comprobarInactivas(tablero->casillerosInactivos, &tablero->cantidadInactivos);
}

void jugar(Tablero_t* tablero){
    mostrarTablero(*tablero);
    while(tablero->estado == 0){
        comprobarEstado(tablero->jugadores[0], tablero->jugadores[1], &tablero->estado);
        if(tablero->estado != 0){
            break;
        }
        turnoJugador(tablero, 0);
        turnoJugador(tablero, 1);
        mostrarTablero(*tablero);
        tablero->turno++;
    }
}

void menuBienvenida(){
    cout << "Bienvenido al juego de la BATALLA DIGITAL V1.0" << endl;
    cout << "-----------------------------------" << endl;
    cout << "Las reglas son simples: " << endl;
    cout << "1. Cada jugador tiene 5 soldados y un número prefijado de minas" << endl;
    cout << "2. Pueden elegir entre ingresarlas manualmente o automaticamente" << endl;
    cout << "3. Los soldados se mueven en las 8 direcciones posibles" << endl;
    cout << "4. Si un soldado se encuentra con una mina, se inactiva el casillero, y muere" << endl;
    cout << "5. Si un soldado se encuentra con otro soldado, ambos mueren" << endl;
    cout << "6. El juego termina cuando un jugador no tiene soldados" << endl;
    cout << "-----------------------------------" << endl;
    cout << "Presione enter para continuar" << endl;
    cin.get();
}

int main() {
    srand((unsigned int)time(NULL));
    Tablero_t tablero;
    bool automatico = false;
    menuBienvenida();
    iniciarParametros(&tablero, &automatico);
    cargarJuego(&tablero, automatico);
    jugar(&tablero);
    mensajeFinal(tablero.estado, tablero.jugadores[0].nombre, tablero.jugadores[1].nombre);
    return 0;
}