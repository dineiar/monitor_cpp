CPP = g++ -std=c++1y
FLAGS = -Wall -O3 
INCLUDE = ../include/


all: 
	$(CPP) $(FLAGS) -I $(INCLUDE) server.cpp -o server
	$(CPP) $(FLAGS) -I $(INCLUDE) client.cpp -o client
clean:
	rm -rf server client

