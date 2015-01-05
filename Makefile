CC = g++
CFLAGS = -std=c++11 -g -Wall -fdiagnostics-color=auto
SRCS = calib.cpp
PROG = main

OPENCV = `pkg-config opencv --cflags --libs`
BOOST = -lboost_filesystem -lboost_system
OPENMP = -fopenmp
LIBS = $(OPENCV) $(OPENMP)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS)  -o $(PROG) $(SRCS) $(LIBS) $(BOOST)

clean:
	rm $(PROG)
