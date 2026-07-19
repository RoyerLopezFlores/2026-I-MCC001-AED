#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <execution>
#include <numeric>
#include "Persona.hpp"


// std::inclusive_scan y std::exclusive_scan suma prefija y suma prefija exclusiva respectivamente.
/*
CategoríaMétodos principales
Búsqueda y Conteofind, find_if, count, count_if, any_of, all_of, none_of, search
Transformación y Modificacióntransform, replace, replace_if, fill, generate, reverse, swap_ranges
Filtrado y Limpiezacopy, copy_n, copy_if, remove, remove_if, uniqueOrdenamiento y Mezclasort, stable_sort, partial_sort, merge, is_sorted
Operaciones de Conjuntosincludes, set_union, set_intersection, set_difference
Matemáticas (<numeric>)reduce, transform_reduce, inclusive_scan, exclusive_scan, adjacent_difference
Iteración generalfor_each, for_each_n
*/
void useCopyIf(std::vector<Persona>& personas) {
    std::vector<Persona> filtered;
    std::copy_if(personas.begin(), personas.end(), std::back_inserter(filtered), [](const Persona& p) {
        return p.nombre == "Juan"; //Puede a ver regex
    });

    std::cout << "Filtered Personas with nombre 'Juan':" << std::endl;
    for (size_t i = 0; i < std::min(filtered.size(), size_t(10)); ++i) { // mostrar los primeros 10 elementos
        std::cout << filtered[i].nombre << std::endl;
    }

}
void useTransform(std::vector<Persona>& personas) {
    std::vector<std::string> nombres;
    nombres.reserve(personas.size());
    std::transform(personas.begin(), personas.end(), std::back_inserter(nombres), [](const Persona& p) {
        return p.nombre + "V"; // Agregar una V al final del nombre
    });

    std::cout << "Nombres of Personas:" << std::endl;
    for (size_t i = 0; i < std::min(nombres.size(), size_t(10)); ++i) { // mostrar los primeros 10 elementos
        std::cout << nombres[i] << std::endl;
    }
}

void useTransformReduce(std::vector<Persona>& personas) {
    size_t totalSize = std::transform_reduce(std::execution::par, personas.begin(), personas.end(), size_t(0), std::plus<>(), [](const Persona& p) {
        return sizeof(Persona) + p.nombre.capacity() + p.apellido.capacity() + p.dni.capacity() + p.fechaNacimiento.capacity();
    });

    std::cout << "Total size of Personas: " << totalSize << " bytes" << std::endl;
}
void useForEach(std::vector<Persona>& personas) {
    std::for_each(personas.begin(), personas.end(), [](const Persona& p) {
        std::cout << p.nombre << std::endl;
    });
}
void useFindIf(std::vector<Persona>& personas) {
    //find_if 
    //find: 

    auto it = std::find_if(std::execution::par, personas.begin(), personas.end(), [](const Persona& p) {
        return p.nombre == "Juan"; // Puede a ver regex
    });

    if (it != personas.end()) {
        std::cout << "Found Persona with nombre 'Juan': " << *it << std::endl;
    } else {
        std::cout << "No Persona found with nombre 'Juan'." << std::endl;
    }
}
void useSort(std::vector<Persona>& personas) {
    std::sort(std::execution::par, personas.begin(), personas.end(), [](const Persona& a, const Persona& b) {
        return a.nombre < b.nombre; // Ordenar por nombre
    });

    std::cout << "Personas sorted by nombre:" << std::endl;
    for (size_t i = 0; i < std::min(personas.size(), size_t(10)); ++i) { // mostrar los primeros 10 elementos
        std::cout << personas[i].nombre << std::endl;
    }
}
void useAnyOf(std::vector<Persona>& personas) {
    //any_of, all_of none_of
    bool anyJuan = std::any_of(std::execution::par, personas.begin(), personas.end(), [](const Persona& p) {
        return p.nombre == "Juan"; // Puede a ver regex
    });

    if (anyJuan) {
        std::cout << "There is at least one Persona with nombre 'Juan'." << std::endl;
    } else {
        std::cout << "There are no Personas with nombre 'Juan'." << std::endl;
    }
}


int main(){
    // Cargar 1000 personas desde un archivo
    std::ifstream inputFile("personas.txt");
    std::vector<Persona> personas;
    
    personas.reserve(1000);
    
    std::string line;
    while (std::getline(inputFile, line)) {
        Persona persona;
        persona.fromString(line);
        personas.push_back(std::move(persona));
    }
    inputFile.close();
    std::vector<Persona> personas10 = std::vector<Persona>(personas.begin(), personas.begin() + std::min(size_t(10), personas.size()));
    std::cout<<std::endl << "First 10 Personas:" << std::endl;
    std::cout<<std::endl << "Demo useTransformReduce" << std::endl;
    useTransformReduce(personas);
    std::cout<<std::endl<< "Demo useForEach" << std::endl;
    useForEach(personas10);
    std::cout<<std::endl << "Demo useFindIf" << std::endl;
    useFindIf(personas);
    std::cout<<std::endl << "Demo useSort" << std::endl;
    useSort(personas);
    std::cout<<std::endl << "Demo useAnyOf" << std::endl;
    useAnyOf(personas);

    return 0;
}


