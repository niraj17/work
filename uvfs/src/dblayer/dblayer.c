#include <stdlib.h>
#include <string.h>
#include "dblayer.h"
#include "dbcommon.h"
#include <mysql.h>
#include <stdio.h>

WORKSPACEID get_workspaceid_from_resultset(MYSQL_RES * res);
DEPID get_depid_from_resultset(MYSQL_RES * res);
DOID get_doid_from_resultset(MYSQL_RES * res);

workspace_record * unmarshall_workspace_record(char ** result);
do_record * unmarshall_do_record(char ** result);
dep_record * unmarshall_dep_record(char ** result);

DOID enter_do_record(char * do_name, 
                                char *do_path, 
                                unsigned int size,
                                unsigned int mtime,
                                WORKSPACEID workspace_id)
{
    MYSQL * conn = get_sql_conn();
    MYSQL_RES * res = NULL; //represents the result set
    const char * table_name = get_dotable_name();
    char * query = NULL;
    char * str_wid = NULL;
    char * str_time = NULL;
    char * str_size = NULL;
    unsigned int doid = -1;

    if(conn == NULL)
        return -1;

    str_wid = (char *)malloc(STRWORKSPACEIDSIZE);
    memset(str_wid, 0, STRWORKSPACEIDSIZE);
    itoa(workspace_id,  str_wid, 10);

    // SELECT query 
    query = allocate_query_space();
    sprintf(query, "SELECT doid FROM %s WHERE name='%s' AND path='%s' AND wid='%s'",
                table_name, do_name, do_path, str_wid);

    //execute the query
    if(exec_query(conn, query, &res) != OK)
        goto done;

    if(res != NULL)
    {
        doid = get_doid_from_resultset(res);
        if(doid == -1 || doid > 0)
            goto done;

        //free the result
        mysql_free_result(res);
        res = NULL;
    }
    free_query_space(query);
    
    // Form the query 
    query = allocate_query_space();
    convert_uint_to_str(mtime, &str_time);
    convert_uint_to_str(size, &str_size);
    sprintf(query,"INSERT INTO %s VALUES  (-1, '%s', '%s', '%s', '%s', '%s')", 
        table_name, do_name, do_path, str_time, str_size, str_wid);

    if(exec_query(conn, query, &res) != OK)
    {
        doid = -1;
        goto done;
    }
    else
    {
        doid = mysql_insert_id(conn);
        goto done;
    }

    // TODO check error with res

    done:
    if(res != NULL)
        mysql_free_result(res);
    free_query_space(query);
    if(str_wid != NULL)
        free(str_wid);
    if(str_time != NULL)
        free(str_time);
    if(str_size != NULL)
        free(str_size);
    free_mysql_conn(conn);
    return doid;
}

DEPID enter_dep_record(char * dep_name, 
                            char * dep_path,
                            unsigned int size,
                            unsigned int mtime,
                            DOID doid,
                            WORKSPACEID wid)
{
    MYSQL * conn = get_sql_conn();
    MYSQL_RES * res = NULL; //represents the result set
    const char * table_name  = get_deptable_name();
    DEPID depid;
    char * query = NULL;
    char * str_size = NULL;
    char * str_time = NULL;
    char * str_doid = NULL;
    char * str_wid = NULL;

    if(conn == NULL)
        return -1;

    str_wid = (char *)malloc(STRWORKSPACEIDSIZE);
    memset(str_wid, 0, STRWORKSPACEIDSIZE);
    itoa(wid,  str_wid, 10);
    str_doid = (char *)malloc(STRDOIDSIZE);
    memset(str_doid, 0, STRDOIDSIZE);
    itoa(doid,  str_doid, 10);

    /* Form the query */    
    query = allocate_query_space();
    sprintf(query, "SELECT depid FROM %s WHERE name='%s'  AND path='%s' AND  doid='%s'  AND wid='%s'",
                table_name, dep_name, dep_path, str_doid, str_wid);

     //execute the query
    if(exec_query(conn, query, &res) != OK)
        goto done;
    if(res != NULL)
    {
        depid = get_depid_from_resultset(res);
        if(depid == -1 || depid > 0)
            goto done;
        //free the result
        mysql_free_result(res);
        res = NULL;
    }
   free_query_space(query);
    //Form the query again;
    query = allocate_query_space();
    convert_uint_to_str(mtime, &str_time);
    convert_uint_to_str(size, &str_size);
    
    sprintf(query,"INSERT INTO %s VALUES ('-1', '%s','%s', '%s', '%s', '%s', '%s')", 
        table_name , dep_name, dep_path, str_time, str_size, str_doid, str_wid);

    if(exec_query(conn, query, &res) != OK)
    {
        depid = -1;
        goto done;
    }
    else
    {
        depid = mysql_insert_id(conn);
        goto done;
    }

    done:
     if(res != NULL)
            mysql_free_result(res);
     if(str_time != NULL)
            free(str_time);
     if(str_size != NULL)
            free(str_size);
    if(str_wid != NULL);
            free(str_wid);
    if(str_doid != NULL);
            free(str_doid);
    free_query_space(query);   
    free_mysql_conn(conn);
    return depid;
}

