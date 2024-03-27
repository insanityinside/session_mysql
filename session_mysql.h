#ifndef PHP_SESSION_MYSQL_H
#define PHP_SESSION_MYSQL_H 1

#ifdef ZTS
#include "TSRM.h"
#endif

#include <mysql.h>

ZEND_BEGIN_MODULE_GLOBALS(session_mysql)
	char *conn;
	int hostcheck;
	int hostcheck_removewww;
	int persistent;
	int quiet;
	int locking;
	char *gc_maxlifetime;
	char *lock_timeout;

	MYSQL *mysql;
	char *db;
	char *host;
	char *user;
	char *pass;
	char *sess_host;
ZEND_END_MODULE_GLOBALS(session_mysql)

#ifdef ZTS
#define SESSION_MYSQL_G(v) TSRMG(session_mysql_globals_id, zend_session_mysql_globals *, v)
#else
#define SESSION_MYSQL_G(v) (session_mysql_globals.v)
#endif

#define PHP_SESSION_MYSQL_VERSION "1.7"
#define PHP_SESSION_MYSQL_EXTNAME "session_mysql"

#include "ext/session/php_session.h"

extern ps_module ps_mod_mysql;
#define ps_mysql_ptr &ps_mod_mysql

extern zend_module_entry session_mysql_module_entry;
#define phpext_session_mysql_ptr &session_mysql_module_entry

ZEND_FUNCTION(session_mysql_status);

PS_FUNCS(mysql);

#endif

