TARGET = main

CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

SRCS = string_list.cpp main.cpp

OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean $(TARGET)
