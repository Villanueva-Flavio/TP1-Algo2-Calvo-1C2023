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
// Toma de notas

// Si la mina explota, se inactiva por 5 turnos
// Soldado -> Muere si pisa mina
// Si 2 soldados chocan ambos mueren
// El jugador luego de dejar la mina, puede optar por
//    mover un soldado Horiz, Vert, Diag


typedef struct Coordenada{
    int x;
    int y;
} Coordenada_t;

typedef struct Jugador{
    string nombre;
    Coordenada_t minas[MAX_MINAS];
    Coordenada_t soldados[MAX_SOLDADOS];
    int soldadosRestantes;
    int minasRestantes;
} Jugador_t;

typedef struct Tablero{
    Coordenada_t casillerosInactivos[MAX_MINAS];
    Jugador_t jugador1;
    Jugador_t jugador2;
    int turno;
    int estado;
    int cantidadInactivos;
}Tablero_t;

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

void preguntarNombre(string* nombre){
    cout << "Ingrese el nombre del jugador, maximo 10 caracteres: ";
    cin >> *nombre;
    while(nombre->length() > 10){
        cout << "Por favor no exceda los 10 caracteres: ";
        cin >> *nombre;
    }
}

void preguntarCantidadMinas(int* minas){
    cout << "Ingrese la cantidad de minas (MAX: %i): " << MAX_MINAS << endl;
    cin >> *minas;
    while (*minas > MAX_MINAS || *minas < 1) {
        cout << "Ingrese la cantidad de minas: ";
        cin >> *minas;
    }
}

void preguntarMetodoInsercion(bool* metodo){
    cout << "Quiere ingresar las minas y los soldados automaticamente? Y/N" << endl;
    char respuesta;
    cin >> respuesta;
    while(respuesta != 'Y' && respuesta != 'y' && respuesta != 'N' && respuesta != 'n'){
        cout << "Por favor ingrese Y o N: ";
        cin >> respuesta;
    }
    *metodo = (respuesta =='Y' || respuesta == 'y')? true : false;    
}

void iniciarParametros(Tablero_t* tablero, bool* automatico){
    preguntarNombre(&tablero->jugador1.nombre);
    preguntarNombre(&tablero->jugador2.nombre);
    preguntarCantidadMinas(&tablero->jugador1.minasRestantes);
    tablero->jugador2.minasRestantes = tablero->jugador1.minasRestantes;
    preguntarMetodoInsercion(automatico);
}

void mensajeFinal(int estado){
    switch (estado) {
        case 1:
            cout << "Gano el jugador 1" << endl;
            break;
        case 2:
            cout << "Gano el jugador 2" << endl;
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
    return !estaEnArray(tablero.jugador1.minas, tablero.jugador1.minasRestantes, posicion) &&
            !estaEnArray(tablero.jugador2.minas, tablero.jugador2.minasRestantes, posicion) &&
            !estaEnArray(tablero.jugador1.soldados, tablero.jugador1.soldadosRestantes, posicion) &&
            !estaEnArray(tablero.jugador2.soldados, tablero.jugador2.soldadosRestantes, posicion);
}

Coordenada_t coordenadaLibreAleatoria(Tablero_t tablero){
    Coordenada_t posicion = coordenadaAleatoria();
    while(!coordenadaLibre(tablero, posicion)){
        posicion = coordenadaAleatoria();
    }
    return posicion;
}

// Considerar coordenada libre y que este in range
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
    for(int i = 0; i < tablero->jugador1.minasRestantes; i++){
            tablero->jugador1.minas[i] = coordenadaLibreAleatoria(*tablero);
            tablero->jugador1.minasRestantes ++;
            tablero->jugador2.minas[i] = coordenadaLibreAleatoria(*tablero);
            tablero->jugador2.minasRestantes ++;
        }
        for(int i = 0; i < MAX_SOLDADOS; i++){
            tablero->jugador1.soldados[i] = coordenadaLibreAleatoria(*tablero);
            tablero->jugador1.soldadosRestantes ++;
            tablero->jugador2.soldados[i] = coordenadaLibreAleatoria(*tablero);
            tablero->jugador2.soldadosRestantes ++;
        }
}