WORKSPACEID enter_workspace_record(char * workspace_name, 
                                                            char * root_path
                                                          /*  char * machinename*/)
{
    // TODO Merge this code into get_workspace_id and avoid duplication
    MYSQL * conn = get_sql_conn();
    MYSQL_RES * res = NULL; //defines the result set
    const  char * table_name = get_workspacetable_name();
    char * query = allocate_query_space();
    unsigned int wid = -1;

    if(conn == (void *) NULL)
        goto done;

    sprintf(query, "SELECT wid FROM %s WHERE name='%s' AND path='%s'",
                  table_name, workspace_name, root_path);

    if(exec_query(conn, query, &res) != OK)
        goto done;

    if(res != NULL)
    {
        wid = get_workspaceid_from_resultset(res);
        if(wid == -1)
            goto done;
        else if (wid != 0)
            goto done;

        // Free result set before executing the next query
        // TODO is this needed , shouln't exec_query take care of this?
        mysql_free_result(res);
        res = NULL;
    }

    query = allocate_query_space();
    sprintf(query,"INSERT INTO %s VALUES (-1, '%s','%s')", table_name, 
                                workspace_name, root_path);
    
    if(exec_query(conn, query, &res) != OK)
    {
        wid = -1;
        goto done;
    }
    else
    {
        wid = mysql_insert_id(conn);
        goto done;
    }
    
    done:
    if(res != NULL)
            mysql_free_result(res);
    free_mysql_conn(conn);
    return wid;
}

// TODO Merge this code with fetch_workspace_record
// Return wid = -1 indicates error
// Positive wid means success
WORKSPACEID get_workspace_id(char * workspace_name, char * workspace_root)
{   
    WORKSPACEID wid = -1;
    MYSQL * conn = get_sql_conn();
    MYSQL_RES * res = NULL; //defines the result set
    const  char * table_name = get_workspacetable_name();
    char * query = allocate_query_space();

    if(conn == (void *) NULL)
        goto done;

    sprintf(query, "SELECT wid FROM %s WHERE name='%s' AND path='%s'",
                  table_name, workspace_name, workspace_root);

    if(exec_query(conn, query, &res) != OK)
           goto done;

    if(res == NULL)
    {
        // We have argued about this before, whether to put the error message into
        // the code that got the error. This needs deep analysis. Atleast ask in Stack
        // Overflow
           fprintf(stderr, " NULL result set\n");     
           goto done;
    }
    else
    {
        wid = get_workspaceid_from_resultset(res);
        if(wid == -1)
            goto done;
        else if (wid != 0)
            goto done;
    }

    done:
        if(res != NULL)
            mysql_free_result(res);
        if(conn != NULL)
            free_mysql_conn(conn);

    return wid;
}   

