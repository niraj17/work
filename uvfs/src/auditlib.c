/*
 *  Audit library : Wrapper onto real audit interface
 *  Version 0.10  
 *
 * This should be compiled as a shared object. 
 * This will be loaded at runtime (dlopen) by make utility.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <libgen.h>

#include<gcrypt.h>

#include "auditlib.h"

#define MAX_CREAT   32
#define MAX_UNLINK  32

int bcfs_set_debug_level(int debug)
{
    debug_auditlib = debug;
    return 0;
}
int bcfs_audit_init(struct audit_info *data)
{
    char cwd[AUDIT_PATH_MAX];
    char *dir = NULL;
    struct audit_info *ai = (struct audit_info *) data;
    if (debug_auditlib) printf("audit_init called\n");
    if (getcwd(cwd, AUDIT_PATH_MAX) == NULL) {
        return errno;
    }
    if (debug_auditlib) printf("cwd=%s\n", cwd);

//  TODO : Why are we searching one level up?
    /*dir = dirname(cwd); */
    dir = cwd;
    while (1) {
        if (strcmp(dir, "/") == 0) break;
        sprintf(ai->dot_audit_path, "%s/.audit", dir);
        if (debug_auditlib) printf("dot_audit_path=%s\n", ai->dot_audit_path);
        ai->dot_audit_fd = open(ai->dot_audit_path, O_RDWR | O_APPEND);
        if (ai->dot_audit_fd > 0) {
            memset(ai->audit_mount_dir, 0, AUDIT_PATH_MAX);
            strcpy(ai->audit_mount_dir, dir);
            break;
        }
        dir = dirname(dir);
    }
    /* save the mount point, assume this as root of workspace, for now*/
    sprintf(ai->workspace_path, "%s", dir);
    if (debug_auditlib) printf("workspace path=%s\n", ai->workspace_path);
  
    if (ai->dot_audit_fd > 0) return 0;
    
    return 1;
}
int bcfs_start_audit(struct audit_info *data)
{
    struct audit_info *ai = (struct audit_info *) data;
    char buf[AUDIT_PATH_MAX];
    int pid = getpid();
    int flag = 0, len = 0;

    if (debug_auditlib) printf("start_audit called\n");
    len = sprintf(buf, "%d|%d|%s\n", pid, flag, ai->audit_file_path);
    if (debug_auditlib) printf("buf=%s", buf);
    if (write(ai->dot_audit_fd, buf, len) < 0) {
        perror("start_audit: write failed");
	return errno;
    }
    return 0;
}

/*  Read a line from a file  descriptor */
/* Returns with newline char removed */
ssize_t readline(int sockd, void *vptr, size_t maxlen) {
    ssize_t n, rc;
    char    c, *buffer;

    buffer = vptr;

    for ( n = 1; n < maxlen; n++ ) {
        
        if ( (rc = read(sockd, &c, 1)) == 1 ) {
            if ( c == '\n' ) {
                *buffer++ = 0;
                break;
	    } else {
                *buffer++ = c;
	    }
        }
        else if ( rc == 0 ) {
            if ( n == 1 )
                return 0;
            else
                break;
        }
        else {
            if ( errno == EINTR )
                continue;
            return -1;
        }
    }

    *buffer = 0;
    return n;
}

int calculate_file_hash(char *path, char *out, int len)
{
    int fd, i, length=0;
    unsigned char *digest;
    char buffer[4096];
    gcry_md_hd_t hd;

    gcry_md_open (&hd, AUDIT_HASH_ALGO, NULL);

    if( (fd = open(path, O_RDONLY)) < 0) {
        printf("Cannot open %s\n", path);
        return (2);
    }

    while ((length = read(fd, buffer, 4096)) > 0) {
        gcry_md_write (hd, buffer, length);
    }

    digest = gcry_md_read(hd, AUDIT_HASH_ALGO);

    for (i = 0; i < gcry_md_get_algo_dlen(AUDIT_HASH_ALGO); i++) {

        if (debug_auditlib) printf("%02x", digest[i]);

	*out = digest[i];
	out++;
    }

    if (debug_auditlib) printf("\n");

    gcry_md_close(hd);

    close(fd);

    return 0;
}


