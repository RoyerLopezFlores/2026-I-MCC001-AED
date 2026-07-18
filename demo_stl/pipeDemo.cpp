#include <iostream>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <ranges>
#include <regex>
#include <string>
#include <vector>

#include "Persona.hpp"

int main() {
    std::ifstream inputFile("personas.txt");
    if (!inputFile) {
        std::cerr << "No se pudo abrir personas.txt" << std::endl;
        return 1;
    }

    std::vector<Persona> personas;
    for (Persona p; inputFile >> p;) {
        personas.push_back(std::move(p));
    }

    // 1) Transformacion: agregar "V" al inicio de todos los nombres.
    auto nombresConVView = personas
        | std::views::transform([](const Persona& p) {
              return std::string("V") + p.nombre;
          });

    std::vector<std::string> nombresConV;
    std::ranges::copy(nombresConVView, std::back_inserter(nombresConV));

    std::cout << "Nombres con prefijo V (primeros 10):" << std::endl;
    for (const auto& nombre : nombresConV | std::views::take(10)) {
        std::cout << nombre << std::endl;
    }

    // 2) Filter + transform: validar DNI con regex y quedarnos con resumen.
    const std::regex dniRegex(R"(^\d{8}$)");
    auto personasConDniValido = personas
        | std::views::filter([&](const Persona& p) {
              return std::regex_match(p.dni, dniRegex);
          })
        | std::views::transform([](const Persona& p) {
              return p.nombre + " " + p.apellido + " -> DNI: " + p.dni;
          });

    std::cout << "\nPersonas con DNI valido (primeras 10):" << std::endl;
    for (const auto& linea : personasConDniValido | std::views::take(10)) {
        std::cout << linea << std::endl;
    }

    // 3) Mas ejemplos de operador | con views.
    //En python personas[:5] -> personas | take(5)
    auto primeros5Nombres = personas
        | std::views::transform([](const Persona& p) { return p.nombre; })
        | std::views::take(5);
    // parece a python personas[2:7] -> personas | drop(2) | take(5)
    auto saltar2YTomar5 = personas
        | std::views::drop(2)
        | std::views::take(5)
        | std::views::transform([](const Persona& p) { return p.nombre; });
    // parece a python personas[-5:] -> personas | reverse | take(5)
    auto nombresInverso = personas
        | std::views::transform([](const Persona& p) { return p.nombre; })
        | std::views::reverse
        | std::views::take(5);

    std::cout << "\nPrimeros 5 nombres:" << std::endl;
    for (const auto& nombre : primeros5Nombres) {
        std::cout << nombre << std::endl;
    }

    std::cout << "\nSaltar 2 y tomar 5 nombres:" << std::endl;
    for (const auto& nombre : saltar2YTomar5) {
        std::cout << nombre << std::endl;
    }

    std::cout << "\nUltimos 5 nombres (usando reverse + take):" << std::endl;
    for (const auto& nombre : nombresInverso) {
        std::cout << nombre << std::endl;
    }

    return 0;
}