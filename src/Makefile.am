AUTOMAKE_OPTIONS = gnu
SUBDIRS = . vahttool

lib_LTLIBRARIES = libvaht.la

libvaht_la_SOURCES = vaht_swap.c vaht_archive.c vaht_resource.c vaht_bmp.c vaht_mov.c vaht_wav.c vaht_name.c vaht_card.c vaht_plst.c vaht_script.c vaht_blst.c vaht_hspt.c
noinst_HEADERS = vaht_intern.h
libvaht_la_CFLAGS = -I@top_srcdir@/include/ -fpack-struct=1 -Wall -Werror
libvaht_la_LDFLAGS = -version-info 0:0:0
