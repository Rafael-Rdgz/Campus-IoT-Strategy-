/* ==============================================================
 * CAMPUS IoT - SISTEMA DE MONITOREO 
 * Elaborado por Rafael Rodriguez Botello Fierro 
 * Curso: Implementacion de Internet de las Cosas (IoT)
 * Compilar: g++ -std=c++17 -Wall -Wextra main.cpp -o nodos
 * Ejecutar: ./nodos
 * ============================================================= */
#include <iostream>  
#include <memory>    // unique_ptr y make_unique gestionan memoria automaticamente.
#include <string>    
using namespace std;

// DATO que sale del procesador y recibe el protocolo.
struct Resultado {
    string tipo; // Que sensor genero el resultado.
};

// CONTRATO abstracto: metodo virtual puro (= 0) obligatorio para cada procesador.
// Destructor virtual: necesario para destruir subclases mediante puntero base.
struct ProcesadorLectura {
    virtual ~ProcesadorLectura() = default;
    virtual Resultado procesar(double lectura) const = 0;
};
// Cada implementacion concreta encapsula un algoritmo distinto.
struct ProcesadorGas : ProcesadorLectura {
    Resultado procesar(double) const override { cout << "Estoy procesando gas\n"; return {};}
};
struct ProcesadorTemp : ProcesadorLectura {
    Resultado procesar(double) const override { cout << "Estoy procesando temperatura\n"; return {};}
};
struct ProcesadorPresencia : ProcesadorLectura {
    Resultado procesar(double) const override { cout << "Estoy procesando presencia\n"; return {};}
};
struct ProcesadorRuido : ProcesadorLectura {
    Resultado procesar(double) const override { cout << "Estoy procesando ruido\n"; return {};}
};

// SEGUNDA familia de estrategias: independiente del procesamiento.
struct ProtocoloEnvio {
    virtual ~ProtocoloEnvio() = default;
    virtual bool enviar(const Resultado& r) const = 0;
};
// Cada protocolo SIMULA el envio con un mensaje en consola.
struct ProtocoloMQTT : ProtocoloEnvio {
    bool enviar(const Resultado&) const override { cout << "Envio por MQTT\n"; return true;}
};
struct ProtocoloHTTP : ProtocoloEnvio {
    bool enviar(const Resultado&) const override { cout << "Envio por HTTP\n"; return true;}
};
struct ProtocoloBluetooth : ProtocoloEnvio {
    bool enviar(const Resultado&) const override { cout << "Envio por Bluetooth\n"; return true;}
};
struct ProtocoloLoRa : ProtocoloEnvio {
    bool enviar(const Resultado&) const override { cout << "Envio por LoRa\n"; return true;}
};

// CONTEXTO: el nodo POSEE estrategias, pero ignora sus clases concretas.
class NodoIoT {
    // unique_ptr expresa propiedad exclusiva y evita fugas de memoria.
    unique_ptr<ProcesadorLectura> procesador;
    unique_ptr<ProtocoloEnvio> protocolo;
public:
    NodoIoT(unique_ptr<ProcesadorLectura> proc, unique_ptr<ProtocoloEnvio> proto)
        : procesador(move(proc)), protocolo(move(proto)) {}
    void set_procesador(unique_ptr<ProcesadorLectura> p) { procesador = move(p);}                  // Cambia el algoritmo DURANTE la ejecucion.
    void set_protocolo(unique_ptr<ProtocoloEnvio> p) { protocolo = move(p);}                   // Cambia el protocolo DURANTE la ejecucion.
    bool realizar_lectura(double lectura) const {
        Resultado r = procesador->procesar(lectura); // DELEGACION al procesador.
        return protocolo->enviar(r);              // DELEGACION al protocolo.
    }
};

int main() {
    // Tres nodos con combinaciones distintas.
    NodoIoT laboratorio(make_unique<ProcesadorGas>(), make_unique<ProtocoloMQTT>());
    NodoIoT aula(make_unique<ProcesadorTemp>(), make_unique<ProtocoloHTTP>());
    NodoIoT pasillo(make_unique<ProcesadorPresencia>(), make_unique<ProtocoloBluetooth>());

    cout << "\n--- Laboratorio ---\n"; laboratorio.realizar_lectura(0.5);
    cout << "\n--- Aula ---\n";        aula.realizar_lectura(0.5);
    cout << "\n--- Pasillo ---\n";     pasillo.realizar_lectura(0.5);

    cout << "\n--- Cambio dinamico del Aula: HTTP -> LoRa ---\n";
    aula.set_protocolo(make_unique<ProtocoloLoRa>());
    aula.realizar_lectura(0.5);

    cout << "\n--- Cambio dinamico del Pasillo: Presencia -> Ruido ---\n";
    pasillo.set_procesador(make_unique<ProcesadorRuido>());
    pasillo.realizar_lectura(0.5);
}
