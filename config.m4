dnl $Id$
dnl config.m4 for extension donie

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(donie, for donie support,
Make sure that the comment is aligned:
[  --with-donie             Include donie support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(donie, whether to enable donie support,
dnl Make sure that the comment is aligned:
dnl [  --enable-donie           Enable donie support])

if test "$PHP_DONIE" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-donie -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/donie.h"  # you most likely want to change this
  dnl if test -r $PHP_DONIE/$SEARCH_FOR; then # path given as parameter
  dnl   DONIE_DIR=$PHP_DONIE
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for donie files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       DONIE_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$DONIE_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the donie distribution])
  dnl fi

  dnl # --with-donie -> add include path
  dnl PHP_ADD_INCLUDE($DONIE_DIR/include)

  dnl # --with-donie -> check for lib and symbol presence
  dnl LIBNAME=donie # you may want to change this
  dnl LIBSYMBOL=donie # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $DONIE_DIR/$PHP_LIBDIR, DONIE_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_DONIELIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong donie lib version or lib not found])
  dnl ],[
  dnl   -L$DONIE_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(DONIE_SHARED_LIBADD)

  PHP_NEW_EXTENSION(donie, donie.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
