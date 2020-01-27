# Entfernenn Sie das '#' in der nächsten Zeile, um Compileroptimierungen
# einzuschalten.
#OPT = -O

all: heat ViewMatrix.class

heat: heat.cpp solver-jacobi.cpp
	g++ $(OPT) -fopenmp -o heat heat.cpp solver-jacobi.cpp

ViewMatrix.class: ViewMatrix.java
	javac ViewMatrix.java

clean:
	rm -f *.o *.c~ heat Matrix.txt
	rm -f ViewMatrix.class
 
