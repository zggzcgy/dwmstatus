NAME	= dwmstatus
VERSION	= v0.2

# Customize below to fit your system

# paths
PREFIX		= /usr/local
MANPREFIX	= ${PREFIX}/share/man
X11INC		= /usr/X11R6/include
X11LIB		= /usr/X11R6/lib
SRC_DIR		= src

# includes and libs
INCS		= -I. -I/usr/include -I${X11INC}
LIBS		= -L/usr/lib -lc -L${X11LIB} -lX11

# Default flags
CPPFLAGS	= -DVERSION=\"${VERSION}\"
CFLAGS		= -std=c99 -pedantic -Wall -O0 ${INCS} ${CPPFLAGS}
LDFLAGS		= ${LIBS}

# Debug flasg
DEBUG_FLAGS	= -DDEBUG=1 -W -ansi -g

# compiler and linker
CC	= cc
