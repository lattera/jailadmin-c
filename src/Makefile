TARGET=jailadmin
CC=clang
CFLAGS=-I../include -I/usr/local/include -DDEBUG -g
LDFLAGS=-Wl,-L/usr/local/lib/mysql -lmysqlclient -lncurses
SRCS=main.c sql.c list.c misc.c ini.c schema.c jail.c network.c mount.c interactive.c interactive/status.c interactive/jailcmd.c
OBJS=main.o sql.o list.o misc.o ini.o schema.o jail.o network.o mount.o interactive.o status.o jailcmd.o

${TARGET}: ${OBJS}
	${CC} -o ${TARGET} ${LDFLAGS} ${OBJS}

clean:
	rm -f *.o jailadmin

status.o: interactive/status.c
	${CC} -c -o status.o ${CFLAGS} interactive/status.c

jailcmd.o: interactive/jailcmd.c
	${CC} -c -o jailcmd.o ${CFLAGS} interactive/jailcmd.c
