#include <iostream>
#include <tbb/global_control.h>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <execution>
#include <numeric>

#include "BenchmarkUtils.hpp"
using namespace std;
std::vector<Persona> cargarPersonas(const std::string& filePath) {
    std::ifstream inputFile(filePath);
    std::vector<Persona> personas;
    for (Persona p; inputFile >> p;) {
        personas.push_back(p);
    }
    return personas;
}

size_t calcularTamanioTotal(const std::vector<Persona>& personas) {
    size_t totalSize = 0;
    for (const auto& p : personas) {
        totalSize += getSizeofPersona(p);
    }
    return totalSize;
}

size_t calcularTamanioTotalParalelo(const std::vector<Persona>& personas) {
    return std::transform_reduce(std::execution::par, personas.begin(), personas.end(), size_t(0), std::plus<>(), getSizeofPersona);
}

size_t calcularTamanioTotalParalelo2(const std::vector<Persona>& personas) {
    size_t totalSize = 0;
    std::mutex mtx;
    std::for_each(std::execution::par, personas.begin(), personas.end(), [&](const Persona& p) {
        size_t size = getSizeofPersona(p);
        std::lock_guard<std::mutex> lock(mtx);
        totalSize += size;
    });
    return totalSize;
}

size_t calcularTamanioTotalParalelo2_5(const std::vector<Persona>& personas) {
    size_t totalSize = 0;
    std::mutex mtx;
    std::atomic<int> indiceActual{0};

    auto threadFunc = [&](int) {
        size_t localTotal = 0;
        while (true) {
            int index = indiceActual.fetch_add(1);
            if (index >= static_cast<int>(personas.size())) {
                break;
            }
            localTotal += getSizeofPersona(personas[index]);
        }
        std::lock_guard<std::mutex> lock(mtx);
        totalSize += localTotal;
    };

    std::vector<std::thread> threadPool;
    threadPool.reserve(kNumThreads);
    for (unsigned int i = 0; i < kNumThreads; ++i) {
        threadPool.emplace_back(threadFunc, i);
    }
    for (auto& t : threadPool) {
        t.join();
    }

    return totalSize;
}

size_t calcularTamanioTotalParalelo3(const std::vector<Persona>& personas) {
    size_t totalSize = 0;
    std::mutex mtx;
    std::vector<std::thread> threads;
    threads.reserve(kNumThreads);

    const size_t blockSize = (personas.size() + kNumThreads - 1) / kNumThreads;
    for (unsigned int i = 0; i < kNumThreads; ++i) {
        threads.emplace_back([&, i] {
            size_t localTotal = 0;
            size_t start = i * blockSize;
            size_t end = std::min(start + blockSize, personas.size());
            for (size_t j = start; j < end; ++j) {
                localTotal += getSizeofPersona(personas[j]);
            }
            std::lock_guard<std::mutex> lock(mtx);
            totalSize += localTotal;
        });
    }

    for (auto& t : threads) {
        t.join();
    }
    return totalSize;
}

size_t calcularTamanioTotalParalelo4(const std::vector<Persona>& personas) {
    size_t totalSize = 0;
    std::vector<std::thread> threads;
    threads.reserve(kNumThreads);

    std::vector<size_t> localTotals(kNumThreads, 0);
    const size_t blockSize = (personas.size() + kNumThreads - 1) / kNumThreads;

    for (unsigned int i = 0; i < kNumThreads; ++i) {
        threads.emplace_back([&, i] {
            size_t localTotal = 0;
            size_t start = i * blockSize;
            size_t end = std::min(start + blockSize, personas.size());
            for (size_t j = start; j < end; ++j) {
                localTotal += getSizeofPersona(personas[j]);
            }
            localTotals[i] = localTotal;
        });
    }

    for (auto& t : threads) {
        t.join();
    }
    for (const auto& localTotal : localTotals) {
        totalSize += localTotal;
    }
    return totalSize;
}
int main(){
    //Configurando numero de hilos es innecesario, ya que la libreria lo hace
    int num_hilos = kNumThreads;
    tbb::global_control limit(tbb::global_control::max_allowed_parallelism, num_hilos);

    auto [personas, cargaMs] = medirTiempoMs(cargarPersonas, string("personas.txt"));
    auto [totalSize, calculoMs] = medirTiempoMs(calcularTamanioTotal, personas);
    auto [totalSizeParalelo, calculoParaleloMs] = medirTiempoMs(calcularTamanioTotalParalelo, personas);
    auto [totalSizeParalelo2, calculoParalelo2Ms] = medirTiempoMs(calcularTamanioTotalParalelo2, personas);
    auto [totalSizeParalelo2_5, calculoParalelo2_5Ms] = medirTiempoMs(calcularTamanioTotalParalelo2_5, personas);
    auto [totalSizeParalelo3, calculoParalelo3Ms] = medirTiempoMs(calcularTamanioTotalParalelo3, personas);
    auto [totalSizeParalelo4, calculoParalelo4Ms] = medirTiempoMs(calcularTamanioTotalParalelo4, personas);

    cout << "Carga demoro: " << cargaMs << " ms" << endl;
    cout << "Calculo demoro: " << calculoMs << " ms" << endl;
    cout << "Total size: " << totalSize << " Size(MB) : "<< totalSize / (1024 * 1024) << endl;
    cout << "Calculo paralelo demoro: " << calculoParaleloMs << " ms" << endl;
    
    
    printResultParallel("Calculo paralelo", totalSizeParalelo, calculoParaleloMs);
    printResultParallel("Calculo paralelo 2", totalSizeParalelo2, calculoParalelo2Ms);
    printResultParallel("Calculo paralelo 2.5", totalSizeParalelo2_5, calculoParalelo2_5Ms);
    printResultParallel("Calculo paralelo 3", totalSizeParalelo3, calculoParalelo3Ms);
    printResultParallel("Calculo paralelo 4", totalSizeParalelo4, calculoParalelo4Ms);
    /*
    RESULTADO DE EJECUCION:
    Carga demora: 6891 ms
    Calculo demoro: 339 ms
    Total size: 1881030314 Size(MB) : 1793
    Calculo paralelo demoro: 57 ms
    Calculo paralelo demoro: 57 ms
    Total size: 1881030314 Size(MB) : 1793
    Calculo paralelo 2 demoro: 760 ms
    Total size: 1881030314 Size(MB) : 1793
    Calculo paralelo 2.5 demoro: 419 ms
    Total size: 1881030314 Size(MB) : 1793
    Calculo paralelo 3 demoro: 60 ms
    Total size: 1881030314 Size(MB) : 1793
    Calculo paralelo 4 demoro: 58 ms
    Total size: 1881030314 Size(MB) : 1793
    */

}