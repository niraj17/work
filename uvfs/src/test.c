#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <dlfcn.h>

#include"auditlib.h"
#include"hashtable.h"
#include "dblayer/dblayer.h"

#include <libgen.h>

// dll handle
void *handle = NULL;
struct audit_record *ard = NULL;
void parse_and_exec_script_file(struct _audit_ops *aops, struct audit_info * ai);
int exec_script(struct _audit_ops *aops, 
                    struct audit_info * ai, 
                    char * do_path, 
                    char deps[][PATH_MAX], char * script);
do_record  *  find_match( char * tgt_name, char * tgt_path,
                                char ** dep_names,
                                char ** dep_paths,
                                int depcount);
int winkin ( do_record * dorec);

char * get_workspace_root();
struct audit_record * get_audit_record();
void * get_dll_handle ();
void dump_audit_data(struct _audit_ops * aops,
        struct audit_info * ai);

// Caller free the memory
char * get_workspace_root()
{
    char cwd[PATH_MAX];
    memset(cwd, 0, PATH_MAX);
    getcwd(cwd, PATH_MAX-1);
    return strdup(cwd);
}

struct audit_record * get_audit_record()
{
    // returns the global ard;
    return ard;
}

void * get_dll_handle ()
{
    //returns the global handle;
    return handle;
}

pid_t get_ppid(pid_t pid)
{
    int fd, res;
    char proc_path[1024];
    char *c, buffer[1024];

    printf ("pid=%d\n",pid);
    snprintf(proc_path, 1024, "%s%d%s", "/proc/", pid, "/stat"); 
    printf("proc_path=%s\n",proc_path);
    if ((fd = open(proc_path, O_RDONLY)) < 0)
    {
	perror("open failed");
	return 0;
    }
    res = read(fd, buffer, 1024);

    printf("buffer = %s\n",buffer);

    c   = strtok(buffer, " "); /*pid*/
    if (c == NULL) return 0;
    c   = strtok(NULL, " "); /* name */
    if (c == NULL) return 0;
    c   = strtok(NULL, " ");
    if (c == NULL) return 0;
    c   = strtok(NULL, " "); /* ppid */
    if (c == NULL) return 0;
    return atoi(c);

}

int my_func(int key, void *value, void *data)
{
    printf("key=%d, value=%s, data=%s\n", key, value, data);
}