/* This reads the audit data into a global variable */
int bcfs_read_audit_data(char *target_relative_path, struct audit_info *data)
{
    struct audit_info *ai = (struct audit_info *) data;
    int fd = 0, i =0, j =0, ret=0;
    char line[AUDIT_PATH_MAX], *c = NULL, full_path[AUDIT_PATH_MAX];
    struct stat st_buf;
    int already_found = 0;

    int creat_count = 0, creat_found = 0;
    char creat_list[MAX_CREAT][AUDIT_PATH_MAX]; /* list of files created */
    int unlink_count = 0, unlink_found = 0;
    char unlink_list[MAX_CREAT][AUDIT_PATH_MAX]; /* list of files unlinked */

    if (debug_auditlib) printf("bcfs_read_audit_data  called\n");
    sprintf(ar.workspace_path, "%s", ai->workspace_path);
    sprintf(ar.target_relative_path, "%s", target_relative_path);
 
    if (debug_auditlib) printf("bcfs_read_audit_data  called: audit_file_path=%s\n",
           ai->audit_file_path);

    fd  = open(ai->audit_file_path, O_RDONLY);
    if (fd < 0) {
        perror("write failed");
        return 1;
    }

    ar.count = 0;

    while (readline(fd, line, AUDIT_PATH_MAX) > 0) {
	creat_found = 0;
	already_found = 0;

        c   = strtok(line, ":"); /*a_pid*/
        if (c == NULL) return 0;
        c   = strtok(NULL, ":"); /*pid*/
        if (c == NULL) return 0;

        c   = strtok(NULL, ":"); /*ops*/
        if (c == NULL) return 0;

	if (strcmp (c, "creat") == 0) {
            c = strtok(NULL, ":"); /*path*/
            if (c == NULL) continue;
	    if (creat_count < MAX_CREAT) {
                strcpy(creat_list[creat_count], c);
                if (debug_auditlib) printf("Added to creat list: %s\n", creat_list[creat_count]);
	        creat_count++;
	    } else {
		printf("Warning: MAX create count exceeded. Target=%s\n",target_relative_path);
	    }
	    continue;
	}
	if (strcmp (c, "unlink") == 0) {
            c = strtok(NULL, ":"); /*path*/
            if (c == NULL) continue;
	    if (unlink_count < MAX_UNLINK) {
                strcpy(unlink_list[unlink_count], c);
                if (debug_auditlib) printf("Added to unlink list: %s\n", unlink_list[unlink_count]);
	        unlink_count++;
	    } else {
		printf("Warning: MAX unlink count exceeded. Target=%s\n", target_relative_path);
	    }
	    continue;
	}

        if (strcmp(c,"read") == 0)
        {
	    ar.ad[ar.count].type = AUDIT_DEPS;
            c = strtok(NULL, ":"); /*path*/
            if (c == NULL) return 0;

	    /* If this file was created in this build, then it should
	       not be included as a dependency */
	    for (i = 0; i < creat_count; i++)
	    {
		if (strcmp(creat_list[i], c) == 0) {
		    creat_found = 1;
                    if (debug_auditlib) printf("creat_found: %s\n", c);
		    break;
		}
	    }
	    if (creat_found) continue;

	    /* If this file has already been included as a dependency,
	       then skip it */
	    for (i = 0; i < ar.count; i++)
	    {
		if (strcmp(ar.ad[i].path, c) == 0) {
		    already_found = 1;
                    if (debug_auditlib) printf("already_found: %s\n", c);
		    break;
		}
	    }
	    if (already_found) continue;

            strcpy(ar.ad[ar.count].path, c);

	    /* skip if this file is same as the target being
	       built */
	    if (strcmp(ar.ad[ar.count].path + 1, target_relative_path) == 0) continue;

	    sprintf(full_path, "%s%s", ar.workspace_path, ar.ad[ar.count].path);
	    if (debug_auditlib) printf("full_path = %send\n", full_path);
	    if (stat(full_path, &st_buf) == 0) {
               ar.ad[ar.count].st_size  = st_buf.st_size;
	       if (debug_auditlib) printf("size is %d\n", ar.ad[ar.count].st_size);
	    }

	    /* Calculate a checksum (MD5) for the file and store it*/
	    calculate_file_hash(full_path, ar.ad[ar.count].hash, AUDIT_HASH_SIZE);

            if (debug_auditlib) printf("ar.ad.path = %s\n", ar.ad[ar.count].path);
            ar.count++;
	    if (ar.count == MAX_AUDIT) {
                printf("Warning : MAX audit limit exceeded: Target = %s\n",
			target_relative_path);
		break;
	    }
        }
    }


    /* files which are created but not unlinked should be listed as DO*/
    for (i = 0; i < creat_count; i++) {
	unlink_found = 0;
        for (j = 0; j < unlink_count; j++) {
	    if (strcmp(creat_list[i], unlink_list[j]) == 0) {
	        unlink_found = 1;
	        break;
	    }
	}
	if (unlink_count == 0) {
	    if ( ar.count == MAX_AUDIT) { 
                printf("Warning : MAX AUDIT limit exceeded at DO: Target = %s\n",
			target_relative_path);
	    }
	    /* TODO: add creat_list[i] as a DO */
	    ar.ad[ar.count].type = AUDIT_DO;
	    sprintf(ar.ad[ar.count].path, "%s", creat_list[i]);
	    ar.count++;
            if (debug_auditlib) printf("Added DO : %s\n", creat_list[i]);
	}
    }


    ar.length = 3 * sizeof(int) + 2 * AUDIT_PATH_MAX + ar.count * sizeof(struct audit_data);

    close(fd);
    if (debug_auditlib) printf("ar.count = %d\n",ar.count);
    return 0;
}
int bcfs_stop_audit(struct audit_info *data)
{
    struct audit_info *ai = (struct audit_info *) data;
    char buf[AUDIT_PATH_MAX];
    int pid = getpid();
    int flag = 0, len = 0;

    if (debug_auditlib) printf("stop_audit called\n");
    len = sprintf(buf, "-%d\n", pid);
    if (write(ai->dot_audit_fd, buf, len) < 0) {
        perror("stop_audit: write failed");
	return errno;
    }
    return 0;
}
int bcfs_set_audit_param(const char *name, const char *value)
{
    return 0;
}
int bcfs_remove_audit_param(const char *name)
{
    return 0;
}

struct _audit_ops  audit_ops = {
     .init = bcfs_audit_init,   
     .set_debug_level = bcfs_set_debug_level,
     .start_audit = bcfs_start_audit,
     .read_audit_data = bcfs_read_audit_data,
     .stop_audit = bcfs_stop_audit,
     .set_audit_param = bcfs_set_audit_param,
     .remove_audit_param = bcfs_remove_audit_param,

     /* TODO : these ops should be moved to some other struct ...*/
     .get_audit_hash  =  calculate_file_hash
    
};
