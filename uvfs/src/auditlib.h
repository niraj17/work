/*
 *  Audit interface : Wrapper onto real audit mechanism
 *  Version 0.10  
 *
 *
 */
#ifndef _audit_h_
#define _audit_h_


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include<gcrypt.h>

#define MAX_AUDIT  1024  /* Maximum number of entry in audit record*/
#define MAX_DO     512   /* Maximum number of possible DO in one target*/

#define AUDIT_PATH_MAX 512 /* Max length of PATH */

#define AUDIT_HASH_ALGO   GCRY_MD_SHA1  /* this is defined in gcrypt.h */
#define AUDIT_HASH_SIZE   20           /* Size of SHA1 hash */

struct audit_info
{
    int flags;
    char dot_audit_path[AUDIT_PATH_MAX];
    int  dot_audit_fd;
    char audit_file_path[AUDIT_PATH_MAX]; /* file which will contain audit data*/
    char audit_mount_dir[AUDIT_PATH_MAX];
    char workspace_path[AUDIT_PATH_MAX]; /* workspace root dir. For now, it's same as mount point
					   of uvfs*/

};

/* required for parsing and handling audit log file */
enum audit_ops {
    AUDIT_READ,
    AUDIT_WRITE,
    AUDIT_CREAT,
    AUDIT_UNLINK
};
enum audit_type {
    AUDIT_DEPS,
    AUDIT_DO,
};

struct audit_data  /* Audit record */
{
    int type;           /* 0 - dependency, 1 - DO */
    unsigned long    st_size;
    char hash[AUDIT_HASH_SIZE];  
    char path[AUDIT_PATH_MAX];
};
struct audit_record
{
    int length;                       /* length of record in the database*/
    int flags;
    char workspace_path[AUDIT_PATH_MAX];             /* workspace root dir*/
    char target_relative_path[AUDIT_PATH_MAX];       /* target path, relative to workspace*/

    int count;                        /* number of audit_data entries */
    struct audit_data ad[MAX_AUDIT];  /* dependency touched and DO created
					 while building this target*/
};

struct audit_record ar;  /* Global variable, should be one per-thread */
int    debug_auditlib;   /* zero means no debugging */

struct _audit_ops 
{
    int version;

    int (*init)(struct audit_info *data);
    int (*set_debug_level)(int debug);
    int (*start_audit)(struct audit_info *data);
    int (*read_audit_data)(char *target_relative_path, struct audit_info *data);
    int (*stop_audit)(struct audit_info *data);
    int (*set_audit_param)(const char *name, const char *value);
    int (*remove_audit_param)(const char *name);

    /* TODO: these ops should be moved to some other place */
    int (*get_audit_hash)(const char *path, char *out, int len);
};


#endif /* _audit_h_ */

