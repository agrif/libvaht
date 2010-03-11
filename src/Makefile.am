AUTOMAKE_OPTIONS = gnu
SUBDIRS = . vahttool

lib_LTLIBRARIES = libvaht.la

libvaht_la_SOURCES = vaht_swap.c vaht_archive.c vaht_resource.c vaht_bmp.c vaht_mov.c vaht_wav.c
noinst_HEADERS = vaht_intern.h
libvaht_la_CFLAGS = -I@top_srcdir@/include/ -fpack-struct=1
libvaht_la_LDFLAGS = -version-info 0:0:0