void cargaManual(Tablero_t* tablero){
    for(int i = 0; i < tablero->jugador1.minasRestantes; i++){
        system("clear");
        cout << "Jugador 1" << endl;
        preguntarCoordenada(*tablero, &tablero->jugador1.minas[i]);
        tablero->jugador1.minas[i] = coordenadaLibreAleatoria(*tablero);
        system("clear");
        cout << "Jugador 2" << endl;
        preguntarCoordenada(*tablero, &tablero->jugador2.minas[i]);
        tablero->jugador2.minas[i] = coordenadaLibreAleatoria(*tablero);
    }
    for(int i = 0; i < MAX_SOLDADOS; i++){
        system("clear");
        cout << "Jugador 1" << endl;
        preguntarCoordenada(*tablero, &tablero->jugador1.soldados[i]);
        tablero->jugador1.soldados[i] = coordenadaLibreAleatoria(*tablero);
        system("clear");
        cout << "Jugador 2" << endl;
        preguntarCoordenada(*tablero, &tablero->jugador2.soldados[i]);
        tablero->jugador2.soldados[i] = coordenadaLibreAleatoria(*tablero);
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
    /*
        Jugador <NOMBRE>:
        Minas restantes: <MINAS>
        Soldados restantes: <SOLDADOS>

        <Tablero con todos los soldados y mis minas>
    */

    FILE* file = fopen(FILE_NAME[0].c_str(), "w");
    FILE* file2 = fopen(FILE_NAME[1].c_str(), "w");
    fprintf(file, "Jugador %s:\n", tablero.turno == 1? tablero.jugador1.nombre.c_str() : tablero.jugador2.nombre.c_str());
    fprintf(file, "Minas restantes: %i\n", tablero.jugador1.minasRestantes - tablero.cantidadInactivos);
    fprintf(file, "Soldados restantes: %i\n\n", MAX_SOLDADOS - tablero.cantidadInactivos);
    fprintf(file2, "Jugador %s:\n", tablero.turno == 1? tablero.jugador2.nombre.c_str() : tablero.jugador1.nombre.c_str());
    fprintf(file2, "Minas restantes: %i\n", tablero.jugador2.minasRestantes - tablero.cantidadInactivos);
    fprintf(file2, "Soldados restantes: %i\n\n", MAX_SOLDADOS - tablero.cantidadInactivos);
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            if(estaEnArray(tablero.jugador1.minas, tablero.jugador1.minasRestantes, {i, j})){
                fprintf(file, "M ");
            } else if (estaEnArray(tablero.jugador2.minas, tablero.jugador2.minasRestantes, {i, j})){
                fprintf(file2, "M ");
            } else if (estaEnArray(tablero.jugador1.soldados, MAX_SOLDADOS, {i, j})){
                fprintf(file, "S ");
            } else if (estaEnArray(tablero.jugador2.soldados, MAX_SOLDADOS, {i, j})){ 
                fprintf(file2, "S ");
            } else {
                fprintf(file, "  ");
                fprintf(file2, "  ");
            }
        }
        fprintf(file, "\n");
        fprintf(file2, "\n");
    }

    fclose(file);
    fclose(file2);
}

void comprobarEstado(Coordenada_t soldados1[MAX_SOLDADOS], Coordenada_t soldados2[MAX_SOLDADOS], int* estado){
    if(estaEnArray(soldados1, MAX_SOLDADOS, soldados1[0]) && estaEnArray(soldados2, MAX_SOLDADOS, soldados2[0])){
        *estado = 3;
    } else if (estaEnArray(soldados1, MAX_SOLDADOS, soldados1[0])){
        *estado = 2;
    } else if (estaEnArray(soldados2, MAX_SOLDADOS, soldados2[0])){
        *estado = 1;
    } else {
        *estado = 0;
    }
}

int seleccionarSoldado(Jugador_t jugador){
    cout << "Seleccione el soldado que desea mover: ";
    int resultado;
    cin >> resultado;
    while(resultado < 1 || resultado > jugador.soldadosRestantes){
        cout << "Por favor ingrese un numero valido: ";
        cin >> resultado;
    }
    return resultado;
}

void obtenerDireccion(string* direccion){
    cin >> *direccion;
    while(*direccion != "W" && *direccion != "A" && *direccion != "S" && *direccion != "D" && *direccion != "WA" && *direccion != "WD" && *direccion != "SA" && *direccion != "SD"){
        cout << "Por favor ingrese una direccion valida: ";
        cin >> *direccion;
    }
}