int main_init()
{   
        // refers to the global handle
 	    handle  = dlopen("auditlib.so", RTLD_LAZY);
	    if (handle == NULL) {
	        fprintf(stderr, "%s\n", dlerror());
	    }   
        ard = dlsym(handle, "ar");
	    if (ard == NULL) {
	        fprintf(stderr, "%s\n", dlerror());
            }   
}
int main(int argc, char *argv[])
{
         int res;
        char realpath[1024];
	    char *rootdir = "/myroot";
        char *path=NULL;
	    char *c;
        char * workspace_dir = NULL;
        struct _audit_ops *aops = NULL;
        struct audit_info  ai;
        void * handle = NULL;
	    struct hashtable *ht = NULL;
        int i = 0;

	    printf("ppid = %d\n", get_ppid(getppid()));

        // Assume that MOUNT POINT is the workspace directory
        // that seems reasonable, for now!
        main_init();
        handle = get_dll_handle();
	    aops = dlsym(handle, "audit_ops");
	    if (aops == NULL) {
	        fprintf(stderr, "%s\n", dlerror());
            }

	    printf("calling init\n");
        printf("Calling dbinit\n");
        db_init();
       if(aops->init(&ai) == 1)
        {
            printf("Could not initialize auditing. Possible reasons\n");
            printf("The .audit file could not be found or opened\n");
            exit (1);
        }
            
            sprintf(ai.audit_file_path, "%s", "/tmp/pppp");
    
            printf("argc = %d\n", argc);

           /*for (i = 1; i < argc; i++)
            {
                printf("argv[%d]=%s\n", i, argv[i]);

                if (i % 2 != 0)
                {
	            aops->start_audit(&ai);
                }
                system(argv[i]);
	            aops->read_audit_data(&ai);

                // print the audit data for verification
                printf("ar.count = %d\n", ard->count);
                while (ard->count > 0) {
                    printf("Audit %d:\n", ard->count);
                    printf("      ops = %d", ard->ad[ard->count-1].ops);
                    printf("      path = %s", ard->ad[ard->count-1].path);

                    ard->count--;
                }

                if (i % 2 != 0)
                {
	            aops->stop_audit(&ai);
                }
            }*/
    
            /*
	    ht = hashtable_new(0);
	    hashtable_foreach(ht, my_func, "data");
	    hashtable_insert(ht, 1, "1");
	    hashtable_insert(ht, 11, "11");
	    hashtable_insert(ht, 100, "100");
	    hashtable_insert(ht, 150, "150");
	    hashtable_insert(ht, 251, "251");
	    hashtable_foreach(ht, my_func, "data");
	    hashtable_remove(ht, 251);
	    hashtable_remove(ht, 150);
	    hashtable_remove(ht, 1);
	    hashtable_remove(ht, 11);
	    hashtable_remove(ht, 100);
	    hashtable_foreach(ht, my_func, "data");
	    printf("Found 120 , %s",hashtable_lookup(ht, 120)); 
	    printf("Found 251 , %s",hashtable_lookup(ht, 251)); 
            */
    
        //parse_and_exec_script_file(aops, &ai);
        {
            int i = 0;
            do_record * dorec = NULL;
            char ** dep_names = (char **) malloc(sizeof(char *) * 5);
            for (i = 0 ; i < 5 ; i++)
            {
                dep_names[i] =(char *)malloc(sizeof(char) * 100);
            }
            strcpy ( dep_names[0], "a");
            strcpy ( dep_names[1], "b");
            strcpy ( dep_names[2], "c");
            strcpy ( dep_names[3], "test.c");
            strcpy ( dep_names[4], "test1.c");
            
            printf("Calling find Match\n");
            // We need to get the wid as well :)
            dorec = find_match("x", "/temp/x",  (char **)dep_names, NULL, 5);
            if(dorec != NULL)
            {
                winkin(dorec);
            }
        }
}

int winkin ( do_record * dorec)
{
    workspace_record * wrec = NULL;
    char path[PATH_MAX];
    char commandline[PATH_MAX];
    memset(path, 0, PATH_MAX);
    memset(commandline, 0, PATH_MAX);
    dump_do_record(dorec);
    // get the file from the other workspace
    wrec = fetch_workspace_record( do_record_get_wid(dorec) );
    strcpy(path, wrec_get_workspace_root(wrec));
    strcat(path, do_record_get_path(dorec));
    strcpy (commandline, "cp " );
    strcat (commandline, path);
    strcat ( commandline, "  .");
    if(system(commandline) != 0)
    {
        fprintf(stderr, "Error doing winkin using command %s\n", commandline);
        fprintf(stderr, "Check for file existence of source and write permissions of current directory\n");
        return -1;
    }
    else
    {
        fprintf(stdout, "Fetched %s using command '%s'\n", path, commandline);
        return 0;
    }
}
// TODO Move this code into dblayer
// Get potential match from the table
// What this does is get the tgt name and find out all deps,
// it sees whether deps given here and all other deps (these were touched in the 
// process of creating the target exist) . 
// If all of them are satisfied, that is the dep names match and other deps have
// a existent file, then the match succeeds. Refer to Zen and art of clearmake 
// maintainence  page 49
do_record *  find_match( char * tgt_name, char * tgt_path,
                                char ** dep_names,
                                char ** dep_paths, //this is currently unused by matching algo
                                int depcount)
{
    DOID doid = -1;
    WORKSPACEID * wids = NULL;
    WORKSPACEID wid = -1;
    do_record * dorec = NULL;
    DEPID * depids = NULL;
    unsigned int db_depcount = -1;

    if(dep_names == NULL)
    {
        fprintf(stderr, "dep_names is NULL\n");
        fflush(stdout);
        goto done;
     }
    wids = get_all_workspace_ids();
    if(wids == NULL)
    {
        goto done;
    }

    // Stop with first match, ideally we should call this in a loop and try out all possible
    // dos from different workspaces
    {
        WORKSPACEID * counterwid = wids;
        while(*counterwid != -1)
        {
             doid = get_doid(tgt_name, tgt_path, *counterwid);    
             if(doid != -1)
            {
                wid = *counterwid;
                break;
            }
            counterwid++;
        }
    }

    if(doid == -1)
    {
        goto done;
    }

    dorec = fetch_do_record(doid, wid);
    if(dorec == NULL)
    {
        fprintf(stderr, "Got a NULL record for a valid doid, wid pari\n");
        goto done;
    }
    depids = do_record_depids(dorec);
    // Dep count got from do record
    db_depcount = do_record_depcount(dorec);
    {
        // For each depid, get the dep record and check whether it is present 
        // in the depnames passed in by the caller
        int i = 0;
        while(i  < db_depcount)
        {
            dep_record * deprec = NULL;
            deprec = fetch_dep_record(depids[i], doid, wid);
            if(deprec == NULL)
            {
                fprintf(stderr, "Got a NULL dep record\n");
                goto done;
            }
            dump_dep_record(deprec);
            {
                const char * dep_name  = dep_record_name(deprec);
                int j = 0;
                int found = 0; // This has to be 1 at the end of this loop
                while(j < depcount)
                {
                    if(strcmp(dep_name, dep_names[j]) == 0)
                    {
                        found =1;
                        break;
                    }
                    j++;
                }
                printf("\n");
                if(found == 0)
                {
                    fprintf(stderr, ("No match found\n"));
                    if(dorec != NULL)
                    {
                        free(dorec);
                        dorec = NULL;
                    }
                    if(deprec!=NULL)
                        free(deprec);
                    goto done;
                }
            }  
            if(deprec != NULL)
                free(deprec);
            i++;
        }
    }    
    // If we come here then we must have a successful match 
    
    done:
        if(wids != NULL)
            free(wids);
        return dorec;
}

