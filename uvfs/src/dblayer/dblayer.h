/* We want  the following features
 * API to enter a audit record from DB
 * API to fetch audit record into DB
 * API to compare audit records
 * Users are never allowed to inspect audit records
 * To ensure this ask them to pass a handle of type audit_handle
 */
#ifndef __DBLAYER_H
#define __DBLAYER_H

#include <mysql.h>
#include "deps.h"
#include "dos.h"
#include "workspace.h"

typedef unsigned int WORKSPACEID;
typedef unsigned int DOID;
typedef unsigned int DEPID;

// The following data structures represent one row in the database
typedef struct{
    WORKSPACEID wid;
    char * workspace_name;
    char * workspace_root;
    char * host_name;
}workspace_record;

typedef struct{
    DOID doid;
    char * do_name;
    char * do_path;
    unsigned timestamp;
    unsigned size;
    WORKSPACEID wid;
    DEPID * depids; 
    unsigned int dep_count;
}do_record;

typedef struct{
    DEPID depid;
    char * dep_name;
    char * dep_path;
    unsigned timestamp;
    unsigned size;
    DOID doid;
    WORKSPACEID wid;
}dep_record;

WORKSPACEID enter_workspace_record(char * workspace_name, 
                                                            char * root_path
                                                          /*  char * machinename*/);
DEPID enter_dep_record(char * dep_name, 
                            char * dep_path,
                            unsigned int size,
                            unsigned int time,
                            DOID doid,
                            WORKSPACEID wid);

DOID enter_do_record(char * do_name, 
                                char *do_path, 
                                unsigned int size,
                                unsigned int time,
                                WORKSPACEID workspace_id);

workspace_record * fetch_workspace_record(WORKSPACEID wid);
do_record * fetch_do_record(DOID doid, WORKSPACEID wid);
dep_record * fetch_dep_record(DEPID depid, DOID doid, WORKSPACEID wid);

WORKSPACEID get_workspace_id(char * workspace_name, char * workspace_root);
DOID get_doid(char * do_name, char* do_path, WORKSPACEID wid);

//Freed by the caller, ends with a -1 to denote end of array
WORKSPACEID * get_all_workspace_ids();

void free_do_record(do_record * dorec);
void free_dep_record(dep_record * deprec);
void free_workspace_record(workspace_record * wrec);

/* Do initialization work on dll entry */
void db_init();

// Accessor functions...methods giving  copy of the members of dep_record
// do_record and workspace_record
DEPID * do_record_depids(do_record * dorec);
unsigned int do_record_depcount(do_record * dorec);
const char *  dep_record_name(dep_record * deprec);
WORKSPACEID do_record_get_wid(do_record * dorec);
const char * wrec_get_workspace_root(workspace_record * wrec);
const char * do_record_get_path(do_record * dorec);

/* Diagnostics */
void dump_workspace_record(workspace_record * rec);
void dump_do_record(do_record * rec);
void dump_dep_record(dep_record * rec);

#endif
