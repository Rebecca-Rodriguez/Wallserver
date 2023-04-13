all:
	c++ -std=c++17 -o wallserver wallserver.cpp

clean:
	rm wallserver