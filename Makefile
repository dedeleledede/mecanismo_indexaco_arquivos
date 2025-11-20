CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude

TARGET = indice

SRCS = 	src/main.cpp \
       	src/index.cpp \
       	src/text_processor.cpp \
       	src/indexer.cpp \
	   	src/serializer.cpp 
OBJS = $(SRCS:src/%.cpp=build/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
		@mkdir -p build
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build