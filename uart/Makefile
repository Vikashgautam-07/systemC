# UART SystemC build

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
SYSTEMC_INC = /usr/local/systemc/include
SYSTEMC_LIB = /usr/local/systemc/lib-linux64
LDFLAGS = -L$(SYSTEMC_LIB) -Wl,-rpath,$(SYSTEMC_LIB)
LDLIBS = -lsystemc

TARGET = uart_test
SOURCES = uart.cc testbench.cc
OBJECTS = $(SOURCES:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -I$(SYSTEMC_INC) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
