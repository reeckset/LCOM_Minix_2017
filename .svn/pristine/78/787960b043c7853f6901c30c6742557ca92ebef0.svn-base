COMPILER_TYPE= gnu
CC=gcc
PROG= lab5
SRCS= test5.c lab5.c vbe.c video_gr.c video_test.c keyboard.c timer.c
CFLAGS= -Wall
DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys
LDFLAGS+= -L .
MAN=
BINDIR?= /usr/sbin
.include <bsd.prog.mk>
.include <bsd.gcc.mk>
