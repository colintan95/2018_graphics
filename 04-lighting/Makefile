HEADERS = model.h
SRC = main.cc model.cc

app: ${HEADERS} ${SRC}
	g++ -std=c++11 -I ../include -lSDL2 -framework OpenGL ${SRC} -o app
