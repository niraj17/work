#include "dbcommon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char * audit_db_name;
const char * user_name;
const char * password;
const char * host_name;
char * query;

void db_init ()
{
    audit_db_name = "auditdb";
    user_name = "lvaikunt";
    password = "sid2deb4.ux";
    host_name = "localhost";
    query = (char *) (malloc(sizeof(char) * 4096));
}

const char * get_auditdb_name()
{
    return audit_db_name;
}

const char * get_user_name()
{
    return user_name;
}

const char * get_password()
{
    return password;
}

const char * get_host_name()
{
    return host_name;
}

MYSQL * get_sql_conn()
{
    MYSQL * conn = (MYSQL *) malloc(sizeof (MYSQL) * 1);

    mysql_init(conn);
    if(mysql_real_connect(conn,
                get_host_name(),
                get_user_name(),
                get_password(),
                get_auditdb_name(),
                0, /* use the standard port*/
                NULL, /* Unix socket, it should be the domain socket*/
                0 /* Client flags */)   == NULL)
    {
        /* Error in connecting to database */
        fprintf(stderr, "%s\n", mysql_error(conn));
        return NULL;
    }
    
    return conn;
}

void free_mysql_conn(MYSQL * conn)
{
    if(conn != NULL)
        mysql_close(conn);
}

char * allocate_query_space()
{
    memset(query, 0, 4096);
    return query;
}

void free_query_space(char * s)
{
    if(s == query)
        memset(s, 0, 4096);
}


// This can probably return a list of char *s
RETVAL exec_query(MYSQL * conn, char * query, MYSQL_RES ** res)
{
    RETVAL ret = NOTOK;
   
    *res = NULL;
    // Run the query and look for error 
    if(query == NULL)
        return NOTOK;
    
    if(mysql_query(conn, query))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        ret = NOTOK;
        return ret;
    }

    *res = mysql_store_result(conn);

     if(*res == NULL)
    {
        // There are other ways to check for this specific error 
        // use mysql_error_string, mysql_errno and mysql_field_count
         //
        if(mysql_field_count(conn) != 0)
        {
            fprintf(stderr,"Error occured when fetching the result set\n");
            ret = NOTOK;
            return ret;
        }
    }

    ret = OK;
    return ret;
}

const char * get_workspacetable_name()
{
    return "workspacetable";
}

const char * get_dotable_name()
{
    return "dotable";
}

const char * get_deptable_name()
{
    return "deptable";
}

// Expecting only one row from the result set
//  TODO May be pass a row to the function
unsigned int get_uint_from_resultset(MYSQL_RES * res)
{
    MYSQL_ROW row; /* Represents one row of the result set */
    my_ulonglong num_rows = 0;
    char * s_id = NULL;

    if(res == NULL)
        return -1;   
    
    num_rows = mysql_num_rows(res);
    if(num_rows > 1)
    {
            fprintf(stderr,
       		" More than one workspace with the same key,  which I don't like\n");
            return -1;
    }
    if(num_rows == 0)
    {
        return -1;
    }
    row = mysql_fetch_row(res);
    s_id = row[0];
    if(s_id == NULL)
        return -1;
    return (atoi(s_id));
}

RETVAL convert_uint_to_str(unsigned int val, char ** result)
{
    *result = NULL;
    *result = (char * ) (malloc (sizeof(char) * 20));
    memset(*result, 0, 20);
    itoa(val, *result, 10);	
    return OK;
}

char * itoa(unsigned int val, char * result, int base)
{
    int i = 0;
    if(val == 0)
    {
        result[0] = '0';
        result[1] = '\0';
    }
    else
    {
        while(val > 0)
        {
         int remainder = val % base;
         char c = remainder+'0';
         result[i] = c;
         i++;
         val = val/base;
        }
        result[i] = '\0';
    }
     strrev(result);
    return result;
}

char * strrev(char * str)
{
    int len = strlen(str);
    int  i = 0;
    int j = len - 1;
    if(len == 0)
    {
        str[0] = '\0';
        return str;
    }
    while(i<=j) // i !=j was the lousiest code ever !
    {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
    return str;
}

char ** get_strings_from_resultset(MYSQL_RES *res, MYSQL_ROW row)
{
    int num_fields = 0;
    char ** result = NULL;
    int i = 0;
    unsigned long *lengths = 0; // Array of lengths for the current row */

    if(row == NULL || res == NULL)
        goto done;
    
    num_fields = mysql_num_fields(res); //one for NULL
    result = (char **) (malloc(sizeof(char *) * (num_fields+1) ));
    memset(result, 0, num_fields+1);

    lengths = mysql_fetch_lengths(res);
    for(i = 0 ; i < num_fields; i++)
    {
        // Store these values into strings and then covert to uint64 as 
        // needed
        result[i] = (char *) (malloc(sizeof(char) * (lengths[i]+1)));
        memset(result[i], 0, (lengths[i]+1));
        if(row[i] != NULL)
        {
            strncpy(result[i], row[i], lengths[i]);
        }
    }
    result[i] = NULL;
 done:
    if(result == NULL)
        fprintf(stderr, "Error occured into fetching strings from result set\n");
    return result;
}

void free_resultset(char ** result)
{
    int i = 0;
    if(result == NULL)
        return;
    
    while(result[i] != NULL)
    {
        if(result[i] != NULL)
            free(result[i]);
        i++;
    }
}