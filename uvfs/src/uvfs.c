/*
 *  UVFS - Userspace Version File System
 *  Version 0.10  
 *
 * A basic implementation for auditing filesystem activities
 * Based on FUSE (Filesystem in USErspace) framework.
 *
 * Niraj Kumar  <niraj17@gmail.com>
 */

#define FUSE_USE_VERSION 26 /* Use latest API */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

#include"hashtable.h"

struct uvfs {
         char *rootdir;
	 struct fuse_args uvfs_args;

	 char auditfile[8]; /* /.audit */
	 struct hashtable *audit_table; /* info in .audit file */

	 struct hashtable *proc_table; 
	 int log_fd; /* file descriptor for log file*/

	 int debug;
	 int foreground;
};

/* One instance of audit_info will be created for every entry in .audit file.
 * Used as value in the HashTable audit_table (key is pid)
 */
struct audit_info {
    int audit_ops;
    int audit_log_fd;
    char *audit_log_path;
};

/* One instance of proc_info will be created for every process that enters
 * uvfs.
 * Used as value in the HashTable proc_table (key is pid)
 */
struct proc_info {
    int    audit_enabled;
    pid_t  a_pid;    /* Ancestor pid */
};


static struct uvfs uvfs;

enum {
	KEY_HELP,
	KEY_VERSION,
	KEY_FOREGROUND,
};

#define UVFS_OPT(t, p, v) { t, offsetof(struct uvfs, p), v }

static struct fuse_opt uvfs_opts[] = {
        UVFS_OPT("rootdir=%s",     rootdir, 0),

	FUSE_OPT_KEY("-V",             KEY_VERSION),
	FUSE_OPT_KEY("--version",      KEY_VERSION),
	FUSE_OPT_KEY("-h",             KEY_HELP),
	FUSE_OPT_KEY("--help",         KEY_HELP),
	FUSE_OPT_KEY("debug",          KEY_FOREGROUND),
	FUSE_OPT_KEY("-d",             KEY_FOREGROUND),
	FUSE_OPT_KEY("-f",             KEY_FOREGROUND),
        FUSE_OPT_END
};

pid_t get_ppid(pid_t pid)
{
    int fd, res, ppid;
    char proc_path[PATH_MAX];
    char *c, buffer[1024];

    snprintf(proc_path, sizeof(proc_path), "%s%d%s", "/proc/", pid, "/stat"); 
    if ((fd = open(proc_path, O_RDONLY)) < 0)
	return 0;

    res = read(fd, buffer, sizeof(buffer));
    if (res <= 0) return 0;
    close(fd);

    c   = strtok(buffer, " "); /*pid*/
    if (c == NULL) return 0;
    c   = strtok(NULL, " "); /* name */
    if (c == NULL) return 0;
    c   = strtok(NULL, " "); /* ?? */
    if (c == NULL) return 0;
    c   = strtok(NULL, " "); /* ppid */
    if (c == NULL) return 0;

    ppid = strtol(c, NULL, 0);

    fprintf(stderr,"get_ppid: pid=%d, ppid=%d \n", pid, (int) strtol(c, NULL, 0));
    return ppid;

}

