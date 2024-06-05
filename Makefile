all: ksp-x52

ksp-x52:
	gcc -I /usr/include/libx52  -l x52  -o ksp-x52 main.c
