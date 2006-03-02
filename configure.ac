# $Id: configure.ac,v 1.15 2003/10/02 13:41:43 paul Exp $
# basic setup
AC_PREREQ(2.57)
AC_INIT(fluids,0.0.0)

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(fluids,0.0.0)
AM_CONFIG_HEADER(config.h)

# check for some default development needs
AC_PROG_CC
AC_C_CONST
AC_PROG_INSTALL
AC_HEADER_STDC
AC_CHECK_HEADERS

# setting CFLAGS
CFLAGS="$CLAGS -Wall -Werror -g -O2 -finline-functions"

# gettext stuff
# ALL_LINGUAS="nl"
# AM_GNU_GETTEXT_VERSION(0.12.1)
# AM_GNU_GETTEXT

# checking for SDL
GLUT_VERSION=3.8.0
AM_PATH_GLUT($GLUT_VERSION, :,
            AC_MSG_ERROR([*** GLUT version $GLUT_VERSION not found!])
            )

# checking for SDL_ttf
SDLTTF_LIB=""
AC_CHECK_LIB(SDL_ttf,TTF_Init,SDLTTF_LIB="-lSDL_ttf")
if test "$SDLTTF_LIB" = "" ; then
  AC_MSG_ERROR([SDL_ttf not found])
fi
AC_SUBST(SDLTTF_LIB)

#checking for libxml
XML_VERSION=2.0.0
AM_PATH_XML2(2.0.0, :,
            AC_MSG_ERROR([*** LIBXML version $XML_VERSION not found!])
           )

AC_OUTPUT(
  Makefile
  src/Makefile
  po/Makefile.in
  intl/Makefile
  tests/Makefile
  utils/Makefile
  utils/ffrenzy-menu.py
  data/Makefile
  data/sounds/Makefile
  data/music/Makefile
  data/levels/Makefile
  data/fonts/Makefile
)
