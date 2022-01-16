all: phong

phong: spinningcube_withlight.cpp textfile.c
	gcc -o phong spinningcube_withlight.cpp -lGL -lGLEW -lglfw -lm

run:
	./phong

clean:
	rm -f phong *~