Coordenada_t obtenerCoordenadaNueva(Coordenada_t soldado, string direccion){
    auto it = coordenadas_nuevas.find(direccion);
    return it != coordenadas_nuevas.end() ? Coordenada_t{soldado.x + it->second.x, soldado.y + it->second.y} : soldado;
}

bool colisionaConMina(Coordenada_t soldado, Coordenada_t minas[MAX_MINAS], int minasRestantes){
    return estaEnArray(minas, minasRestantes, soldado);
}

bool colisionaConSoldado(Coordenada_t soldado, Coordenada_t soldados[MAX_SOLDADOS]){
    return estaEnArray(soldados, MAX_SOLDADOS, soldado);
}

bool coordenadaMovible(Tablero_t* tablero, Coordenada_t posicion, int jugador){
    bool resultado = true;
    if((!estaEnRango(posicion.x, posicion.y)) ||
    ((jugador == 1) && (
      colisionaConMina(posicion, tablero->jugador1.minas, tablero->jugador1.minasRestantes) || 
      colisionaConSoldado(posicion, tablero->jugador1.soldados) || 
      colisionaConMina(posicion, tablero->casillerosInactivos, tablero->cantidadInactivos))) ||
    ((jugador == 2) && (
      colisionaConMina(posicion, tablero->jugador2.minas, tablero->jugador2.minasRestantes) || 
      colisionaConSoldado(posicion, tablero->jugador2.soldados) || 
      colisionaConMina(posicion, tablero->casillerosInactivos, tablero->cantidadInactivos)))) {

        cout << "Coordenada Invalida" << endl;
        resultado = false;
    }
    return resultado;
}

void copiarCoordenada(Coordenada_t* destino, Coordenada_t fuente){
    destino->x = fuente.x;
    destino->y = fuente.y;
}

void actualizarCasillerosInactivos(Tablero_t* tablero, Coordenada_t posicion){
    tablero->casillerosInactivos[tablero->cantidadInactivos] = posicion;
    tablero->cantidadInactivos++;
}

void efectuarColision(Tablero_t* tablero, int jugador, int soldado){

}

void moverSoldado(Tablero_t* tablero, int jugador, int soldadoAMover){
    string direccion;
    Coordenada_t aux;

    cout << "Seleccione la direccion a la que desea moverlo (W A S D W-A W-D S-A S-D): ";
    
    do{
        obtenerDireccion(&direccion);
        aux = obtenerCoordenadaNueva(tablero->jugador1.soldados[soldadoAMover], direccion);
    } while(!coordenadaMovible(tablero, aux, jugador));
    
    if(jugador == 1){
        copiarCoordenada(&tablero->jugador1.soldados[soldadoAMover], aux);
    } else {
        copiarCoordenada(&tablero->jugador2.soldados[soldadoAMover], aux);
    }
    //actualizarPosicion(tablero, jugador, soldadoAMover, aux);
    efectuarColision(tablero, jugador, soldadoAMover);
    // Chequear colision y efectuar acciones
    // Soldado - Soldado (Ambos mueren)
    // Soldado - Mina (Ambos desaparecen)
    //    Celda inactiva 5 turnos

}

void turnoJugador(Tablero_t* tablero, int jugador){
    //Preguntar a donde quiere moverlo
    //Comprobar si hay mina
    //Comprobar si hay soldado
    int soldadoAMover = (jugador == 1)? seleccionarSoldado(tablero->jugador1) -1 : seleccionarSoldado(tablero->jugador2) -1;
    moverSoldado(tablero, jugador, soldadoAMover);
}

void jugar(Tablero_t* tablero){
    mostrarTablero(*tablero);
    while(tablero->estado == 0){
        comprobarEstado(tablero->jugador1.soldados, tablero->jugador2.soldados, &tablero->estado);
        if(tablero->estado != 0)
            break;
        turnoJugador(tablero, 1);
        turnoJugador(tablero, 2);
        mostrarTablero(*tablero);
    }
}

int main() {
    srand(time(NULL));
    Tablero_t tablero;
    bool automatico = false;

    iniciarParametros(&tablero, &automatico);
    cargarJuego(&tablero, automatico);
    jugar(&tablero);
    mensajeFinal(tablero.estado);
    return 0;
}