// Before calling exec script search for potential candidates 
// Implicit : takes the workspace name from global state.
// All names relative to the workspace root
// char ** end  with NULL to know the end of the array
// Call start audit, then call read_audit_data on ai
// This will fill up the global record called ar.
// TODO TODO Big problem all dep arrays work for only 3 deps. FIXME
int exec_script(struct _audit_ops * aops,
                struct audit_info * ai, 
                char * do_path, 
                char  deps[][PATH_MAX], 
                char * script)
{
    int status = 0;
    WORKSPACEID wid = -1;
    DOID doid = -1;
    char * workspace_name = NULL;
    char * workspace_root = NULL;

    // TODO we only accept max 3 extra deps found from auditing
    char extra_deps[3][PATH_MAX] ;
    int dep_count = 0;
    int can_insert_deps = 0;
    
    // Start the auditing
    // Audit file is set to /tmp/pppp in main, want to change it? do it here
     aops->start_audit(ai);

    // execute the script
    if(system(script) != 0)
    {
        fprintf(stderr, "Error executing script given in the Makefile\n");
        exit(1);
    }
    
    // Stop auditing
    aops->stop_audit(ai);

    // Dump audit data
    dump_audit_data(aops, ai);

    // Get extra deps
   {
        struct  audit_record * ar = NULL;
        dep_count = 0;
        char * cur_dep;
        int i = 0;
        aops->read_audit_data(ai); 
        ar = get_audit_record();
        while(i < 3)
        {
            memset(extra_deps, 0, PATH_MAX);
            i++;
        }
        i = 0;
        if(ar->count == 0)
        {
            fprintf(stdout, "ar_count is zero \n");
            fflush(stdout);
            can_insert_deps = 0;
        }
        else
        {
           dep_count  = (ar->count >  3)?3:ar->count;
           while(i < dep_count)
           {
               // TODO Remove duplicates
               cur_dep = extra_deps[i];
               strcpy(extra_deps[i],  ar->ad[i].path);
               i++;
           }
           // Every thing successful, make deps_to_insert point to extra_deps
           can_insert_deps =1;
         }
    }  
    // Get the audit data and insert it into the table.
    // Enter workspace id
    // The workspace root is the place where the .audit file is found,
    // name really doesn't matter but still.
    workspace_root = ai->audit_mount_dir;
    workspace_name = basename(workspace_root);
    printf("%s %s",workspace_root, workspace_name);
    fflush(stdout);
    wid =  enter_workspace_record(workspace_name, workspace_root);
    
  // Enter do record
    {
        char * do_name = (char * ) (malloc(sizeof(char) * (strlen(do_path) + 1) ));
        memset(do_name, 0, strlen(do_path) +1);
        strcpy(do_name, basename(do_path));
        doid = enter_do_record(do_name, do_path, 0, 0, wid);

        if(doid == -1)
        {
            fprintf(stderr, "Entering audit record failed\n");
            exit(1);
        }
    }

    // Enter deps 
    {
       // TODO Inserts only 3 dependencies, take care of this later
       int i = 0;
       for (i = 0; i < 3 ; i++)
        {
            DEPID depid = -1;
            char * dep_path = deps[i];
            char * dep_name = (char * )(malloc(sizeof(char) * strlen(dep_path) +1 ));
            memset(dep_name, 0, strlen(dep_path) + 1);
            strcpy(dep_name, basename(dep_path));
            printf("strlen is %d\n", strlen(dep_path));
            depid = enter_dep_record(dep_name, 
                            dep_path,
                            0,
                            0,
                            doid,
                            wid);
            if(depid == -1)
            {
                fprintf(stderr, "Error inserting audit record\n Dependency insertiong failed\n");
                exit(1);
            }
        }
    }

    // Insert extra deps
    // TODO Supporting only three deps now
    if(can_insert_deps == 1)
    {
       int  i = 0;
       while(i < dep_count)
        {
             DEPID depid = -1;
            char * dep_path = extra_deps[i];
            char * dep_name = (char * )(malloc(sizeof(char) * strlen(dep_path) +1 ));
            memset(dep_name, 0, strlen(dep_path) + 1);
            strcpy(dep_name, basename(dep_path));
             printf("strlen is %d\n", strlen(dep_path));
            printf("Inserting extra deps %s\n", dep_path);
            depid = enter_dep_record(dep_name, 
                            dep_path,
                            0,
                            0,
                            doid,
                            wid);
            if(depid == -1)
            {
                fprintf(stderr, "Error inserting audit record\n Dependency insertiong failed\n");
                exit(1);
            }
            i ++;
        }
    }
    return status;
}

