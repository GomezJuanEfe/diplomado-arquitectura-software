#include <iostream>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <numeric>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

// Uso: ./client [iteraciones] [calentamiento]
//   iteraciones : numero de round-trips a medir (default 10000)
//   calentamiento: round-trips previos que no se miden (default 200)
int main(int argc, char* argv[]) {
    const int ITERATIONS = (argc > 1) ? std::atoi(argv[1]) : 2000;
    const int WARMUP = (argc > 2) ? std::atoi(argv[2]) : 200;

    if (ITERATIONS <= 0) {
        std::cerr << "El numero de iteraciones debe ser positivo" << std::endl;
        return 1;
    }

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Error al crear socket" << std::endl;
        return 1;
    }

    // Timeout de recepcion: si el servidor no responde, no nos quedamos
    // bloqueados para siempre en una sola iteracion del benchmark.
    timeval timeout{};
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(9000);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    // Conectamos el socket UDP al destino fijo: el kernel cachea la ruta y
    // filtra paquetes de otros origenes, y podemos usar send()/recv() en
    // vez de sendto()/recvfrom() (un poco menos de trabajo por paquete).
    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Error al conectar el socket" << std::endl;
        close(sock);
        return 1;
    }

    const char msg[] = "estimulo";
    char buffer[1024]; // Buffer de 1024 para dejar espacio real al '\0'

    // --- Fase de calentamiento (no se mide) ---
    // Deja que el CPU salga de estados de ahorro de energia, se llenen
    // caches/TLBs y el runtime se estabilice antes de tomar mediciones.
    for (int i = 0; i < WARMUP; ++i) {
        if (send(sock, msg, strlen(msg), 0) < 0) continue;
        recv(sock, buffer, sizeof(buffer) - 1, 0);
    }

    // --- Fase de medicion: un round-trip por iteracion ---
    std::vector<long long> latencias_us;
    latencias_us.reserve(ITERATIONS);
    int perdidos = 0;

    for (int i = 0; i < ITERATIONS; ++i) {
        auto start = std::chrono::steady_clock::now();

        if (send(sock, msg, strlen(msg), 0) < 0) {
            ++perdidos;
            continue;
        }

        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        auto end = std::chrono::steady_clock::now();

        if (n < 0) {
            ++perdidos; // timeout o error: paquete perdido, se descarta la muestra
            continue;
        }

        buffer[n] = '\0';
        auto latencia = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        latencias_us.push_back(latencia);
    }

    close(sock);

    if (latencias_us.empty()) {
        std::cerr << "No se recibio ninguna respuesta valida (" << perdidos << " perdidas)" << std::endl;
        return 1;
    }

    std::sort(latencias_us.begin(), latencias_us.end());

    auto percentil = [&](double p) -> long long {
        size_t idx = static_cast<size_t>(p * (latencias_us.size() - 1));
        return latencias_us[idx];
    };

    long long minimo = latencias_us.front();
    long long maximo = latencias_us.back();
    long long mediana = percentil(0.50);
    long long p95 = percentil(0.95);
    long long p99 = percentil(0.99);
    double promedio = std::accumulate(latencias_us.begin(), latencias_us.end(), 0LL)
                       / (double)latencias_us.size();

    std::cout << "Ultima respuesta recibida: " << buffer << std::endl;
    std::cout << "\n--- Benchmark de latencia (RTT), " << latencias_us.size()
               << " muestras (" << perdidos << " perdidas de " << ITERATIONS << ") ---" << std::endl;
    std::cout << "Minimo   : " << minimo   << " us (" << minimo   / 1000.0 << " ms)" << std::endl;
    std::cout << "Mediana  : " << mediana  << " us (" << mediana  / 1000.0 << " ms)" << std::endl;
    std::cout << "Promedio : " << promedio << " us (" << promedio / 1000.0 << " ms)" << std::endl;
    std::cout << "p95      : " << p95      << " us (" << p95      / 1000.0 << " ms)" << std::endl;
    std::cout << "p99      : " << p99      << " us (" << p99      / 1000.0 << " ms)" << std::endl;
    std::cout << "Maximo   : " << maximo   << " us (" << maximo   / 1000.0 << " ms)" << std::endl;

    std::cout << "\nObjetivo (<1 ms en p99): "
               << ((p99 < 1000) ? "CUMPLIDO" : "NO CUMPLIDO") << std::endl;

    return 0;
}