int is_ancestor(pid_t pid, pid_t a_pid)
{
    pid_t cur_pid = pid;

    if (pid == a_pid) 
	return 1;

    while( cur_pid > 1) { /* pid 1 is parent of all*/
         cur_pid = get_ppid(cur_pid);
	 if (cur_pid == a_pid) return 1;
    }

    return 0;
}
int init_proc_context()
{
    struct proc_info *pi = NULL;
    struct fuse_context *fc;
    pid_t mypid, cur_pid;
    void *value = NULL;
    int found = 0;

    fc = fuse_get_context();
    cur_pid = mypid = fc->pid;

    fprintf(stderr, "init_proc_context: mypid=%d\n", mypid);

    if( hashtable_lookup(uvfs.proc_table, mypid) != NULL) {
        fprintf(stderr, "init_proc_context: this process already initilized\n");
	return 1;
    }

    pi = malloc(sizeof (struct proc_info));

    while(cur_pid > 1) {
        if( (value = hashtable_lookup(uvfs.audit_table, cur_pid)) == NULL)
        {
	    cur_pid = get_ppid(cur_pid);
        }
	else {
            fprintf(stderr,"init_proc_context: found \n");
	    pi->audit_enabled = 1;
	    pi->a_pid = cur_pid;
	    /*memcpy(pi, value, sizeof(struct proc_info));*/
	    found = 1;
	    break;
	}
    }

    /* Not found, set as not being audited */
    if( ! found) {
        fprintf(stderr,"init_proc_context:  not found \n");
	pi->audit_enabled = 0;
        pi->a_pid = mypid;
    }

    hashtable_insert(uvfs.proc_table, mypid, pi);
    return 1;
}

int do_audit(const char *action, const char *path)
{
    struct fuse_context *fc;
    pid_t a_pid, mypid;
    void *value = NULL;
    int  length = 0;
    char buffer[PATH_MAX];

    fc = fuse_get_context();
    mypid = fc->pid;

    fprintf(stderr, "do_audit\n");

    if((value = hashtable_lookup(uvfs.proc_table, mypid)) == NULL)
	return 1;  /* should not happen*/

    a_pid = ((struct proc_info *)value)->a_pid;

    if ( ((struct proc_info *)value)->audit_enabled) {
        if( (value = hashtable_lookup(uvfs.audit_table,
			a_pid)) != NULL)
        {
	    /* write log file*/
            fprintf(stderr, "do_audit : writting log file\n");
	    length = snprintf(buffer, sizeof(buffer), "%d:%d:%s:%s\n", a_pid, mypid, action, path);
	    write(((struct audit_info *)value)->audit_log_fd, buffer, length);
        }
    }
    return 1;
}

void hash_to_string(int key, int value, int buf)
{
    char pid[16],ops[16];
    struct audit_info *ai = (struct audit_info *)value;

    snprintf(pid, sizeof(pid), "%d", (pid_t) key);
    snprintf(ops, sizeof(ops), "%#x", ai->audit_ops);

    strcat((char *)buf, pid);
    strcat((char *)buf, "|");
    strcat((char *)buf, ops);
    strcat((char *)buf, "|");
    fprintf(stderr,"audt_log_path=%s.\n",  ai->audit_log_path);
    strcat((char *)buf, ai->audit_log_path);
    strcat((char *)buf, "\n");
}

void hash_to_size(int key, int value, int count)
{
    char pid[16],ops[16];
    struct audit_info *ai = (struct audit_info *)value;
    int  *size  = (int *) count;
    int len1, len2;

    len1 = snprintf(pid, sizeof(pid), "%d", (pid_t) key);
    len2 = snprintf(ops, sizeof(ops), "%#x", ai->audit_ops);

    *size =  *size + len1 + len2 + 
	strlen(ai->audit_log_path) + 3; /* 2 seperator and newline */ 
}
int hash_to_buffer_size(struct hashtable *hash)
{
    int count = 0;
    hashtable_foreach(uvfs.audit_table, hash_to_size, &count);
    return count;
}

/************************************************************
**** FUSE callback functions begin***************************
 ************************************************************/
static int uvfs_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char realpath[PATH_MAX];

       /* Add entry for special file .audit */
       if (strcmp (path, uvfs.auditfile) == 0) {
               stbuf->st_ino = 1; /* TODO: ??*/
               stbuf->st_mode = S_IFREG | S_IRUSR | S_IWUSR | S_IRGRP |
                   S_IWGRP | S_IROTH;
               stbuf->st_uid  = geteuid();
               stbuf->st_gid  = getegid();
	       stbuf->st_size = hash_to_buffer_size(uvfs.audit_table);
               return 0;
       }

	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, sizeof(realpath), "%s%s", uvfs.rootdir, path);
	} else {
	    snprintf(realpath, sizeof(realpath), "%s", path);
	}


	res = lstat(realpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}
