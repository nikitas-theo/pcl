###############################################################################
#  CVS version:
#     $Id: Makefile,v 1.2 2004/05/05 22:00:08 nickie Exp $
###############################################################################
#
#  Makefile    : Makefile
#  Project     : PCL Compiler
#  Version     : 1.0 alpha
#  Written by  : Nikolaos S. Papaspyrou (nickie@softlab.ntua.gr)
#  Date        : May 14, 2003
#  Description : Generic symbol table in C
#
#  Comments: (in Greek iso-8859-7)
#  ---------
#  ������ �������� �����������.
#  ����� ������������ ��������� ��� ��������� �����������.
#  ������ ����������� ������������ ��� �����������.
#  ���������� ����������� ����������


.PHONY: clean distclean count objfiles

default : objfiles

CFILES   = symbol.c error.c general.c 
HFILES   = symbol.h error.h general.h
OBJFILES = $(patsubst %.c,%.o,$(CFILES))
SRCFILES = $(HFILES) $(CFILES)

CC=gcc
CFLAGS=-Wall -pedantic -g

%.o : %.c 
	$(CC) $(CFLAGS) -c $<

general.o  : general.c general.h error.h
error.o    : error.c general.h error.h
symbol.o   : symbol.c symbol.h general.h error.h

objfiles : general.o error.o symbol.o 

clean:
	$(RM) $(OBJFILES) *~

distclean: clean
	$(RM) $(EXEFILES)
