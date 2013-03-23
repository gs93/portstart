CXX = g++
CXXFLAGS = -Wall -Wextra -Weffc++ -pedantic -std=c++11 -pthread -g -O3
RM = rm -f
CP = cp
SRC = portstart.cpp rule.cpp
DESTDIR = /usr/local

all: portstart

portstart:
	g++ $(CXXFLAGS) $(LDFLAGS) -o portstart $(SRC) $(LDLIBS) 

install:
	$(CP) "portstart" "$(DESTDIR)/sbin"

uninstall:
	$(RM) "$(DESTDIR)/sbin/portstart"

clean:
	$(RM) "portstart"
