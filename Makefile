# Compilateur
CC = gcc
# FLAGS
CFLAGS = -Wall
# Fichiers
OBJECTS = stack.o automaton.o

all: automaton

#Dépendances
automaton.o: stack.h stack.c
stack.o: stack.h

# Compilation des fichiers .c
%.o: %.c 
	$(CC) $(CFLAGS) -c $<

# Cible 'automaton'
automaton: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@
	@echo "Succès! Cible accessible sous le nom: '$@'\n"

# Suppression des fichiers .o et de la cible
clean:
	-rm *.o automaton


