#!/bin/sh

echo Cleaning...
# First of all clean up the generated crud
rm -f configure config.log config.guess config.sub config.cache
rm -f config.status
rm -f aclocal.m4
rm -f `find . -name 'Makefile'`

echo aclocal...
aclocal-1.7

echo autoconf...
autoconf
echo autoheader...
autoheader
echo autopoint...
autopoint -f
echo automake...
automake-1.7 --add-missing -c --gnu

echo getting files to translate...
echo > po/POTFILES.in ;
for x in src/*.c ; do
  echo $x >> po/POTFILES.in
done;
