#include <iostream>
#include <chrono>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Error al crear socket" << std::endl;
        return 1;
    }

    sockaddr_in server{}, client{};
    server.sin_family = AF_INET;
    server.sin_port = htons(9000);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Error en bind" << std::endl;
        close(sock);
        return 1;
    }

    // Buffer de 1024 para dejar espacio real al terminador '\0' (1023 máximo)
    char buffer[1024];
    const char* reply = "respuesta";
    std::cout << "Servidor UDP de baja latencia escuchando en el puerto 9000..." << std::endl;

    unsigned long long conteo = 0;

    while (true) {
        socklen_t len = sizeof(client);

        // 1. Bloquearse esperando el estímulo (Fuera del cronómetro)
        int n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&client, &len);
        if (n < 0) continue;
        buffer[n] = '\0';

        // 2. Iniciar cronómetro SOLO para medir el tiempo de respuesta interno
        auto start = std::chrono::high_resolution_clock::now();

        // Enviar respuesta inmediatamente
        sendto(sock, reply, strlen(reply), 0, (sockaddr*)&client, len);

        auto end = std::chrono::high_resolution_clock::now();
        auto tiempo_interno = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        // Reportamos solo cada 1000 estimulos: con miles de iteraciones en
        // un benchmark, imprimir en cada una inundaria la terminal y anadiria
        // I/O innecesario al proceso (aunque ocurre despues de responder).
        ++conteo;
        if (conteo % 1000 == 0) {
            std::cout << "Estimulo #" << conteo << " - tiempo de respuesta interno: "
                       << tiempo_interno << " us" << std::endl;
        }
    }

    close(sock);
    return 0;
}
