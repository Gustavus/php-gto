PHP_ARG_ENABLE(gto, Whether or not to enable the Gustavus Testing Overrides extension, [  --enable-gto   Enable the Gustavus Testing Overrides extension])

if test "$PHP_GTO" != "no"; then
  PHP_NEW_EXTENSION(gto, php_gto.c, $ext_shared)

  dnl Boilerplate for OSX compatibility. Not sure why anyone would want this, but whatevs.
  dnl Block originates from the PHP extension tutorial found here:
  dnl http://www.kchodorow.com/blog/2011/08/11/php-extensions-made-eldrich-hello-world/
  case $build_os in
    darwin1*.*.*)
      AC_MSG_CHECKING([whether or not to compile for recent osx architectures])
      CFLAGS="$CFLAGS -arch i386 -arch x86_64 -mmacosx-version-min=10.5"
      AC_MSG_RESULT([yes])
      ;;

    darwin*)
      AC_MSG_CHECKING([whether or not to compile for every osx arcitecture ever])
      CFLAGS="$CFLAGS -arch i386 -arch x86_64 -arch ppc -arch ppc64"
      AC_MSG_RESULT([yes])
      ;;
  esac
fi