// TODO Merge this code with fetch_do_record
// TODO distinguish between a successful query and a failed query
// TODO In a successful query you may not have any result set. Need to
// distinguish that case.
DOID get_doid(char * do_name, char * do_path, WORKSPACEID wid)
{
    DOID doid = -1;
    MYSQL * conn = get_sql_conn();
    MYSQL_RES * res = NULL; //represents the result set
    const char * table_name = get_dotable_name();
    char * query = NULL;
    char * str_wid = NULL;
    
    if(conn == NULL)
        return -1;

    str_wid = (char *)malloc(STRWORKSPACEIDSIZE);
    memset(str_wid, 0, STRWORKSPACEIDSIZE);
    itoa(wid,  str_wid, 10);

    // SELECT query 
    query = allocate_query_space();
    sprintf(query, "SELECT doid FROM %s WHERE name='%s' AND path='%s' AND wid='%s'",
                table_name, do_name, do_path, str_wid);

    //execute the query
    if(exec_query(conn, query, &res) != OK)
        goto done;

    if(res != NULL)
    {
        doid = get_doid_from_resultset(res);
        if(doid == -1 || doid > 0)
            goto done;
    }

    done:
        if(res != NULL)
            mysql_free_result(res);
        if(conn != NULL)
            free_mysql_conn(conn);
    return doid;
}


DOID get_doid_from_resultset(MYSQL_RES * res)
{
    DOID id = get_uint_from_resultset(res);
    if(id == -1)
    {
       fprintf(stderr, "Error occured in fetching id from result set\n");
    }
    return id;
}

DEPID get_depid_from_resultset(MYSQL_RES * res)
{
    DEPID id = get_uint_from_resultset(res);
    if(id == -1)
    {
       fprintf(stderr, "Error occured in fetching id from result set\n");
    }
    return id;
}

WORKSPACEID get_workspaceid_from_resultset(MYSQL_RES * res)
{
     WORKSPACEID id = get_uint_from_resultset(res);
    if(id == -1)
    {
       fprintf(stderr, "Error occured in fetching id from result set\n");
    }
    return id;
}

workspace_record * fetch_workspace_record(WORKSPACEID wid)
{
    // Execute the select * query with wid=$(wid)
    MYSQL * conn = get_sql_conn();
    char * query = allocate_query_space();
    const char * table_name = get_workspacetable_name();
    char * str_wid = NULL;
    MYSQL_RES * res = NULL; //Represents a result set
    workspace_record * rec = NULL;
    char ** resultset = NULL;
        
    convert_uint_to_str(wid, &str_wid);    
    if(str_wid == NULL)
        goto done;
    
    sprintf(query, "SELECT * FROM %s WHERE wid='%s'",
        table_name, str_wid);
    // execute query
    if(exec_query(conn, query, &res) != OK)
        goto done;

    if(res == NULL)
    {   
        fprintf(stderr,"No result set with wid= %u\n", wid);
        goto done;
    }
    else
    {
        my_ulonglong num_rows = 0;
        MYSQL_ROW row = NULL;
        // Count the number of rows. It should be only one
        num_rows = mysql_num_rows(res);
        if(num_rows > 1)
        {
            fprintf(stderr, "More than one row found for wid %u. There is a error\
                in the database\n",wid);
            goto err;
        }
        row = mysql_fetch_row(res);
        resultset = get_strings_from_resultset(res, row);

        if(resultset == NULL)
            goto err;   

        rec = unmarshall_workspace_record(resultset);
        if (rec == NULL)
            goto err;
    }

    goto done;
    
    err:
        if(rec != NULL)
        {
            free(rec);
            rec = NULL;
        }
    done:
        if(res != NULL)
            mysql_free_result(res);
        free_query_space(query);
        if(str_wid != NULL)
            free(str_wid);
        free_mysql_conn(conn);
        free_resultset(resultset);
    return rec;
}

