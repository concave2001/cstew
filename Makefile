all: main
	
main: main.c file_control
	gcc -I"includes" -Wall -Wextra -o main main.c

file_control: file_control.c
	gcc -Wall -Wextra -o file_control file_control.c

clean:
	rm -rf main file_control
