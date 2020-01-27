# Entfernenn Sie das '#' in der n�chsten Zeile, um Compileroptimierungen
# einzuschalten.
#OPT = -O

all: heat ViewMatrix.class

heat: heat.cpp solver-jacobi.cpp
	g++ $(OPT) -fopenmp -o heat heat.cpp solver-jacobi.cpp
	g++ $(OPT) -fopenmp -o heat-initial heat.cpp solver-jacobi-initial.cpp

ViewMatrix.class: ViewMatrix.java
	javac ViewMatrix.java

clean:
	rm -f *.o *.c~ heat heat-initial Matrix.txt
	rm -f ViewMatrix.class
 
