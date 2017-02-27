OBJECTS = hmmain.o
HEADERS = hminc.h
CC = g++
OUTPUT = 
DEBUG = -Wall
LINKING_OPTIONS = 
CFLAGS = $(shell allegro-config --cflags)
LDFLAGS = $(shell allegro-config --libs)
WIN_CC = wine c:\\MinGW\\bin\\gcc.exe
WIN_OUTPUT = hykemiter.exe

hykemiter: $(OBJECTS)
	 $(CC) $(DEBUG) $(LINKING_OPTIONS) -o hykemiter $(OBJECTS) $(LDFLAGS)

hmmain.o: hmmain.cpp $(HEADERS)
	 $(CC) $(DEBUG) -c hmmain.cpp $(CFLAGS)

clean:
	 rm -f $(OBJECTS)

win:
	 $(WIN_CC) $(DEBUG) *.cpp -o $(WIN_OUTPUT) -lalleg

