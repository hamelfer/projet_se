CC = gcc

# -MMD : Cette option du compilateur génère automatiquement des fichiers de dépendance .d pour chaque fichier .c

CFLAGS = -std=c2x -D_XOPEN_SOURCE -Wpedantic -Wall \
	-Wextra -Wconversion -Werror -fstack-protector-all -fpie \
	-pie -O2 -D_FORTIFY_SOURCE=2 -MMD	$(patsubst %, -I'%', $(VPATH))

# Nom de l'exécutable
TARGET = server

# modules
MODULES = matrix segment worker

#search paths for headers and sources
VPATH	= $(patsubst %, src/%, $(MODULES))


# Liste des fichiers source
SRCS = $(patsubst %, %.c, $(MODULES))
SRCS += main.c
OBJS = $(patsubst %.c, %.o, $(SRCS))

DEPS = $(OBJS:.o=.d)

# Cible par défaut
all: $(TARGET)

# Règle pour construire l'exécutable
# $@ représente le nom de la cible
# $^ représente toutes les dépendances
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Règle générique pour compiler les fichiers source en fichiers objets
# $@ représente le nom de la cible
# $< represente le fichier source
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o : main.c
	$(CC) $(CFLAGS) -c $< -o $@

# Inclut les fichiers .d générés automatiquement, de sorte que make sache quand regénérer les fichiers objets.
-include $(DEPS)

# Cible de nettoyage
clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

# Cible de nettoyage complet
distclean: clean
	rm -f -v *~
		
# Marquer 'clean' et 'distclean' comme des cibles phony
# informe make que all, clean et distclean sont des cibles fictives et ne correspondent pas à des fichiers.
.PHONY: all clean distclean
