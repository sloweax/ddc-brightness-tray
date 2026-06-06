CXX = g++
CXXFLAGS = $(shell pkg-config --cflags Qt6Widgets Qt6Core ddcutil)
LIBS = $(shell pkg-config --libs Qt6Widgets Qt6Core ddcutil)

ddc-brightness-tray: main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	rm ddc-brightness-tray
