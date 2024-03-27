PHP_ARG_ENABLE(session_mysql, whether to enable mysql session storage support, [  --enable-session-mysql   Enable mysql session storage support])
PHP_ARG_WITH(mysql, path to mysql install, [  --with-mysql=DIR        Path to mysql install dir, if not in /usr or /usr/local])

if test "$PHP_SESSION_MYSQL" = "yes"; then
  AC_DEFINE(HAVE_SESSION_MYSQL, 1, [Whether you have session mysql storage])
  if test "$PHP_MYSQL"; then
    PHP_ADD_INCLUDE($PHP_MYSQL/include/mysql)
  fi
  PHP_NEW_EXTENSION(session_mysql, session_mysql.c, $ext_shared)
fi


