#include "groups.h"
#include<grp.h>
#include<pwd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

char *getUserGroups(uid_t uid){
struct passwd*pwd =getpwuid(uid)
;
if(!pwd){return NULL;}


int ngroups=0;

getgrouplist(pwd->pw_name, pwd->pw_gid, NULL,&ngroups);
	if(ngroups<=0)
	{return NULL;}

gid_t *groups=malloc(ngroups*sizeof(gid_t));
    if (!groups){
        return NULL; }

	 if (getgrouplist(pwd->pw_name,pwd->pw_gid,groups,&ngroups) < 0) {
        free(groups);
        return NULL; }


	size_t bufSize = 128; 
    char *result = malloc(bufSize);
    if (!result) {
        free(groups);
        return NULL; }
    strcpy(result, "[");
    for (int i = 0; i < ngroups; i++) {
        struct group *gr = getgrgid(groups[i]);
        if (gr) {
	size_t needed = strlen(result) + strlen(gr->gr_name) + 3;
            if (needed > bufSize) {
                bufSize = needed + 50; // pewien zapas
                char *temp = realloc(result, bufSize);
                if (!temp) {
                    free(result);
                    free(groups);
                    return NULL;
                }
                result = temp;
            }
            strcat(result, gr->gr_name);
            if (i < ngroups - 1) {
                strcat(result, ", ");
            }
        }
    }

strcat(result, "]");
    free(groups);
    return result;
}

