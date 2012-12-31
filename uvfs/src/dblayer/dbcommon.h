/*
Author : Lakshmi Narasimhan T V

*/

#ifndef __DB_COMMON_H
#define __DB_COMMON_H

#include <mysql.h>

#define STRWORKSPACEIDSIZE (sizeof(char) * sizeof(WORKSPACEID) * 4)
#define STRDEPIDSIZE (sizeof(char) * sizeof(DEPID) * 4)
#define STRDOIDSIZE (sizeof(char) * sizeof(DOID) * 4)

typedef enum{
        NOTOK=-1,
        OK
}RETVAL;

const char * get_auditdb_name();
const char * get_user_name();
const char * get_password();
const char * get_host_name();
MYSQL * get_sql_conn();
void free_mysql_conn(MYSQL * conn);
char * allocate_query_space();
void free_query_space(char * s);
RETVAL exec_query(MYSQL * conn, char * query, MYSQL_RES ** res);
unsigned int get_uint_from_resultset(MYSQL_RES * res);
RETVAL convert_uint_to_str(unsigned int val, char ** result);
const char * get_workspacetable_name();
const char * get_dotable_name();
const char * get_deptable_name();
char * itoa(unsigned int, char * result, int base);
char * strrev(char * str);
void free_resultset(char ** result);

//
char ** get_strings_from_resultset(MYSQL_RES *res, MYSQL_ROW row);

#endif
