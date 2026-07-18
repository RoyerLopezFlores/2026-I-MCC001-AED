#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

namespace {

constexpr const char* kTokenClass = "Persona";
constexpr const char* kTokenSeparator = "|";

const vector<string> kFirstNames = {
	"Juan", "Luis", "Carlos", "Miguel", "Jose", "Pedro", "Daniel", "Andres",
	"Ricardo", "Sergio", "Mario", "Jorge", "Fernando", "Hector", "Diego", "Cesar",
	"Marcos", "Raul", "Alberto", "Ruben"
};

const vector<string> kMiddleNames = {
	"Antonio", "Enrique", "Manuel", "Eduardo", "Felipe", "Gustavo", "Rafael", "David",
	"Martin", "Oscar", "Patricio", "Alex", "Nicolas", "Fabian", "Ivan", "Bruno"
};

const vector<string> kSurnames = {
	"Perez", "Gomez", "Lopez", "Garcia", "Torres", "Ramos", "Vargas", "Flores",
	"Castillo", "Diaz", "Fernandez", "Medina", "Santos", "Mendoza", "Cortez", "Ruiz",
	"Herrera", "Chavez", "Salazar", "Aguilar"
};

int randomInt(mt19937& rng, int minValue, int maxValue) {
	uniform_int_distribution<int> dist(minValue, maxValue);
	return dist(rng);
}

bool randomChance(mt19937& rng, int percent) {
	uniform_int_distribution<int> dist(1, 100);
	return dist(rng) <= percent;
}

const string& pickOne(const vector<string>& values, mt19937& rng) {
	return values[randomInt(rng, 0, static_cast<int>(values.size()) - 1)];
}

string buildCompoundName(mt19937& rng) {
	string name = pickOne(kFirstNames, rng);
	if (randomChance(rng, 55)) {
		const vector<string>& pool = randomChance(rng, 70) ? kMiddleNames : kFirstNames;
		string second = pickOne(pool, rng);
		if (second != name) {
			name += " " + second;
		}
	}
	return name;
}

string buildCompoundSurname(mt19937& rng) {
	string surname = pickOne(kSurnames, rng);
	if (randomChance(rng, 75)) {
		string second = pickOne(kSurnames, rng);
		if (second == surname) {
			second = pickOne(kSurnames, rng);
		}
		if (second != surname) {
			surname += " " + second;
		}
	}
	return surname;
}

bool isLeapYear(int year) {
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int year, int month) {
	static const int monthLengths[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if (month == 2 && isLeapYear(year)) {
		return 29;
	}
	return monthLengths[month - 1];
}

string buildValidDate(mt19937& rng) {
	int year = randomInt(rng, 1950, 2005);
	int month = randomInt(rng, 1, 12);
	int day = randomInt(rng, 1, daysInMonth(year, month));

	char buffer[11];
	snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year, month, day);
	return buffer;
}

string buildDirtyDate(mt19937& rng) {
	switch (randomInt(rng, 0, 4)) {
		case 0: {
			int year = randomInt(rng, 1950, 2005);
			int month = randomInt(rng, 13, 19);
			int day = randomInt(rng, 1, 31);
			char buffer[11];
			snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year, month, day);
			return buffer;
		}
		case 1: {
			int year = randomInt(rng, 1950, 2005);
			int month = randomInt(rng, 1, 12);
			int day = randomInt(rng, 32, 39);
			char buffer[11];
			snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year, month, day);
			return buffer;
		}
		case 2:
			return "0000-00-00";
		case 3: {
			int year = randomInt(rng, 1950, 2005);
			int month = randomInt(rng, 1, 12);
			int day = randomInt(rng, 1, 28);
			char buffer[11];
			snprintf(buffer, sizeof(buffer), "%04d/%02d/%02d", year, month, day);
			return buffer;
		}
		default: {
			int year = randomInt(rng, 1950, 2005);
			int month = randomInt(rng, 1, 12);
			int day = randomInt(rng, 1, 28);
			char buffer[11];
			snprintf(buffer, sizeof(buffer), "%02d-%02d-%04d", day, month, year);
			return buffer;
		}
	}
}

string buildDni(mt19937& rng, bool dirty) {
	if (!dirty) {
		string dni(8, '0');
		for (char& ch : dni) {
			ch = static_cast<char>('0' + randomInt(rng, 0, 9));
		}
		return dni;
	}

	switch (randomInt(rng, 0, 4)) {
		case 0: {
			int length = randomInt(rng, 4, 7);
			string dni;
			dni.reserve(length);
			for (int i = 0; i < length; ++i) {
				dni.push_back(static_cast<char>('0' + randomInt(rng, 0, 9)));
			}
			return dni;
		}
		case 1: {
			int length = randomInt(rng, 9, 11);
			string dni;
			dni.reserve(length);
			for (int i = 0; i < length; ++i) {
				dni.push_back(static_cast<char>('0' + randomInt(rng, 0, 9)));
			}
			return dni;
		}
		case 2: {
			string dni(8, '0');
			for (char& ch : dni) {
				ch = static_cast<char>('0' + randomInt(rng, 0, 9));
			}
			dni[randomInt(rng, 0, 7)] = static_cast<char>('A' + randomInt(rng, 0, 25));
			return dni;
		}
		case 3:
			return "ABC12345";
		default: {
			string dni(8, '0');
			for (char& ch : dni) {
				ch = static_cast<char>('0' + randomInt(rng, 0, 9));
			}
			dni.insert(randomInt(rng, 1, 6), "-");
			return dni;
		}
	}
}

}  // namespace

int main(int argc, char* argv[]) {
	int count = 10;
	string outputPath = "personas.txt";
	auto start = chrono::steady_clock::now();

	if (argc >= 2) {
		count = max(1, atoi(argv[1]));
	}
	if (argc >= 3) {
		outputPath = argv[2];
	}

	random_device rd;
	mt19937 rng(rd());
	ofstream outputFile(outputPath);
	if (!outputFile) {
		cerr << "No se pudo abrir el archivo de salida: " << outputPath << endl;
		return 1;
	}
    
	for (int i = 0; i < count; ++i) {
		const bool dirtyRecord = randomChance(rng, 40);
		const bool dirtyDate = dirtyRecord && randomChance(rng, 70);
		const bool dirtyDni = dirtyRecord && randomChance(rng, 70);

		const string nombre = buildCompoundName(rng);
		const string apellido = buildCompoundSurname(rng);
		const string dni = buildDni(rng, dirtyDni);
		const string fechaNacimiento = dirtyDate ? buildDirtyDate(rng) : buildValidDate(rng);

		outputFile << kTokenClass << kTokenSeparator << nombre << kTokenSeparator << apellido
				   << kTokenSeparator << dni << kTokenSeparator << fechaNacimiento << '\n';
	}

	auto end = chrono::steady_clock::now();
	auto elapsedMs = chrono::duration_cast<chrono::milliseconds>(end - start).count();
	cout << "Generacion demoro " << elapsedMs << " ms" << endl;

	return 0;
}
/*
.\generator.exe 10
Generacion demoro 0 ms
.\generator.exe 100
Generacion demoro 0 ms
.\generator.exe 1000
Generacion demoro 1 ms
.\generator.exe 1000000
Generacion demoro 987 ms
.\generator.exe 10000000
Generacion demoro 9839 ms
*/