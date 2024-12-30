CC = gcc

CFLAGS = -std=c2x -Wpedantic -Wall \
	-Wextra -Wconversion -Werror -fstack-protector-all -fpie \
	-pie -O2 -D_FORTIFY_SOURCE=2 -MMD	$(patsubst %, -I'%', $(VPATH)) \
	-D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=500 -Wfatal-errors \
	-lpthread -DDEBUG=0

# nom de l'executable du serveur
TARGET = server

# nom de l'executable d'un programme d'exemple de client
CLIENT = client

# noms des modules utilises dans le projet
MODULES = matrix segment workera workerb utils

#emplacements pour chercher les fichiers .c et .h
VPATH	= $(patsubst %, src/%, $(MODULES))

#noms des fichiers .c utilises dans le projet
SRCS = $(patsubst %, %.c, $(MODULES))
SRCS += main.c

#chemin des fichiers objet
OBJS = $(patsubst %.c, obj/%.o, $(SRCS))

DEPS = $(OBJS:.o=.d)

#cible par defaut : executable serveur et executable client
all: $(TARGET) $(CLIENT)

#cible executable serveur
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

#cible executable client
$(CLIENT): client.c
	$(CC) $(CFLAGS) -o $@ $<

#regles de compilation des fichiers objet
obj/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o : main.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

#cible de nettoyage
clean:
	rm -f $(TARGET) $(CLIENT) $(OBJS) $(DEPS)

distclean: clean
	rm -f -v server_request_pipe /dev/shm/*

.PHONY: all clean distclean
