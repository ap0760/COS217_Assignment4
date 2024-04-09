#--------------------------------------------------------------------
# Makefile for Assignment 4, Part 3
#--------------------------------------------------------------------

all: ft
clean:
	rm -f ft meminfo*.out
clobber: clean
	rm -f dynarray.o path.o ft_client.o nodeFT.o ft.o

ft: dynarray.o path.o nodeFT.o ft.o ft_client.o
	gcc217 -g dynarray.o path.o nodeFT.o ft.o ft_client.o -o ft

dynarray.o: dynarray.c dynarray.h
	gcc217 -g -c dynarray.c
path.o: path.c dynarray.h path.h a4def.h
	gcc217 -g -c path.c
ft_client.o: ft_client.c ft.h a4def.h
	gcc217 -g -c ft_client.c
nodeFT.o: nodeFT.c dynarray.h path.h nodeFT.h a4def.h
	gcc217 -g -c nodeFT.c
ft.o: ft.c nodeFT.h ft.h dynarray.h path.h a4def.h
	gcc217 -g -c ft.c
