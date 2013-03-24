NAME = portstart
CXX = g++
CXXFLAGS = -Wall -Wextra -Weffc++ -pedantic -std=c++11 -pthread -g -O3 ${INCS} ${CPPFLAGS}
LDFLAGS = -g ${LIBS}
RM = rm -f
CP = cp -f
PREFIX = /usr/local

SRC := $(wildcard *.cpp) iniparser/iniparser.cpp
OBJ = $(patsubst %.cpp,%.o,$(SRC))

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

${NAME} : $(OBJ)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) ${LDFLAGS} $^ -o $@

clean:
	$(RM) "${NAME}" *.o iniparser/*.o

install: all
	mkdir -p "${DESTDIR}${PREFIX}/bin"
	$(CP) "${NAME}" "${DESTDIR}${PREFIX}/bin"
	chmod 755 "${DESTDIR}${PREFIX}/bin/${NAME}"

uninstall:
	$(RM) "${DESTDIR}${PREFIX}/bin/${NAME}"

.PHONY: all clean install uninstall
