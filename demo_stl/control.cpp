#include<iostream>
#include<fstream>
#include<execution>
#include<numeric>
#include <vector>


#include "BenchmarkUtils.hpp"
#include "DataQueue.hpp"
#include "Consumer.hpp"

size_t getSizeWithoutReadAll(const std::string& filePath) {
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return 0;
    }
    // Obtener una linea y procesar con un hilo paralelo 
    size_t totalSize = 0;
    std::string line;
    Persona persona;
    while (std::getline(inputFile, line)) {
        persona.fromString(line);
        totalSize += getSizeofPersona(persona);
    }
    return totalSize;

}

size_t getSizeWithReadAllBlock(const std::string& filePath) {
    const size_t BLOCK = 10000;
    std::vector<Persona> personas;
    personas.reserve(BLOCK);
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return 0;
    }
    size_t totalSize = 0;
    std::string line;
    while (true){
        personas.clear();

        
        while (personas.size() < BLOCK && std::getline(inputFile, line)){
            Persona p;
            p.fromString(line);
            personas.push_back(std::move(p));
        }

        if (personas.empty())
            break;
        //    break;

        size_t partial = std::transform_reduce(
            std::execution::par,
            personas.begin(),
            personas.end(),
            size_t{0},
            std::plus<>(),
            [](const Persona& p)
            {
                return getSizeofPersona(p);
            });
            totalSize += partial;
    }
    return totalSize;
}
size_t getSizeProductoConsumer(const std::string& filePath) {
    
    BlockingQueue<std::vector<std::string>> queue;
    //Usar atomic despues,, usaremos solo el hilo consumidor para acumular el total
    std::vector<std::thread> workers;
    std::vector<size_t> localTotals(kNumThreads, 0);
    for(int i=0; i<kNumThreads; ++i){
        workers.emplace_back([&queue, &localTotals, i](){
            std::vector<std::string> lines;
            size_t localTotal = 0;
            while(queue.pop(lines)){
                for(const auto& line : lines){
                    Persona persona;
                    persona.fromString(line);
                    size_t size = getSizeofPersona(persona);
                    //Acumular el tamaño en una variable compartida
                    //Usar atomic o mutex para proteger la variable compartida
                    localTotal += size;
                }

            }
            localTotals[i] = localTotal;
        });
    }

    // Obtener una linea y procesar con un hilo paralelo 
    
    std::thread producer([&queue, &filePath](){
        std::ifstream inputFile(filePath);
        if (!inputFile.is_open()) {
            std::cerr << "Error opening file: " << filePath << std::endl;
            queue.finish();
            return;
        }
        std::string line;
        const size_t BLOCK = 10000;
        std::vector<std::string> block;
        // Reservar espacio para el bloque
        block.reserve(BLOCK);
        while (std::getline(inputFile, line)) {
            
            block.push_back(std::move(line));
            while (block.size() < BLOCK && std::getline(inputFile, line)) {
                block.push_back(std::move(line));
            }
            queue.push(std::move(block));
            block.clear();
        }

        queue.finish();
        std::cout << "Producer finished" << std::endl;
    });

    producer.join();
    for(auto& worker : workers){
        worker.join();
    }
    size_t totalSize = 0;
    for(size_t localTotal : localTotals){
        totalSize += localTotal;
    }

    return totalSize;

    /*
    Producer finished (Error del diseño el productor hace parseo)
    getSizeWithoutReadAll demoro: 2795 ms
    Total size: 2179999655 Size(MB) : 2079
    getSizeWithReadAllBlock demoro: 4626 ms
    Total size: 1891736660 Size(MB) : 1804
    getSizeProductoConsumer demoro: 41739 ms
    Total size: 1904565678 Size(MB) : 1816
    ************************
    Producer finished // Lento por los mutex
    getSizeWithoutReadAll demoro: 2890 ms
    Total size: 2179999655 Size(MB) : 2079
    getSizeWithReadAllBlock demoro: 4716 ms
    Total size: 1891736660 Size(MB) : 1804
    getSizeProductoConsumer demoro: 56611 ms
    Total size: 1891736660 Size(MB) : 1804
    */
}

size_t getSizeProductoConsumerV2(const std::string& filePath) {
    // Creamos un arreglo de colas, UNA PARA CADA HILO
    std::vector<SPSCQueue<std::string>> queues(kNumThreads);
    
    std::vector<std::thread> workers;
    std::vector<size_t> localTotals(kNumThreads, 0);

    // 1. INICIAR CONSUMIDORES
    for (int i = 0; i < kNumThreads; ++i) {
        workers.emplace_back([&queues, &localTotals, i]() {
            std::string line;
            size_t total = 0;
            
       
            while (true) {
                if (queues[i].pop(line)) {
                    Persona persona;
                    persona.fromString(line);
                    total += getSizeofPersona(persona);
                } else {
                    if (queues[i].is_finished()) {
                        break; 
                    }
                    // "Relajamos" el CPU para no quemar el procesador esperando
                    std::this_thread::yield(); 
                }
            }
            localTotals[i] = total;
        });
    }

    // 2. INICIAR PRODUCTOR
    std::thread producer([&queues, &filePath]() {
        std::ifstream inputFile(filePath);
        if (!inputFile.is_open()) return;

        std::string line;
        int target_queue = 0;

        while (std::getline(inputFile, line)) {
            while (!queues[target_queue].push(std::move(line))) {
                std::this_thread::yield();
            }
            
            // Pasamos a la cola del siguiente hilo
            target_queue = (target_queue + 1) % kNumThreads;
        }

        // Avisamos a TODAS las colas que hemos terminado
        for (auto& q : queues) {
            q.set_finished();
        }
    });

    // 3. CONSOLIDAR
    producer.join();
    for (auto& worker : workers) worker.join();

    size_t totalSize = 0;
    for (size_t val : localTotals) totalSize += val;

    return totalSize;
}


int main(){
    std::ifstream inputFile("personas.txt");
    auto [totalSize, elapsedTime] = medirTiempoMs(getSizeWithoutReadAll, "personas.txt");
    auto [totalSize2, elapsedTime2] = medirTiempoMs(getSizeWithReadAllBlock, "personas.txt");
    auto [totalSize3, elapsedTime3] = medirTiempoMs(getSizeProductoConsumer, "personas.txt");
    auto [totalSize4, elapsedTime4] = medirTiempoMs(getSizeProductoConsumerV2, "personas.txt");
    
    printResultParallel("getSizeWithoutReadAll", totalSize, elapsedTime);
    printResultParallel("getSizeWithReadAllBlock", totalSize2, elapsedTime2);
    printResultParallel("getSizeProductoConsumer", totalSize3, elapsedTime3);
    printResultParallel("getSizeProductoConsumerV2", totalSize4, elapsedTime4);
    return 0;

}
