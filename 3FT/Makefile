#--------------------------------------------------------------------
# Makefile for Assignment 4, Part 2
# dt* targets are built using checkerDT
# rules to build dtBad*.o and nodeBad*.o from source will fail
# Author: Christopher Moretti
#--------------------------------------------------------------------

# GCC = gcc217
# #GCC = gcc217m

# TARGETS = ft

# .PRECIOUS: %.o

# all: $(TARGETS)

# clean:
# 	rm -f $(TARGETS) meminfo*.out

# clobber: clean
# 	rm -f dynarray.o path.o ft_client.o nodeFT.o ft2.o *~

# ft: dynarray.o path.o nodeFT.o ft2.o ft_client.o
# 	$(GCC) -g $^ -o $@

# dynarray.o: dynarray.c dynarray.h
# 	$(GCC) -g -c $<

# path.o: path.c dynarray.h path.h a4def.h
# 	$(GCC) -g -c $<

# ft_client.o: ft_client.c ft2.h a4def.h
# 	$(GCC) -g -c $<

# checkerDT.o: checkerDT.c dynarray.h checkerDT.h nodeDT.h path.h a4def.h
# 	$(GCC) -g -c $<

# nodeFT.o: nodeFT.c dynarray.h nodeFT.h path.h a4def.h
# 	$(GCC) -g -c $<

# ft2.o: ft2.c dynarray.h nodeFT.h ft2.h path.h a4def.h
# 	$(GCC) -g -c $<

#You can't re-build the .o files we provide, and
#you shouldn't be changing the header files they rely on
#but in case the headers' modification times have changed,
#update the .o files' modification times to still be newer.
# nodeDT%.o: dynarray.h checkerDT.h nodeDT.h path.h a4def.h
# 	touch $@

# dtBad%.o: dynarray.h checkerDT.h nodeDT.h dt.h path.h a4def.h
# 	touch $@


all: ft2
clean:
	rm -f ft2 meminfo*.out
clobber: clean
	rm -f dynarray.o path.o ft_client.o nodeFT.o ft2.o

ft2: dynarray.o path.o nodeFT.o ft2.o ft_client.o
	gcc217 -g dynarray.o path.o nodeFT.o ft2.o ft_client.o -o ft2

dynarray.o: dynarray.c dynarray.h
	gcc217 -g -c dynarray.c
path.o: path.c dynarray.h path.h a4def.h
	gcc217 -g -c path.c
ft_client.o: ft_client.c ft.h a4def.h
	gcc217 -g -c ft_client.c
nodeFT.o: nodeFT.c dynarray.h path.h nodeFT.h a4def.h
	gcc217 -g -c nodeFT.c
ft2.o: ft2.c nodeFT.h ft.h dynarray.h path.h a4def.h
	gcc217 -g -c ft2.c
