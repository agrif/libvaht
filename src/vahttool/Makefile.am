AUTOMAKE_OPTIONS = gnu

bin_PROGRAMS = vahttool

vahttool_SOURCES = main.c options.c mode-list.c mode-extract.c convert-tBMP.c convert-tMOV.c
noinst_HEADERS = vahttool.h
vahttool_CFLAGS = -I@top_srcdir@/include/ -fpack-struct=1
vahttool_LDADD = @top_builddir@/src/libvaht.la