void parse_and_exec_script_file(struct _audit_ops *aops, struct audit_info * ai)
{
    FILE * fp = fopen ("Makefile", "r");
    char tgt_path[PATH_MAX];
    // The 4th row is NULL, this will mark the end of the dependencies
    char deps[3][PATH_MAX];
    char str[PATH_MAX];
    char script[PATH_MAX];
    int dep_count = 0;

    if ( fp == NULL)
    {
        fprintf(stderr, "Error occured when opening a file called Makefile\n");
        fprintf(stderr, "Create a file and then start this program again\n");
        exit(1);
    }
    
    fscanf(fp, "%s\n", tgt_path);
    while( dep_count < 3 && strcmp(str, "marker") != 0)
    {
        fscanf(fp, "%s\n", str);
        memset(deps[dep_count], 0, PATH_MAX);
        strcpy(deps[dep_count], str);
        dep_count++;
    }
    while(strcmp(str, "marker") != 0)
        fscanf(fp, "%s\n",str);

    // Read the script line
    {
        size_t len = 0;
        memset(script, 0, PATH_MAX);
        fgets(script, PATH_MAX-1, fp);
        len = strlen(script);
        // Removes the \n character at the end of the line
        script[len-1]='\0';
    }
    fprintf(stdout, "%s %s %s %s %s", tgt_path, deps[0], deps[1], deps[2], script);
    exec_script(aops, ai, tgt_path, deps, script);
}

void dump_audit_data(struct _audit_ops * aops,
        struct audit_info * ai)
{
// Good part is that the read will only get files that were read not written,
// Holds good for me, 
// But the paths can contain duplicates.
// DB layer will handle it though.
// TODO remove duplicates from the audit paths
 /*   struct  audit_record * ar = NULL;
    int count = ard->count;
    ar = get_audit_record();
    aops->read_audit_data(ai);

     // print the audit data for verification
    printf("ar.count = %d\n", ard->count);
    
    while (count > 0) 
    {
        printf("Audit %d:\n", ard->count);
        printf("      ops = %d", ard->ad[ard->count-1].ops);
        printf("      path = %s", ard->ad[ard->count-1].path);
        count--;
   }*/
}
