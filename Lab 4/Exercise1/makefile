# Entfernenn Sie das '#' in der nächsten Zeile, um Compileroptimierungen
# einzuschalten.
#OPT = -O

# Zahl der Knoten und Prozesse für den HorUS-Cluster
NODES = 1
PROCS = 4

all: integrate

integrate: integrate.cpp
	mpic++ $(OPT) -DPARALLEL -o integrate integrate.cpp

clean:
	rm -f *.o *.c~ integrate
 
run:
	srun --exclusive --partition short --mpi=pmi2 \
	     --nodes=$(NODES) --ntasks=$(PROCS) $(PWD)/integrate

