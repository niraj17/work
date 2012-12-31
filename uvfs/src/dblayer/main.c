#include "dblayer.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char ** argv)
{
    WORKSPACEID wid=0;
    DOID doid;
    DEPID depid;
        DEPID depid1;
    workspace_record * wrec;
    do_record * dorec;
    dep_record * deprec;
    char str[100];
    strcpy(str, "123456");
    
    db_init();
/*    wid = enter_workspace_record("/asccad/qvwe", "qwfccasasd");
    doid = enter_do_record("/assddas", "asdqasd", 0,0,wid);
    depid = enter_dep_record("/aaccwes","acqwawz123xsdqq", 4,0, doid, wid);
    depid1 = enter_dep_record("/aaccwecs","aacqwawz123xsdqq", 4,0, doid, wid);
    wrec = fetch_workspace_record(wid); 
    dorec = fetch_do_record(doid, wid);
    deprec = fetch_dep_record(depid, doid, wid);  
    dump_workspace_record(wrec);
    dump_do_record(dorec);
    dump_dep_record(deprec);
    deprec = fetch_dep_record(depid1, doid, wid);
        dump_dep_record(deprec);*/
//    printf("%s", strrev(str));

    wid = 9;
    doid = get_doid("all", "/temp/all", wid);
    dorec = fetch_do_record(doid, wid);
    dump_do_record(dorec);
    return 0;
}


