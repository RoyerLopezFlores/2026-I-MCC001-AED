#include <string>
#include <ostream>
#include <istream>
#include <sstream>
using TS = std::string;
class Persona{
public:
    TS nombre;
    TS apellido;
    TS dni;
    TS fechaNacimiento;
    static constexpr const char* TOKEN_CLASS="Persona";
    static constexpr const char* TOKEN_SEPARATOR="|";

    void fromString(const std::string& line){
        std::istringstream iss(line);
        iss >> *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Persona& persona) {
        os << Persona::TOKEN_CLASS << Persona::TOKEN_SEPARATOR << persona.nombre << Persona::TOKEN_SEPARATOR << persona.apellido << Persona::TOKEN_SEPARATOR << persona.dni << Persona::TOKEN_SEPARATOR << persona.fechaNacimiento;
        return os;
    }
    friend std::istream& operator>>(std::istream& is, Persona& persona) {
        TS token;
        if(!std::getline(is, token, Persona::TOKEN_SEPARATOR[0]) || token != Persona::TOKEN_CLASS) {
            is.setstate(std::ios::failbit);
            return is;
        }
        std::getline(is, persona.nombre, Persona::TOKEN_SEPARATOR[0]);
        std::getline(is, persona.apellido, Persona::TOKEN_SEPARATOR[0]);
        std::getline(is, persona.dni, Persona::TOKEN_SEPARATOR[0]);
        std::getline(is, persona.fechaNacimiento);


        return is;
    }

};

