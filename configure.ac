AC_INIT(libvaht, 0.2, aargri@gmail.com)
AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AM_PROG_LIBTOOL
AC_LANG_C

AC_HEADER_STDC
AC_CHECK_HEADERS(stdint.h,,AC_MSG_ERROR(libvaht needs fixed-width integer types from stdint.h))

AC_CHECK_HEADERS(png.h,,AC_MSG_ERROR(vahttool needs libpng to output images in a sane format))
AC_CHECK_LIB(png, png_create_write_struct,,AC_MSG_ERROR(vahttool needs libpng to output images in a sane format))

AC_OUTPUT(Makefile src/Makefile include/Makefile src/vahttool/Makefile)
