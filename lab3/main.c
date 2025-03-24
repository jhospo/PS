#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utmpx.h>
#include <pwd.h>
#include <string.h>
#include "groups.h"

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
                fprintf(stderr, "Użycie: %s [-h] [-g]\n", argv[0]);
                exit(EXIT_FAILURE); } }
 
struct utmpx *entry;
setutxent();
 while ((entry=getutxent())!=NULL) {
	if(entry->ut_type==USER_PROCESS) {
	struct passwd *pwd = getpwnam(entry->ut_user);
            if (!pwd) {continue;}
		 printf("%s", entry->ut_user);

		if (showHost && strlen(entry->ut_host) > 0) {
                printf(" (%s)", entry->ut_host); }
		if (showGroups) {
                char *groupsStr = getUserGroups(pwd->pw_uid);
                if (groupsStr) {
                    printf(" %s", groupsStr);
			free(groupsStr); }}

		printf("\n");
        }
    }

    endutxent();
    return 0;
}




