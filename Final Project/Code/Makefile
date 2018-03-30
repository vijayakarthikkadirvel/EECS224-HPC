.DEFAULT_GOAL := all

MPICC = mpiCC
MPICFLAGS = -std=c++11
MPICOPTFLAGS = -O3 -g -lpng
MPILDFLAGS =

TARGETS = serial-brandes$(EXEEXT) parallel-brandes$(EXEEXT) sequential_BC.txt$(EXEEXT) parallel_BC.txt$(EXEEXT)

all: $(TARGETS)

parallel-brandes$(EXEEXT): BC_Parallel.cpp
	$(MPICC) $(MPICFLAGS) $(MPICOPTFLAGS) \
	    -o $@ BC_Parallel.cpp $(SRCS_COMMON) $(MPILDFLAGS)


serial-brandes$(EXEEXT): BC_Sequential.cpp
	$(MPICC) $(MPICFLAGS) $(MPICOPTFLAGS)\
	    -o $@ BC_Sequential.cpp $(MPILDFLAGS)


clean:
	rm -f $(TARGETS)

# eof
