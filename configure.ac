AC_INIT(libvaht, 0.3, aargri@gmail.com)

# make sure our source dir is correct
AC_CONFIG_SRCDIR([src/vaht_archive.c])

# initialize AutoMake
AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)

# use macros from this folder
AC_CONFIG_MACRO_DIR([m4])

# check for required programs
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AM_PROG_LIBTOOL
AC_LANG_C

# check for required headers
AC_HEADER_STDC
AC_CHECK_HEADERS(stdint.h,,AC_MSG_ERROR(libvaht needs fixed-width integer types from stdint.h))

# check for libpng
AC_CHECK_HEADERS(png.h,,AC_MSG_ERROR(vahttool needs libpng to output images in a sane format))
AC_CHECK_LIB(png, png_create_write_struct,,AC_MSG_ERROR(vahttool needs libpng to output images in a sane format))

# Doxygen Support
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)

DX_INIT_DOXYGEN([$PACKAGE_NAME],[include/doxygen.cfg])

# substitute our stuff!
AC_OUTPUT(Makefile src/Makefile include/Makefile src/vahttool/Makefile)
