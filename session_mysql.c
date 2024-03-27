#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "session_mysql.h"

#include "ext/session/php_session.h"

ZEND_DECLARE_MODULE_GLOBALS(session_mysql)

/* {{{ zend_session_mysql_init_globals
 */
static void session_mysql_init_globals(zend_session_mysql_globals *session_mysql_globals_p TSRMLS_DC)
{
	SESSION_MYSQL_G(db)=NULL;
	SESSION_MYSQL_G(host)=NULL;
	SESSION_MYSQL_G(sock)=NULL;
	SESSION_MYSQL_G(port)=0;
	SESSION_MYSQL_G(user)=NULL;
	SESSION_MYSQL_G(pass)=NULL;
	SESSION_MYSQL_G(mysql)=NULL;
	SESSION_MYSQL_G(sess_host)=NULL;
}
/* }}} */

PHP_MINIT_FUNCTION(session_mysql);
PHP_MSHUTDOWN_FUNCTION(session_mysql);
PHP_RINIT_FUNCTION(session_mysql);
PHP_RSHUTDOWN_FUNCTION(session_mysql);
PHP_MINFO_FUNCTION(session_mysql);

/* {{{ session_mysql_functions[]
 */
/* compiled function list so Zend knows what's in this module */
static function_entry session_mysql_functions[] =
{
	ZEND_FE(session_mysql_status, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ session_mysql_module_entry
 */
/* compiled module information */
zend_module_entry session_mysql_module_entry =
{
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
	"session mysql",
	session_mysql_functions,
	PHP_MINIT(session_mysql),
	PHP_MSHUTDOWN(session_mysql),
	PHP_RINIT(session_mysql),
	PHP_RSHUTDOWN(session_mysql),
	PHP_MINFO(session_mysql),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_SESSION_MYSQL_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* implement standard "stub" routine to introduce ourselves to Zend */
#if COMPILE_DL_SESSION_MYSQL
ZEND_GET_MODULE(session_mysql)
#endif

/* {{{ PHP_INI_MH(OnChangeSessionMysqlHost)
 */
PHP_INI_MH(OnChangeSessionMysqlHost)
{
	int len = new_value_length;
	int i=0,y;
	char *val,*host=NULL,*db=NULL,*user=NULL,*pass=NULL,*sock=NULL;
	unsigned int port;

	val=estrdup(new_value);

	len=strlen(val);

	while(val[i]) {
		if(!strncmp(val+i,"host=",5)) {
			i+=5;
			y=i;
			while(val[y] && val[y]!=' ') {
				y++;
			}
			val[y]='\0';
			host=pestrdup(val+i,1);
			i=y+1;
		} else if (!strncmp(val+i,"port=",5)) {
			i+=5;
			y=i;
			while(val[y] && val[y]!=' ') {
				y++;
			}
			val[y]='\0';
			port=(unsigned int) strtol(val+i, NULL, 10);
			i=y+1;
		} else if (!strncmp(val+i,"sock=",5)) {
			i+=5;
			y=i;
			while(val[y] && val[y]!=' ') {
				y++;
			}
			val[y]='\0';
			sock=pestrdup(val+i,1);
			i=y+1;
		} else if (!strncmp(val+i,"db=",3)) {
			i+=3;
			y=i;
			while(val[y] && val[y]!=' ') {
				y++;
			}
			val[y]='\0';
			db=pestrdup(val+i,1);
			i=y+1;
		} else if (!strncmp(val+i,"user=",5)) {
			i+=5;
			y=i;
			while(val[y] && val[y]!=' ') {
				y++;
			}
			val[y]='\0';
			user=pestrdup(val+i,1);
			i=y+1;
		} else if (!strncmp(val+i,"pass=",5)) {
			i+=5;
			y=i;
			while(val[y] && val[y]!=' ') {
				y++;
			}
			val[y]='\0';
			pass=pestrdup(val+i,1);
			i=y+1;
		} else {
			i++;
		}
	}

	if ((!host && !sock) || !db || !user || !pass) {
		return(FAILURE);
	}
	if (SESSION_MYSQL_G(host)) {
		pefree(SESSION_MYSQL_G(host),1);
	}
	if (SESSION_MYSQL_G(db)) {
		pefree(SESSION_MYSQL_G(db),1);
	}
	if (SESSION_MYSQL_G(user)) {
		pefree(SESSION_MYSQL_G(user),1);
	}
	if (SESSION_MYSQL_G(pass)) {
		pefree(SESSION_MYSQL_G(pass),1);
	}
	if (SESSION_MYSQL_G(sock)) {
		pefree(SESSION_MYSQL_G(sock),1);
	}
	SESSION_MYSQL_G(host)=host;
	SESSION_MYSQL_G(db)=db;
	SESSION_MYSQL_G(user)=user;
	SESSION_MYSQL_G(pass)=pass;
	SESSION_MYSQL_G(sock)=sock;
	SESSION_MYSQL_G(port)=port;

	for(i=0 ; i<strlen(new_value) ; i++) {
		new_value[i]=' ';
	}

	efree(val);

	return(SUCCESS);
}
/* }}} */

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
STD_PHP_INI_ENTRY("session_mysql.db", "host=localhost db=phpsession user=phpsession pass=phpsession", PHP_INI_ALL, OnChangeSessionMysqlHost, conn, zend_session_mysql_globals, session_mysql_globals)
STD_PHP_INI_ENTRY("session_mysql.hostcheck", "1", PHP_INI_ALL, OnUpdateBool, hostcheck, zend_session_mysql_globals, session_mysql_globals)
STD_PHP_INI_ENTRY("session_mysql.hostcheck_removewww", "1", PHP_INI_ALL, OnUpdateBool, hostcheck_removewww, zend_session_mysql_globals, session_mysql_globals)
STD_PHP_INI_ENTRY("session_mysql.persistent", "1", PHP_INI_ALL, OnUpdateBool, persistent, zend_session_mysql_globals, session_mysql_globals)
STD_PHP_INI_ENTRY("session_mysql.gc_maxlifetime", "21600", PHP_INI_ALL, OnUpdateString, gc_maxlifetime, zend_session_mysql_globals, session_mysql_globals)
STD_PHP_INI_ENTRY("session_mysql.quiet", "0", PHP_INI_ALL, OnUpdateBool, quiet, zend_session_mysql_globals, session_mysql_globals)
STD_PHP_INI_ENTRY("session_mysql.locking", "1", PHP_INI_ALL, OnUpdateBool, locking, zend_session_mysql_globals, session_mysql_globals)
STD_PHP_INI_ENTRY("session_mysql.lock_timeout", "5", PHP_INI_ALL, OnUpdateString, lock_timeout, zend_session_mysql_globals, session_mysql_globals)
PHP_INI_END()
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(session_mysql)
{
	ZEND_INIT_MODULE_GLOBALS(session_mysql, session_mysql_init_globals, NULL);

	REGISTER_INI_ENTRIES();

//	ChangeSessionMysqlHost(TSRMLS_C);

	php_session_register_module(&ps_mod_mysql);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(session_mysql)
{
	UNREGISTER_INI_ENTRIES();

	if (SESSION_MYSQL_G(mysql)) {
		mysql_close(SESSION_MYSQL_G(mysql));
		SESSION_MYSQL_G(mysql)=NULL;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(session_mysql)
{
	zval **array, **token;

	if (zend_hash_find(&EG(symbol_table), "_SERVER", sizeof("_SERVER"), (void **) &array) == SUCCESS &&
		Z_TYPE_PP(array) == IS_ARRAY &&
		zend_hash_find(Z_ARRVAL_PP(array), "SERVER_NAME", sizeof("SERVER_NAME"), (void **) &token) == SUCCESS) {

		SESSION_MYSQL_G(sess_host)=estrdup(Z_STRVAL_PP(token));
	} else {
		SESSION_MYSQL_G(sess_host)=NULL;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(session_mysql)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(session_mysql)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "MySQL Session Save Handler", "enabled");
	php_info_print_table_row(2, "Version", PHP_SESSION_MYSQL_VERSION);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

ps_module ps_mod_mysql = {
	PS_MOD(mysql)
};

static char *get_escapedhost() {
	char *server_name, *escapedhost;
	int host_len;

	if (SESSION_MYSQL_G(hostcheck) && SESSION_MYSQL_G(sess_host)) {
		if (SESSION_MYSQL_G(hostcheck_removewww)) {
			if(!strncasecmp("www.",SESSION_MYSQL_G(sess_host),4)) {
				SESSION_MYSQL_G(sess_host)+=4;
			}
		}
		host_len=strlen(SESSION_MYSQL_G(sess_host));
		escapedhost=emalloc(host_len*2+1);
		mysql_real_escape_string(SESSION_MYSQL_G(mysql),escapedhost,SESSION_MYSQL_G(sess_host),host_len);
	} else {
		escapedhost=estrdup("");
	}
	return escapedhost;
}

static int session_mysql_connect() {
#if	MYSQL_VERSION_ID >= 50013
	my_bool opt=1;
#endif

	if (!SESSION_MYSQL_G(mysql) || !SESSION_MYSQL_G(persistent)) {
		if (!SESSION_MYSQL_G(mysql)) {
			if (!(SESSION_MYSQL_G(mysql)=mysql_init(SESSION_MYSQL_G(mysql)))) {
				return FAILURE;
			}
		}

#if	MYSQL_VERSION_ID >= 50013
		mysql_options(SESSION_MYSQL_G(mysql), MYSQL_OPT_RECONNECT, &opt);
#endif
		if (mysql_real_connect(
								SESSION_MYSQL_G(mysql),
								SESSION_MYSQL_G(host),
								SESSION_MYSQL_G(user),
								SESSION_MYSQL_G(pass),
								SESSION_MYSQL_G(db),
								SESSION_MYSQL_G(port),
								SESSION_MYSQL_G(sock),
								CLIENT_FOUND_ROWS)) {

			return SUCCESS;
		}
	} else {
		if (mysql_ping(SESSION_MYSQL_G(mysql))==0) {
			return SUCCESS;
		}
	}
	return FAILURE;
}

static void session_mysql_close() {
	if (!SESSION_MYSQL_G(persistent)) {
		mysql_close(SESSION_MYSQL_G(mysql));
		SESSION_MYSQL_G(mysql)=NULL;
	}
}

static int session_mysql_read(const char *key, char **val, int *vallen TSRMLS_DC) {
	int key_len, query_len, selectquery_len, lockquery_len, escapedhost_len, ret=FAILURE;

	char *prequery_select="select sess_val from phpsession where sess_key='%s' and sess_host='%s' and unix_timestamp()<=sess_mtime+%s";
	int prequery_select_len=106;

	char *prequery_lock="select get_lock('%s%s',%s)";
	int prequery_lock_len=24;

	char *escapedkey=NULL;
	char *escapedhost=NULL;
	char *query_lock=NULL;
	char *query_select=NULL;
	MYSQL_RES *res;
	MYSQL_ROW row;
	my_ulonglong rows;
	unsigned long *lengths;

	escapedhost=get_escapedhost();
	escapedhost_len=strlen(escapedhost);

	key_len=strlen(key);

	escapedkey=emalloc(key_len*2+1);

	mysql_real_escape_string(SESSION_MYSQL_G(mysql),escapedkey,key,key_len);

	if (SESSION_MYSQL_G(locking)) {
		lockquery_len=prequery_lock_len+(key_len*2+1)+escapedhost_len+strlen(SESSION_MYSQL_G(lock_timeout));
		query_lock=emalloc(lockquery_len);
		query_len=snprintf(query_lock,lockquery_len,prequery_lock,escapedkey,escapedhost,SESSION_MYSQL_G(lock_timeout));
		mysql_real_query(SESSION_MYSQL_G(mysql),query_lock,query_len);
		res=mysql_use_result(SESSION_MYSQL_G(mysql));
		mysql_free_result(res);
	}


	selectquery_len=prequery_select_len+(key_len*2+1)+strlen(SESSION_MYSQL_G(gc_maxlifetime))+escapedhost_len;
	query_select=emalloc(selectquery_len);

	query_len=snprintf(query_select,selectquery_len,prequery_select,escapedkey,escapedhost,SESSION_MYSQL_G(gc_maxlifetime));

	if (!mysql_real_query(SESSION_MYSQL_G(mysql),query_select,query_len)) {
		if ((res=mysql_store_result(SESSION_MYSQL_G(mysql)))) {
			if (mysql_num_rows(res)!=0) {
				if (row=mysql_fetch_row(res)) {
					lengths=mysql_fetch_lengths(res);
					if (lengths[0]>0) {
						/* do not use estrdup, because session data can contain '\0' */
						*val=emalloc(lengths[0]+1);
						memcpy(*val,row[0],lengths[0]);
						*(*val+lengths[0])='\0';

						*vallen=lengths[0];
						ret=SUCCESS;
					} else {
						*vallen=0;
					}
				}
			}
			mysql_free_result(res);
		}
	}

	if (escapedkey) {
		efree(escapedkey);
	}

	if (escapedhost) {
		efree(escapedhost);
	}

	if (query_select) {
		efree(query_select);
	}

	if (query_lock) {
		efree(query_lock);
	}
	
	return ret;
} 

static int session_mysql_write(const char *key, const char *val, const int vallen TSRMLS_DC) {
	int key_len, query_len, updatequery_len, insertquery_len, unlockquery_len, escapedhost_len, ret=FAILURE;
	char *prequery_update="update phpsession set sess_val='%s',sess_mtime=unix_timestamp() where sess_host='%s' and sess_key='%s'";
	int prequery_update_len=102;
	char *prequery_insert="insert into phpsession(sess_key,sess_host,sess_mtime,sess_val) values('%s','%s',unix_timestamp(),'%s')";
	int prequery_insert_len=102;
	char *prequery_unlock="select release_lock('%s%s')";
	int prequery_unlock_len=25;
	char *escapedkey=NULL;
	char *escapedval=NULL;
	char *escapedhost=NULL;
	char *query_update=NULL;
	char *query_insert=NULL;
	char *query_unlock=NULL;
	MYSQL_RES *res;

	escapedhost=get_escapedhost();
	escapedhost_len=strlen(escapedhost);

	key_len=strlen(key);

	escapedkey=emalloc(key_len*2+1);
	escapedval=emalloc(vallen*2+1);

	mysql_real_escape_string(SESSION_MYSQL_G(mysql),escapedkey,key,key_len);
	mysql_real_escape_string(SESSION_MYSQL_G(mysql),escapedval,val,vallen);

	updatequery_len=prequery_update_len+(key_len*2+1)+(vallen*2+1)+escapedhost_len;

	query_update=emalloc(updatequery_len);

	query_len=snprintf((char *)query_update,updatequery_len,prequery_update,escapedval,escapedhost,escapedkey);

	if (!mysql_real_query(SESSION_MYSQL_G(mysql),query_update,query_len)) {
		if (mysql_affected_rows(SESSION_MYSQL_G(mysql))==1) {
			ret=SUCCESS;
		} else {
			insertquery_len=prequery_insert_len+(key_len*2+1)+(vallen*2+1)+escapedhost_len;
			query_insert=emalloc(insertquery_len);

			query_len=snprintf((char *)query_insert,insertquery_len,prequery_insert,escapedkey,escapedhost,escapedval);

			if (!mysql_real_query(SESSION_MYSQL_G(mysql),query_insert,query_len)) {
				if (mysql_affected_rows(SESSION_MYSQL_G(mysql))==1) {
					ret=SUCCESS;
				}
			}
		}
	}

	if (SESSION_MYSQL_G(locking)) {
		unlockquery_len=prequery_unlock_len+(key_len*2+1)+escapedhost_len;
		query_unlock=emalloc(unlockquery_len);
		query_len=snprintf(query_unlock,unlockquery_len,prequery_unlock,escapedkey,escapedhost);
		mysql_real_query(SESSION_MYSQL_G(mysql),query_unlock,query_len);
		res=mysql_use_result(SESSION_MYSQL_G(mysql));
		mysql_free_result(res);
	}


	if (escapedkey) {
		efree(escapedkey);
	}
	if (escapedhost) {
		efree(escapedhost);
	}
	if (escapedval) {
		efree(escapedval);
	}
	if (query_update) {
		efree(query_update);
	}
	if (query_insert) {
		efree(query_insert);
	}
	if (query_unlock) {
		efree(query_unlock);
	}

	return ret;
}


static int session_mysql_delete(const char *key TSRMLS_DC) {
	int key_len, query_len, deletequery_len, unlockquery_len, escapedhost_len, ret=FAILURE;
	char *prequery_delete="delete from phpsession where sess_key='%s' and sess_host='%s'";
	int prequery_delete_len=62;
	char *prequery_unlock="select release_lock('%s%s')";
	int prequery_unlock_len=25;
	char *escapedkey=NULL;
	char *escapedhost=NULL;
	char *query_delete=NULL;
	char *query_unlock=NULL;
	MYSQL_RES *res;
	MYSQL_ROW row;
	my_ulonglong rows;
	unsigned long *lengths;

	escapedhost=get_escapedhost();
	escapedhost_len=strlen(escapedhost);

	key_len=strlen(key);

	escapedkey=emalloc(key_len*2+1);

	mysql_real_escape_string(SESSION_MYSQL_G(mysql),escapedkey,key,key_len);

	deletequery_len=prequery_delete_len+(key_len*2+1)+escapedhost_len;
	query_delete=emalloc(deletequery_len);

	query_len=snprintf(query_delete,deletequery_len,prequery_delete,escapedkey,escapedhost);

	if (!mysql_real_query(SESSION_MYSQL_G(mysql),query_delete,query_len)) {
		if (mysql_affected_rows(SESSION_MYSQL_G(mysql))!=-1) {
			ret=SUCCESS;
		}
	}

	if (SESSION_MYSQL_G(locking)) {
		unlockquery_len=prequery_unlock_len+(key_len*2+1)+escapedhost_len;
		query_unlock=emalloc(unlockquery_len);
		query_len=snprintf(query_unlock,unlockquery_len,prequery_unlock,escapedkey,escapedhost);
		mysql_real_query(SESSION_MYSQL_G(mysql),query_unlock,query_len);
		res=mysql_use_result(SESSION_MYSQL_G(mysql));
		mysql_free_result(res);
	}

	if (escapedkey) {
		efree(escapedkey);
	}

	if (escapedhost) {
		efree(escapedhost);
	}

	if (query_delete) {
		efree(query_delete);
	}

	if (query_unlock) {
		efree(query_unlock);
	}

	return ret;
}

static int session_mysql_gc() {
	char *prequery="delete from phpsession where unix_timestamp()>=sess_mtime+%s";
	char *query=NULL;
	char *query_optimize="optimize table phpsession";
	int query_optimize_len=25;
	int key_len, query_len, prequery_len, ret=FAILURE;
	MYSQL_RES *res;

	prequery_len=strlen(prequery)+strlen(SESSION_MYSQL_G(gc_maxlifetime))+1;
	query=emalloc(prequery_len);

	query_len=snprintf(query,prequery_len,prequery,SESSION_MYSQL_G(gc_maxlifetime));

	if (!mysql_real_query(SESSION_MYSQL_G(mysql),query,query_len)) {
		if (mysql_affected_rows(SESSION_MYSQL_G(mysql))==1) {
			ret=SUCCESS;
		}
	}
	if (!mysql_real_query(SESSION_MYSQL_G(mysql),query_optimize,query_optimize_len)) {
		res=mysql_use_result(SESSION_MYSQL_G(mysql));
		mysql_free_result(res);
	}

	if (query) {
		efree(query);
	}
	
	return ret;
}

/* session save handler functions */
/* {{{ PS_OPEN_FUNC
 */
PS_OPEN_FUNC(mysql)
{
	int ret;
	*mod_data = (void *)1; 

	ret=session_mysql_connect();
	if (SESSION_MYSQL_G(quiet)) {
		return SUCCESS;
	} else {
		return ret;
	} 
}
/* }}} */

/* {{{ PS_CLOSE_FUNC
 */
PS_CLOSE_FUNC(mysql)
{
	*mod_data = (void *)0;

	session_mysql_close();

	return SUCCESS;
}
/* }}} */

/* {{{ PS_READ_FUNC
 */
PS_READ_FUNC(mysql)
{
	int ret;
	if (!SESSION_MYSQL_G(mysql)) {
		if (!session_mysql_connect()){
			return FAILURE;
		}
	}

	/* here is ok to return failure */
	ret=session_mysql_read(key,val,vallen TSRMLS_CC);

	return ret;
}
/* }}} */

/* {{{ PS_WRITE_FUNC
 */
PS_WRITE_FUNC(mysql)
{
	int ret;

	if (!SESSION_MYSQL_G(mysql)) {
		if (!session_mysql_connect()){
			if (SESSION_MYSQL_G(quiet)) {
				return SUCCESS;
			} else {
				return FAILURE;
			}
		}
	}

	ret=session_mysql_write(key, val, vallen TSRMLS_CC);

	if (SESSION_MYSQL_G(quiet)) {
		return SUCCESS;
	} else {
		return ret;
	}
}
/* }}} */

/* {{{ PS_DESTROY_FUNC
 */
PS_DESTROY_FUNC(mysql)
{
	int ret;

	if (!SESSION_MYSQL_G(mysql)) {
		if (!session_mysql_connect()){
			if (SESSION_MYSQL_G(quiet)) {
				return SUCCESS;
			} else {
				return FAILURE;
			}
		}
	}

	ret=session_mysql_delete(key TSRMLS_CC);

	if (SESSION_MYSQL_G(quiet)) {
		return SUCCESS;
	} else {
		return ret;
	}
}
/* }}} */

/* {{{ PS_GC_FUNC
 */
PS_GC_FUNC(mysql)
{
	int ret;

	if (!SESSION_MYSQL_G(mysql)) {
		if (!session_mysql_connect()){
			if (SESSION_MYSQL_G(quiet)) {
				return FAILURE;
			} else {
				return SUCCESS;
			}
		}
	}

	ret=session_mysql_gc();

	if (SESSION_MYSQL_G(quiet)) {
		return SUCCESS;
	} else {
		return ret;
	}
}
/* }}} */

#if HARDENING_PATCH
/* {{{ PS_VALIDATE_SID_FUNC
 */
PS_VALIDATE_SID_FUNC(mysql)
{
	int ret, vallen;
	char *val, c;
	const char *p;     


	for (p = key; (c = *p); p++) {
		/* valid characters are a..z,A..Z,0..9 */
		if (!((c >= 'a' && c <= 'z')
			|| (c >= 'A' && c <= 'Z')
			|| (c >= '0' && c <= '9')
			|| c == ','
			|| c == '-')) {
				return FAILURE;
		}
	}

	if (!PS(use_strict_mode)) {
		return SUCCESS;
	}

	if (!SESSION_MYSQL_G(mysql)) {
		if (!session_mysql_connect()){
			return FAILURE;
		}
	}

	ret=session_mysql_read(key,&val,&vallen TSRMLS_CC);

	return ret;    
}
/* }}} */
#endif


ZEND_FUNCTION(session_mysql_status) {
	return;
}


/*
 * Local variables:
 * tab-width: 6
 * c-basic-offset: 6
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
