all: Simulador.cpp   CamadaFisica.cpp CamadaFisica.hpp CamadaEnlace.cpp CamadaFisica.o CamadaEnlace.hpp CamadaEnlace.o
	g++ -std=c++11 -Wall CamadaFisica.o  CamadaEnlace.o Simulador.cpp -o Simulador
	./Simulador

compile: Simulador.cpp   CamadaFisica.cpp CamadaFisica.hpp CamadaEnlace.cpp CamadaFisica.o CamadaEnlace.o
	g++ -std=c++11 -Wall CamadaFisica.o CamadaEnlace.o Simulador.cpp -o Simulador
	
test: Simulador.cpp
	./Simulador
	
cpplint: Simulador.cpp   CamadaFisica.cpp CamadaEnlace.cpp
	cpplint   --exclude=catch.hpp  *.*
	
debug: Simulador.cpp   CamadaFisica.cpp CamadaFisica.hpp  CamadaEnlace.cpp
	g++ -std=c++11 -Wall -Wall -g -c CamadaFisica.cpp
	g++ -std=c++11 -Wall  -g CamadaFisica.o Simulador.cpp -o Simulador
	gdb Simulador
	
cppcheck: Simulador.cpp   CamadaFisica.cpp CamadaFisica.hpp CamadaEnlace.cpp
	cppcheck  --enable=warning . 

CamadaFisica.o : CamadaFisica.cpp CamadaFisica.hpp CamadaEnlace.cpp CamadaEnlace.hpp
	g++ -std=c++11 -Wall -Wall -c CamadaFisica.cpp

CamadaEnlace.o : CamadaFisica.cpp CamadaFisica.hpp CamadaEnlace.cpp CamadaEnlace.hpp
	g++ -std=c++11 -Wall -Wall -c CamadaEnlace.cpp

Simulador: 	Simulador.cpp   CamadaFisica.cpp camadaEnlace.cpp CamadaFisica.hpp Simulador.o 
	g++ -std=c++11 -Wall CamadaFisica.o CamadaEnlace.o Simulador.cpp -o Simulador

clean:
	rm -rf *.o *.exe *.gc*
	
	
