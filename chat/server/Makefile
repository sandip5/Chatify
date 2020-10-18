all:
	g++ -std=c++17 -pthread server.cpp ../dbConnection/dboperation.h  -o server $(pkg-config --cflags --libs libmongocxx)
#all:
#	g++ -pthread -o server server.cpp