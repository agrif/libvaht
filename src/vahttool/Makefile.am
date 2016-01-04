AUTOMAKE_OPTIONS = gnu

bin_PROGRAMS = vahttool

vahttool_SOURCES = main.c options.c mode-list.c mode-extract.c convert-tBMP.c convert-tMOV.c convert-tWAV.c convert-NAME.c convert-CARD.c convert-PLST.c convert-script.c convert-BLST.c convert-HSPT.c convert-RMAP.c convert-SLST.c
noinst_HEADERS = vahttool.h
vahttool_CFLAGS = -I@top_srcdir@/include/ -Wall -Werror
vahttool_LDADD = @top_builddir@/src/libvaht.la
