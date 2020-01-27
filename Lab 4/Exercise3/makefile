# Entfernenn Sie das '#' in der nächsten Zeile, um Compileroptimierungen
# einzuschalten.
#OPT = -O

all: heat ViewMatrix.class

heat: heat.cpp solver-gauss.cpp
	mpic++ $(OPT) -o heat heat.cpp solver-gauss.cpp

ViewMatrix.class: ViewMatrix.java
	javac ViewMatrix.java

clean:
	rm -f *.o *.cpp~ heat Matrix.txt
	rm -f ViewMatrix.class
 
