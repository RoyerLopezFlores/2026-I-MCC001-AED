#include "BenchmarkUtils.hpp"


#include <iostream>


size_t getSizeofPersona(const Persona& persona) {
    return sizeof(Persona) + persona.nombre.capacity() + persona.apellido.capacity() + persona.dni.capacity() + persona.fechaNacimiento.capacity();
}



void printResultParallel(const std::string& methodName, size_t totalSize, long long elapsedMs) {
    std::cout << methodName << " demoro: " << elapsedMs << " ms" << std::endl;
    std::cout << "Total size: " << totalSize << " Size(MB) : " << totalSize / (1024 * 1024) << std::endl;
}
