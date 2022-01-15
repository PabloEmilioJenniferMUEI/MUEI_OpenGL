all: phong

phong: spinningcube_withlight.cpp textfile.c
	g++ -o phong spinningcube_withlight.cpp textfile.c -lGL -lGLEW -lglfw

run:
	./phong

clean:
	rm -f phong *~
