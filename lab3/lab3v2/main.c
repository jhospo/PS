#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utmpx.h>
#include <pwd.h>
#include <string.h>
#include <dlfcn.h>
#include "groups.h"
typedef char* (*GetGroupsFunc)(uid_t uid);
int main(int argc,char*argv[]) {
    int opt;
    int showHost=0; 
	int showGroups = 0;
while ((opt = getopt(argc, argv,"hg"))!=-1) {
        switch (opt) {
            case 'h':
                showHost = 1;
                break;
            case 'g':
                showGroups = 1;
                break;
            default:
                fprintf(stderr, "uycie: %s [-h] [-g]\n", argv[0]);
                exit(EXIT_FAILURE); } }
void *handle = dlopen("./libgroups.so", RTLD_LAZY);
    GetGroupsFunc getUserGroupsDynamic = NULL;
    if (!handle) {
        fprintf(stderr, "bie udało się załadować biblioteki: %s\n", dlerror());
    } else {getUserGroupsDynamic = (GetGroupsFunc)dlsym(handle, "getUserGroups");
        char *error = dlerror();
        if (error != NULL) {
            fprintf(stderr, "błąd przy pobieraniu funkcji getUserGroups: %s\n", error);
            getUserGroupsDynamic = NULL;
        }
    }



struct utmpx *entry;
setutxent();
 while ((entry=getutxent())!=NULL) {
	if(entry->ut_type==USER_PROCESS) {
	struct passwd *pwd = getpwnam(entry->ut_user);
            if (!pwd) {continue;}
		 printf("%s", entry->ut_user);

		if (showGroups) {
                char *groupsStr = NULL;

		if (getUserGroupsDynamic) {
                    groupsStr = getUserGroupsDynamic(pwd->pw_uid);
                }
                if (groupsStr) {
                    printf(" %s", groupsStr);
                    free(groupsStr);
                } else {
                    printf(" [brak info o grupach]");
                }
            }
            printf("\n");
        }
    }
    endutxent();
//wazme
 if (handle) {
        dlclose(handle);
    }
    return 0;
}




