CXX = g++
CXXFLAGS = -std=c++17 
LDFLAGS = 
SRC = server.cpp http_tcpServer.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean