# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

# Имя исполняемого файла
TARGET = transport_system

# Список исходных файлов
SRCS = main.cpp \
       Bus.cpp \
       Tram.cpp \
       WaterTransport.cpp \
       AirTransport.cpp \
       HybridTransport.cpp

# Автоматически генерируем список объектных файлов
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

fclean: clean

re: fclean all

.PHONY: all clean fclean re

