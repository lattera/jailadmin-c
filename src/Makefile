TARGET=jailadmin
CC=clang
CFLAGS=-I../include -I/usr/local/include -DDEBUG -g
LDFLAGS=-Wl,-L/usr/local/lib/mysql -lmysqlclient
SRCS=main.c sql.c list.c misc.c ini.c
OBJS=main.o sql.o list.o misc.o ini.o

${TARGET}: ${OBJS}
	${CC} -o ${TARGET} ${LDFLAGS} ${OBJS}

clean:
	rm -f *.o jailadmin