do_record * fetch_do_record(DOID doid, WORKSPACEID wid)
{
    MYSQL * conn = get_sql_conn();
    char * query = NULL;
    const char * table_name = get_dotable_name();
    char * str_wid = NULL;
    char * str_doid = NULL;
    MYSQL_RES * res = NULL; //Represents a result set
    MYSQL_RES * res1 = NULL;
    do_record * rec = NULL;
    char ** resultset = NULL;

    convert_uint_to_str(doid, &str_doid);
    convert_uint_to_str(wid, &str_wid);
    
    if(str_doid == NULL || str_wid == NULL)
        goto err;

    query = allocate_query_space();
    // Execute the query SELECT * from table where doid=%s and 
    sprintf(query, "SELECT * FROM %s WHERE doid='%s' AND  wid='%s'",
                table_name, str_doid, str_wid);

    if(exec_query(conn, query, &res) != OK)
        goto done;

    if(res == NULL)
    {   
        fprintf(stderr,"No result set with doid= %u and wid=%u\n", doid, wid);
        goto done;
    }
    else
    {
        my_ulonglong num_rows = 0;
        MYSQL_ROW row = NULL;
        //const char *dep_table_name = get_deptable_name(); 
        // Count the number of rows. It should be only one
        num_rows = mysql_num_rows(res);
        if(num_rows > 1)
        {
            fprintf(stderr, "More than one row found for wid %u. There is a error\
                in the database\n",wid);
            goto err;
        }
        row = mysql_fetch_row(res);
        resultset = get_strings_from_resultset(res, row);

        if(resultset == NULL)
            goto err;   

        rec = unmarshall_do_record(resultset);
        if (rec == NULL)
            goto err;

        // Execute the query to get the depids from this doid and wid
        // Since this is the second query, free
        free_query_space(query);
        query = allocate_query_space();

        sprintf(query, "SELECT depid FROM deptable WHERE doid='%s' AND wid='%s'",
            str_doid, str_wid);
        
        if(exec_query(conn, query, &res1) != OK)
        {
            fprintf(stdout, "Error executing query %s, no resultset\n", query); 
            goto done;
        }
        if(res1 == NULL)
        {
              fprintf(stderr,"NULL result set with doid= %u and wid=%u\n", doid, wid);
              goto done;
        }
        else
        {
            my_ulonglong num_rows = 0;
            MYSQL_ROW row = NULL;
            int i = 0;
            // Get the number of rows
            num_rows = mysql_num_rows(res1);
            if(num_rows == 0)
                goto done;

            rec->depids = (DEPID *)(malloc(sizeof(DEPID) * num_rows));
            memset(rec->depids, 0, sizeof(DEPID) * num_rows);
            
            while(i < num_rows)
            {
                row = mysql_fetch_row(res1);
                rec->depids[i] = atoi(row[0]);
                i++;
            }
            rec->dep_count = num_rows;
        }   
    }

    goto done;
    
    err:
        if(rec != NULL)
        {
            free(rec);
            rec = NULL;
        }
     done:
        if(str_wid != NULL)
            free(str_wid);
        if(str_doid != NULL)
            free(str_doid);
        if(conn != NULL)
            free_mysql_conn(conn);
        if(resultset != NULL)
            free_resultset(resultset);
        if(res != NULL)
            mysql_free_result(res);
        if(res1 != NULL)
            mysql_free_result(res1);
        free_query_space(query);
    return rec;
}