static int uvfs_fgetattr(const char *path, struct stat *stbuf,
	struct fuse_file_info *fi)
{
	int res;

       /* Add entry for special file .audit */
       if (strcmp (path, uvfs.auditfile) == 0) {
               stbuf->st_ino = 1; /* TODO: ??*/
               stbuf->st_mode = S_IFREG | S_IRUSR | S_IWUSR | S_IRGRP |
                   S_IWGRP | S_IROTH;
               stbuf->st_uid  = geteuid();
               stbuf->st_gid  = getegid();
	       stbuf->st_size = hash_to_buffer_size(uvfs.audit_table);
               return 0;
       }

        res = fstat(fi->fh, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int uvfs_access(const char *path, int mask)
{
	int res;
	char realpath[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	} else {
	    snprintf(realpath, sizeof(realpath), "%s", path);
	}

	res = access(realpath, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static int uvfs_readlink(const char *path, char *buf, size_t size)
{
	int res;
	char realpath[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	} else {
	    snprintf(realpath, sizeof(realpath), "%s", path);
	}

	res = readlink(realpath, buf, size - 1);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	fprintf(stderr, "readlink=%s.\n", buf);
	return 0;
}


static int uvfs_opendir(const char *path, struct fuse_file_info *fi)
{
        DIR *dp = NULL;
	char realpath[PATH_MAX];

	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	} else {
	    snprintf(realpath, sizeof(realpath), "%s", path);
	}

        dp = opendir(realpath);
        if (dp == NULL)
                return -errno;

        fi->fh = (unsigned long) dp;

        return 0;
}

static int uvfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	DIR *dp = NULL;
	struct dirent *de;
        struct stat audit_st;

       /* Add entry for special file .audit */
       if (strcmp (path, "/") == 0) {
               memset(&audit_st, 0, sizeof(audit_st));
               audit_st.st_ino = 1;
               audit_st.st_mode = 8 << 12; /*DT_REG*/
               filler(buf, uvfs.auditfile+1, &audit_st, 0);
       }

	dp = (DIR *)(uintptr_t) fi->fh;
	seekdir(dp, offset);

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
	        if (filler(buf, de->d_name, &st, telldir(dp)))
	                    break;
	}

	return 0;
}
static int uvfs_releasedir(const char *path, struct fuse_file_info *fi)
{
        DIR *dp = (DIR *)(uintptr_t) fi->fh;
        closedir(dp);
        return 0;
}


static int uvfs_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;
	char realpath[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	} else {
	    snprintf(realpath, sizeof(realpath), "%s", path);
	}

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(realpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(realpath, mode);
	else
		res = mknod(realpath, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int uvfs_mkdir(const char *path, mode_t mode)
{
	int res;
	char realpath[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	} else {
	    snprintf(realpath, sizeof(realpath), "%s", path);
	}

	res = mkdir(realpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int uvfs_unlink(const char *path)
{
	int res;
	char realpath[PATH_MAX];

       /* Handle special file .audit */
       if (strcmp (path, uvfs.auditfile) == 0) {
               return -EPERM;
       }

	init_proc_context();

	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	} else {
	    snprintf(realpath, sizeof(realpath), "%s", path);
	}

	res = unlink(realpath);
	if (res == -1)
		return -errno;

	do_audit("unlink", path);

	return 0;
}

static int uvfs_rmdir(const char *path)
{
	int res;
	char realpath[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	} else {
	    snprintf(realpath, sizeof(realpath), "%s", path);
	}

	res = rmdir(realpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int uvfs_symlink(const char *from, const char *to)
{
	int res;
	char realpath_to[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath_to, PATH_MAX, "%s%s", uvfs.rootdir, to);
	} else {
	    snprintf(realpath_to, sizeof(realpath), "%s", to);
	}

	res = symlink(from, realpath_to);
	if (res == -1)
		return -errno;

	return 0;
}

static int uvfs_rename(const char *from, const char *to)
{
	int res;
	char realpath_from[PATH_MAX];
	char realpath_to[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath_from, PATH_MAX, "%s%s", uvfs.rootdir, from);
	    snprintf(realpath_to, PATH_MAX, "%s%s", uvfs.rootdir, to);
	} else {
	    snprintf(realpath_from, sizeof(realpath_from), "%s", from);
	    snprintf(realpath_to, sizeof(realpath_to), "%s", to);
	}

	res = rename(realpath_from, realpath_to);
	if (res == -1)
		return -errno;

	return 0;
}

static int uvfs_link(const char *from, const char *to)
{
	int res;
	char realpath_from[PATH_MAX];
	char realpath_to[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath_from, PATH_MAX, "%s%s", uvfs.rootdir, from);
	    snprintf(realpath_to, PATH_MAX, "%s%s", uvfs.rootdir, to);
	} else {
	    snprintf(realpath_from, sizeof(realpath_from), "%s", from);
	    snprintf(realpath_to, sizeof(realpath_to), "%s", to);
	}

	res = link(realpath_from, realpath_to);
	if (res == -1)
		return -errno;

	return 0;
}

static int uvfs_chmod(const char *path, mode_t mode)
{
	int res;
	char realpath[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	} else {
	    snprintf(realpath, sizeof(realpath), "%s", path);
	}

	res = chmod(realpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int uvfs_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;
	char realpath[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	} else {
	    snprintf(realpath, sizeof(realpath), "%s", path);
	}

	res = lchown(realpath, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int uvfs_truncate(const char *path, off_t size)
{
	int res;
	char realpath[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	} else {
	    snprintf(realpath, sizeof(realpath), "%s", path);
	}

	res = truncate(realpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int uvfs_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	struct timeval tv[2];
	char realpath[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	} else {
	    snprintf(realpath, sizeof(realpath), "%s", path);
	}

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(realpath, tv);
	if (res == -1)
		return -errno;

	return 0;
}

static int uvfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
        int fd;
	char realpath[PATH_MAX];

       /* Handle special file .audit */
       if (strcmp (path, uvfs.auditfile) == 0) {
               return -EEXIST;
       }
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	} else {
	    snprintf(realpath, sizeof(realpath), "%s", path);
	}
	init_proc_context();

        fprintf(stderr,"uvfs_create: mode=%x, fi->flags=%x\n", mode,
		fi->flags);

        fd = open(realpath, fi->flags, mode);
        if (fd == -1)
              return -errno;

        fi->fh = fd;
	do_audit("creat", path);
        return 0;
}

static int uvfs_open(const char *path, struct fuse_file_info *fi)
{
	int fd;
	char realpath[PATH_MAX];

	init_proc_context();

       /* Handle special file .audit */
       if (strcmp (path, uvfs.auditfile) == 0) {
               return 0;
       }
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	} else {
	    snprintf(realpath, sizeof(realpath), "%s", path);
	}


	fd = open(realpath, fi->flags);
	if (fd == -1)
		return -errno;

        fi->fh = fd;
	do_audit("open", path);

	return 0;
}

static int uvfs_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int res;

	init_proc_context();
       /* Handle special file .audit */
       if (strcmp (path, uvfs.auditfile) == 0) {
	   *buf = 0;
	   hashtable_foreach(uvfs.audit_table, hash_to_string, buf);
           return size;
       }

	res = pread(fi->fh, buf, size, offset);
	if (res == -1)
		res = -errno;

	do_audit("read", path);
	return res;
}

static int uvfs_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int res;
	pid_t pid = 0;
	char *c = NULL, *audit_log_path = NULL;
	int audit_ops = 0;
	struct audit_info *ai = NULL;

	char tmp_path[20];

       /* Handle special file .audit */
       if (strcmp (path, uvfs.auditfile) == 0) {
	       if (*buf != '-') {
                   c   = strtok(buf, "|"); /*pid*/
                   if (c != NULL) pid = strtol(c, NULL, 0);
                   c   = strtok(NULL, "|"); /* audit_ops */
                   if (c != NULL) audit_ops = strtol(c, NULL, 0);
                   c   = strtok(NULL, "|"); /* audit_log_path */
                   if (c != NULL) audit_log_path = c;

		   if (audit_log_path == NULL) {
		       sprintf(tmp_path, "%s%d", "/tmp/audit", pid);
		       audit_log_path = tmp_path;
		   }
		   /* Replace the newline, if there*/
		   c = strchr(audit_log_path, '\n');
		   if (c) *c = 0;

                   ai = malloc(sizeof (struct audit_info));
		   ai->audit_ops = audit_ops;
		   ai->audit_log_path = strdup(audit_log_path);
                   fprintf(stderr,"audt_log_path=%s.\n",  ai->audit_log_path);
		   ai->audit_log_fd = open(audit_log_path, O_RDWR|O_CREAT|O_TRUNC);
                   fchmod(ai->audit_log_fd, S_IRUSR|S_IWUSR |S_IRGRP|S_IWGRP|S_IROTH);

	           hashtable_insert(uvfs.audit_table, pid, ai);

	       } else { /* remove this pid from auditing ...*/
                   c   = strtok(buf + 1, "|"); /*pid*/
                   if (c != NULL) pid = (int) strtol(c, NULL, 0);
                   fprintf(stderr,"removing pid=%d.\n",  pid);
	           ai = (struct audit_info *) hashtable_lookup(uvfs.audit_table, pid);
		   if (ai) {
	               hashtable_remove(uvfs.audit_table, pid);
		       close(ai->audit_log_fd);
                       fprintf(stderr,"audt_log_path=%s.\n",  ai->audit_log_path);
		       free(ai->audit_log_path);
                       fprintf(stderr,"audt_log_path freed.\n");
		       free(ai);
		   }
	       }
               return size;
        }

	init_proc_context();


	res = pwrite(fi->fh, buf, size, offset);
	if (res == -1)
		res = -errno;

	do_audit("write", path);
	return res;
}

static int uvfs_flush(const char *path, struct fuse_file_info *fi)
{
      int res;

       /* Handle special file .audit */
       if (strcmp (path, uvfs.auditfile) == 0) {
               return 0;
       }

      res = close(dup(fi->fh));
      if (res == -1)
           return -errno;

      return 0;
}

static int uvfs_release(const char *path, struct fuse_file_info *fi)
{
       /* Handle special file .audit */
       if (strcmp (path, uvfs.auditfile) == 0) {
               return 0;
       }

       close(fi->fh);
       return 0;
}


static int uvfs_statfs(const char *path, struct statvfs *stbuf)
{
	int res;
	char realpath[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	    path = realpath;
	}

	res = statvfs(path, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}


static int uvfs_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented */
static int uvfs_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{
	char realpath[PATH_MAX];

       /* Handle special file .audit */
       if (strcmp (path, uvfs.auditfile) == 0) {
               return 0;
       }
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	    path = realpath;
	}

	int res = lsetxattr(path, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int uvfs_getxattr(const char *path, const char *name, char *value,
			size_t size)
{
	char realpath[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	    path = realpath;
	}

	int res = lgetxattr(path, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int uvfs_listxattr(const char *path, char *list, size_t size)
{
	char realpath[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	    path = realpath;
	}

	int res = llistxattr(path, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int uvfs_removexattr(const char *path, const char *name)
{
	char realpath[PATH_MAX];
	/* Adjust to the real path on the system*/
	if (uvfs.rootdir) {
	    snprintf(realpath, PATH_MAX, "%s%s", uvfs.rootdir, path);
	    path = realpath;
	}
	int res = lremovexattr(path, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

/*
static int uvfs_lock(const char *path, struct fuse_file_info *fi, int cmd,
	                    struct flock *lock)
{
    (void) path;

     return ulockmgr_op(fi->fh, cmd, lock, &fi->lock_owner,
                   sizeof(fi->lock_owner));
}
*/

static struct fuse_operations uvfs_oper = {
	.getattr	= uvfs_getattr,
	.fgetattr	= uvfs_fgetattr,
	.access		= uvfs_access,
	.readlink	= uvfs_readlink,
	.opendir	= uvfs_opendir,
	.readdir	= uvfs_readdir,
	.releasedir	= uvfs_releasedir,
	.mknod		= uvfs_mknod,
	.mkdir		= uvfs_mkdir,
	.symlink	= uvfs_symlink,
	.unlink		= uvfs_unlink,
	.rmdir		= uvfs_rmdir,
	.rename		= uvfs_rename,
	.link		= uvfs_link,
	.chmod		= uvfs_chmod,
	.chown		= uvfs_chown,
	.truncate	= uvfs_truncate,
	.utimens	= uvfs_utimens,
	.create		= uvfs_create,
	.open		= uvfs_open,
	.read		= uvfs_read,
	.write		= uvfs_write,
	.flush		= uvfs_flush,
	.release	= uvfs_release,
	.statfs		= uvfs_statfs,
	.release	= uvfs_release,
	.fsync		= uvfs_fsync,
#ifdef HAVE_SETXATTR
	.setxattr	= uvfs_setxattr,
	.getxattr	= uvfs_getxattr,
	.listxattr	= uvfs_listxattr,
	.removexattr	= uvfs_removexattr,
#endif
/*	.lock           = uvfs_lock, */
};

/************************************************************
**** FUSE callback functions ends ***************************
 ************************************************************/
static void usage(const char *progname)
{
	fprintf(stderr,
"usage: %s [user@]host:[dir] mountpoint [options]\n"
"\n"
"    -o rootdir=dir to mirror \n"
"\n", progname);
}

static void uvfs_add_arg(const char *arg)
{
	if (fuse_opt_add_arg(&uvfs.uvfs_args, arg) == -1)
		_exit(1);
}

static int uvfs_opt_proc(void *data, const char *arg, int key,
                          struct fuse_args *outargs)
{
	(void) data;
	char buffer[1024];

	switch (key) {
	case FUSE_OPT_KEY_OPT:
		/*if (is_uvfs_opt(arg)) { */
			snprintf(buffer, 1024, "-o%s", arg);
			uvfs_add_arg(buffer);
			return 0;
		/*} */
		return 1;

	case FUSE_OPT_KEY_NONOPT:
		/*
		if (!sshfs.host && strchr(arg, ':')) {
			sshfs.host = strdup(arg);
			return 0;
		}
		*/
		return 1;

	case KEY_HELP:
		usage(outargs->argv[0]);
		fuse_opt_add_arg(outargs, "-ho");
	        fuse_main(outargs->argc, outargs->argv, &uvfs_oper, NULL);
		exit(1);

	case KEY_VERSION:
		fprintf(stderr, "UVFS version %s\n", UVFS_VERSION);
		fuse_opt_add_arg(outargs, "--version");
	        fuse_main(outargs->argc, outargs->argv, &uvfs_oper, NULL);
		exit(0);

	case KEY_FOREGROUND:
		uvfs.foreground = 1;
		return 1;

	default:
		fprintf(stderr, "internal error\n");
		abort();
	}
}

int main(int argc, char *argv[])
{
        struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

        if (fuse_opt_parse(&args, &uvfs, uvfs_opts, uvfs_opt_proc) == -1 )
		                    exit(1);

	sprintf(uvfs.auditfile, "%s", "/.audit");
	uvfs.audit_table = hashtable_new(0);
	uvfs.proc_table = hashtable_new(0);

        /*	umask(0); */
	return fuse_main(args.argc, args.argv, &uvfs_oper, NULL);
}