dep_record * fetch_dep_record(DEPID depid, DOID doid, WORKSPACEID wid)
{
    MYSQL * conn = get_sql_conn();
    char * query = NULL;
    const char * table_name = get_deptable_name();
    char * str_wid = NULL;
    char * str_doid = NULL;
    char * str_depid = NULL;
    MYSQL_RES * res = NULL; //Represents a result set
    dep_record * rec = NULL;
    char ** resultset = NULL;

    convert_uint_to_str(doid, &str_doid);
    convert_uint_to_str(wid, &str_wid);
    convert_uint_to_str(depid, &str_depid);
    
    if(str_doid == NULL || str_wid == NULL || str_depid == NULL)
        goto err;

    query = allocate_query_space();
    // Execute the query SELECT * from table where doid=%s and 
    sprintf(query, "SELECT * FROM %s WHERE depid='%s' AND doid='%s' AND  wid='%s'",
                table_name, str_depid, str_doid, str_wid);

    if(exec_query(conn, query, &res) != OK)
        goto done;

    if(res == NULL)
    {   
        fprintf(stderr,"No result set with depid=%u and doid= %u and wid=%u\n",
                    depid, doid, wid);
        goto done;
    }
    else
    {
        my_ulonglong num_rows = 0;
        MYSQL_ROW row = NULL;
        // Count the number of rows. It should be only one
        num_rows = mysql_num_rows(res);
        if(num_rows > 1)
        {
            fprintf(stderr, "More than one row found for wid %u. There is a error\
                in the database\n",wid);
            goto err;
        }
        row = mysql_fetch_row(res);
        resultset = get_strings_from_resultset(res, row);

        if(resultset == NULL)
            goto err;   

        rec = unmarshall_dep_record(resultset);
        if (rec == NULL)
            goto err;
    }

    goto done;
    
    err:
        if(rec != NULL)
        {
            free(rec);
            rec = NULL;
        }
     done:
        if(str_wid != NULL)
            free(str_wid);
        if(str_doid != NULL)
            free(str_doid);
        if(conn != NULL)
            free_mysql_conn(conn);
        if(resultset != NULL)
            free_resultset(resultset);
        free_query_space(query);
    return rec;
}

workspace_record * unmarshall_workspace_record(char ** result)
{
    int i =0;
    workspace_record * rec = (workspace_record *)malloc(sizeof(workspace_record));
    if (rec == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for workspace record\n");
        goto done;
    }

    memset(rec, 0, sizeof(workspace_record));
    while(result[i] != NULL)
    {
        switch (i)
        {
            case 0:
                rec->wid = atoi(result[i]);
                break;
            case 1:
                rec->workspace_name = strdup(result[i]);
                break;
            case 2:
                rec->workspace_root = strdup(result[i]);
                break;
            default:
                fprintf(stderr, "Unhandled case\n");
                goto done;
        }
        i++;
    }
    done:
    return  rec;
};

do_record * unmarshall_do_record(char ** result)
{
    int i =0;
    do_record * rec = (do_record *)malloc(sizeof(do_record));
    if (rec == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for do record\n");
        goto done;
    }

    memset(rec, 0, sizeof(do_record));
    while(result[i] != NULL)
    {
        char * tempstr = result[i];

        switch (i)
        {
            case 0:
                rec->doid = atoi(tempstr);
                break;
            case 1:
                rec->do_name = strdup(tempstr);
                break;
            case 2:
                rec->do_path = strdup(tempstr);
                break;
            case 3:
                rec->timestamp = atoi(tempstr);
                break;
            case 4:
                rec->size = atoi(tempstr);
                break;
            case 5:
                rec->wid = atoi(tempstr);
                break;
            default:
                fprintf(stderr, "Unhandled case\n");
                goto done;
        }
        i++;
    }
    done:
    return  rec;
};

dep_record * unmarshall_dep_record(char ** result)
{
    int i =0;
    dep_record * rec = (dep_record *)malloc(sizeof(dep_record));
    if (rec == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for dep record\n");
        goto done;
    }

    memset(rec, 0, sizeof(dep_record));
    while(result[i] != NULL)
    {
        char * tempstr = result[i];
        switch (i)
        {
            case 0:
                rec->depid = atoi(tempstr);
                break;
            case 1:
                rec->dep_name = strdup(tempstr);
                break;
            case 2:
                rec->dep_path = strdup(tempstr);
                break;
            case 3:
                rec->timestamp = atoi(tempstr);
                break;
            case 4:
                rec->size = atoi(tempstr);
                break;
            case 5:
                rec->doid = atoi(tempstr);
                break;
             case 6:
                rec->wid = atoi(tempstr);
                break;

            default:
                fprintf(stderr, "Unhandled case\n");
                goto done;
        }
        i++;
    }
    done:
    return  rec;
};

void dump_workspace_record(workspace_record * rec)
{
    if(rec == NULL)
        return;
    fprintf(stdout,"WID %u, ", rec->wid);
    fprintf(stdout, "Workspace name %s, ", rec->workspace_name);
    fprintf(stdout, "Workspace root %s, ", rec->workspace_root);
    fprintf(stdout, "\n");
    fflush(stdout);
}

void dump_do_record(do_record * rec)
{
    if(rec == NULL)
        return;
    fprintf(stdout, "DOID : %u, ", rec->doid);
    fprintf(stdout, "DO name : %s, ", rec->do_name);
    fprintf(stdout, "DO path : %s, ", rec->do_path);
    fprintf(stdout, "DO timestamp : %u, ", rec->timestamp);
    fprintf(stdout, "DO size : %u, ", rec->size);
    fprintf(stdout, "WID %u, ", rec->wid);
    {
        int i = 0;
        while( i < rec->dep_count)
        {
            fprintf(stdout, "DEPID : %u, ", rec->depids[i]);
            i++;
        }
    }
    fprintf(stdout, "\n");
}   


void dump_dep_record(dep_record * rec)
{
    if(rec == NULL)
    {
        fprintf(stderr, "Dep record is NULL. Did you mean this to happen?\n");
        return;
    }
    fprintf(stdout, "DEPID : %u, ", rec->depid);
    fprintf(stdout, "DEP name : %s, ", rec->dep_name);
    fprintf(stdout, "DEP path : %s, ", rec->dep_path);
    fprintf(stdout, "DEP timestamp : %u, ", rec->timestamp);
    fprintf(stdout, "DEP size : %u  ", rec->size);
    fprintf(stdout, "DEP doid : %u, " , rec->doid);
    fprintf(stdout, "DEP wid : %u, " , rec->wid);
    fprintf(stdout, "\n");
    fflush(stdout);
}

DEPID * do_record_depids(do_record * dorec)
{
    return dorec->depids;
}

unsigned int do_record_depcount(do_record * dorec)
{
    return dorec->dep_count;
}

const char *  dep_record_name(dep_record * deprec)
{
    return (const char *) (deprec->dep_name);
}


WORKSPACEID * get_all_workspace_ids()
{
    MYSQL * conn = get_sql_conn();
    char * query = NULL;
    MYSQL_RES * res = NULL;
    const char * table_name = get_workspacetable_name();
    WORKSPACEID  * wids = NULL;
    // Form the query to get all workspace ids and then fill up wids

    query = allocate_query_space();
    sprintf(query, "SELECT wid FROM %s", table_name);

    if(exec_query(conn, query, &res) != OK)
    {
        printf("Exec query failed\n");
        fflush(stdout);
        goto done;
    }

    if(res == NULL)
    {   
        fprintf(stderr,"No result set for query %s",query);
        goto done;
    }
    else
    {
        my_ulonglong num_rows = 0;
        MYSQL_ROW row = NULL;
        int i = 0;
        // Get the number of rows
        num_rows = mysql_num_rows(res);
        if(num_rows == 0)
        {
            goto done;
        }

        wids = (WORKSPACEID * ) (malloc(sizeof(WORKSPACEID) * (num_rows+1) ));
        memset(wids, 0, (num_rows + 1));
            
        while(i < num_rows)
        {
            row = mysql_fetch_row(res);
            wids[i] = atoi(row[0]);
            i++;
        }
        wids[i] = (WORKSPACEID)-1;    
    }

    done:
        if(conn != NULL)
            free_mysql_conn(conn);
        if(query != NULL)
            free_query_space(query);
        if(res != NULL)
            mysql_free_result(res);
    return  wids;
}

WORKSPACEID do_record_get_wid(do_record * dorec)
{
    if(dorec == NULL)
        return -1;
    return dorec->wid;
}

const char * wrec_get_workspace_root(workspace_record * wrec)
{
    if(wrec == NULL)
        return NULL;
    return (const char * )wrec->workspace_root;
}

const char * do_record_get_path(do_record * dorec)
{
    if(dorec == NULL)
        return NULL;
    return (const char * ) dorec->do_path;
}
