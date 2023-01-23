#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>



int maxinode = 0;
int maxusername = 0;
int maxlinks = 0;
int maxgroupname = 0; 
int maxsize = 0;


typedef struct list{
    int size;
    struct fileinfo **arr;
    int length;
    char* name;
}list;

typedef struct fileinfo{
    char* name;
    long inode;
    char* username;
    char* groupname;
    struct tm *time;
    long links;
    long size;
    char* permissions;
    struct stat *stats;
}fileinfo;

typedef struct mainlist{
    struct list **arr;
    int size;
    int length;
}mainlist;

void addtomainlist(mainlist* lst, list* info){
    if(lst->size == lst->length){
        lst->arr = realloc(lst->arr, lst->length*2*(sizeof(list*)));
        lst->size = lst->size*2;
    }
    lst->arr[lst->length] = info;
    lst->length++;
}

void addtolist(list* lst, fileinfo* info){
    if(lst->size == lst->length){
        lst->arr = realloc(lst->arr, (lst->length +2)*(sizeof(fileinfo*)));
        lst->size = lst->size+2;
    }
    lst->arr[lst->length] = info;
    lst->length++;
}

void getAndretGroup(gid_t grpNum, char* buf)
{
    
    struct group *grp = getgrgid(grpNum);
   
    strcpy(buf,grp->gr_name);
}

void getAndretUserName(uid_t uid, char* bur)
{
    struct passwd *pw = getpwuid(uid);
    strcpy(bur,pw->pw_name);
}

void sort(list *lst){
    int i,j;
    for (i = 1; i < lst->length; i++) {
        struct fileinfo *key = lst->arr[i];
        j = i - 1;
        while (j >= 0 && strcmp(key->name, lst->arr[j]->name) < 0) {
            lst->arr[j + 1] = lst->arr[j];
            j = j - 1;
        }
        lst->arr[j + 1] = key;
    }
}
void max(bool l, bool inode, fileinfo* info){
    char str[512];
    if(inode){
        sprintf(str, "%ld",info->inode);
        int len = strlen(str);
        if(maxinode < len){
            maxinode = len;
        }
    }
    if (l){
        sprintf(str, "%ld",info->links);
        int len = strlen(str);
        if(maxlinks < len){
            maxlinks = len;
        }
        len = strlen(info->groupname);
        if(maxgroupname < len){
            maxgroupname = len;
        }
        len = strlen(info->username);
        if(maxusername < len){
            maxusername = len;
        }
        sprintf(str, "%ld",info->size);
        len = strlen(str);
        if(maxsize < len){
            maxsize = len;
        }

    }
}

void orgainze(bool inode, bool l, struct fileinfo* info, struct stat *stats, struct list *lst){
    char* buf = malloc(100*sizeof(char));
    buf[0] = '\0';
    if(inode){
        info->inode = stats->st_ino;
    }
    if(l){
        info->links = stats->st_nlink;
  
      
        info->stats = stats;
        getAndretUserName(stats->st_uid, buf);
        info->username = malloc(sizeof(char)*(strlen(buf) + 1));
        strcpy(info->username, buf);
        buf[0] = '\0';
        getAndretGroup(stats->st_gid, buf);
        info->groupname = malloc(sizeof(char)*(strlen(buf) + 1));
        strcpy(info->groupname, buf);
        buf[0] = '\0';
        info->size = stats->st_size;
        info->permissions = malloc(sizeof(char)*(11));
        strcpy(info->permissions ,"\0");
        (S_ISDIR(stats->st_mode)) ? strcat(info->permissions, "d") : strcat(info->permissions, "-");
        (stats->st_mode & S_IRUSR) ? strcat(info->permissions, "r") : strcat(info->permissions, "-");
        (stats->st_mode & S_IWUSR) ? strcat(info->permissions, "w") : strcat(info->permissions, "-");
        (stats->st_mode & S_IXUSR) ? strcat(info->permissions, "x") : strcat(info->permissions, "-");
        (stats->st_mode & S_IRGRP) ? strcat(info->permissions, "r") : strcat(info->permissions, "-");
        (stats->st_mode & S_IWGRP) ? strcat(info->permissions, "w") : strcat(info->permissions, "-");
        (stats->st_mode & S_IXGRP) ? strcat(info->permissions, "x") : strcat(info->permissions, "-");
        (stats->st_mode & S_IROTH) ? strcat(info->permissions, "r") : strcat(info->permissions, "-");
        (stats->st_mode & S_IWOTH) ? strcat(info->permissions, "w") : strcat(info->permissions, "-");
        (stats->st_mode & S_IXOTH) ? strcat(info->permissions, "x") : strcat(info->permissions, "-");


    }
    free(buf);



    max(l,inode, info);
    addtolist(lst,info);
    
      
}


void myls(char* file, bool inode, bool l, bool R, struct mainlist *ls){
   
    DIR *dir;
    struct dirent *pDirent;
    struct list *lst = malloc(sizeof(struct list));
    lst->arr = malloc(sizeof(struct fileinfo*)*2);
    lst->size = 2;
    lst->length = 0;
    lst->name = file;
  
    char str[512];
    addtomainlist(ls,lst);
    
    dir = opendir(file);


 
    struct list *lstfiles = malloc(sizeof(struct list));
    lstfiles->arr = malloc(sizeof(struct fileinfo*)*2);
    lstfiles->size = 2;
    lstfiles->length = 0;
    


    while ((pDirent = readdir(dir)) != NULL) {
        if(strcmp(pDirent->d_name,".") != 0 && strcmp(pDirent->d_name,"..") != 0 ){

        
            struct stat *stats = malloc(sizeof(struct stat));
            stat(pDirent->d_name, stats);
            struct fileinfo *info = malloc(sizeof(struct fileinfo));
            info->name = malloc(sizeof(char) * (strlen(pDirent->d_name) + 1));
            info->name = strcpy(info->name,pDirent->d_name);
            orgainze(inode, l, info, stats, lst);
        }

    }
  
    closedir(dir);
   
    int i,j;
    for (i = 1; i < lst->length; i++) {
        struct fileinfo *key = lst->arr[i];
        j = i - 1;
        while (j >= 0 && strcmp(key->name, lst->arr[j]->name) < 0) {
            lst->arr[j + 1] = lst->arr[j];
            j = j - 1;
        }
        lst->arr[j + 1] = key;
    }

  
    
  
    



    for(int i = 0; i < lst->length; i++){


       
        if(R){


            
            strcpy(str, file);
            strcat(str,"/");
            strcat(str,lst->arr[i]->name);
       
            dir = opendir(str);
            if(dir != NULL){
                char* filestr = malloc(sizeof(char)*(strlen(str) + 1));
                strcpy(filestr,str);
                struct fileinfo *info1 = malloc(sizeof(struct fileinfo));
                info1->name = filestr;
            
                addtolist(lstfiles, info1);
               

                closedir(dir);
                
            }
        }



    }
    if(R){
        for(int i = 0; i < lstfiles->length; i++){
            myls(lstfiles->arr[i]->name, inode, l, R, ls);
           
            free(lstfiles->arr[i]);
        }
    }
    free(lstfiles->arr);
    free(lstfiles);
    


    
}

int main(int argc, char *argv[])
{
    DIR *dir;
    bool inode = false;
    bool l = false;
    bool R = false;
    char dic[512];
    struct mainlist *ls = malloc(sizeof(ls));
    ls->arr = malloc(sizeof(list*) * 2);
    ls->size = 2;
    ls->length = 0;
    char buf[512];
    

    bool flag = false; 
    int i = 0;
    int count = 0;

    if(argc == 1){
        printf("Error : Nonexistent files or directories\n");
    }   
    for(int i = 1; i < argc; i++){
    
        if(!flag && argv[i][0] == '-'){
            for (int x = 1; x < strlen(argv[i]); x++){
                if(argv[i][x] == 'i'){
                    inode =true;
                }
                else if(argv[i][x] == 'l'){
                    l = true;
                }
                else if(argv[i][x] =='R'){
                    R = true;
                   
                }
                else{
                    printf("Error   : Unsupported Option\n");
                   
                    return 0;
                }
            }

            if(argc == 2){
                printf("Error : Nonexistent files or directories\n");
            }
        }

        else{ 
            count++;
            flag = true;
            strcpy(dic, argv[i]);
            char *str = malloc((strlen(dic) + 1) * sizeof(char));
            strcpy(str,dic);
            dir = opendir(dic);
            if(dir==NULL){
                
                flag = true;
                struct stat *stats = malloc(sizeof(struct stat));
                
                if(stat(argv[i], stats) == -1){
                   
                   


                    struct list *lst = malloc(sizeof(struct list));
                    lst->arr = malloc(sizeof(struct fileinfo*));
                    lst->name = malloc((strlen(argv[i]) + 1) * sizeof(char));
                    strcpy(lst->name,argv[i]);
                    addtomainlist(ls,lst);
                    lst->length = 0;
                    lst->size = -2;
                    free(stats);
                }
                else{
                    
                    struct list *lst = malloc(sizeof(struct list));
                    lst->arr = malloc(sizeof(struct fileinfo*)*2);
                    lst->size = 2;
                    lst->length = 0;
                    lst->name = malloc((strlen(argv[i]) + 1) * sizeof(char));
                    strcpy(lst->name,argv[i]);
                    struct fileinfo *info = malloc(sizeof(struct fileinfo));
                    info->name = malloc((strlen(argv[i]) + 1) * sizeof(char));
                    strcpy(info->name,argv[i]);
                    orgainze(inode, l, info, stats, lst);
                    addtomainlist(ls,lst);
                   
                   
                }

            }
            else{
               
                closedir(dir);
                myls(str,inode, l,R, ls);
            }
        }
    }
    struct tm *time = malloc(sizeof(time));
    int first = false;
    //printf("%s\n",ls->arr[1]->arr[0]->name);d
    for(int x = 0; x < ls->length; x++){
        if(count > 1 || R){
            if(first){
                printf("\n");
            }
            else{
                first = true;
            }
            printf("%s:\n", ls->arr[x]->name);
        }
 
        if(ls->arr[x]->size < 0){
            printf("Error : Nonexistent files or directories\n");
        }
    
        for(i= 0; i < ls->arr[x]->length; i++){
            if(inode){
                printf("%-*ld        ",maxinode,ls->arr[x]->arr[i]->inode);
            }
            if(l){
                printf("%s      ",ls->arr[x]->arr[i]->permissions);
                printf("%-*ld       ", maxlinks,ls->arr[x]->arr[i]->links);
                printf("%-*s    ", maxusername,ls->arr[x]->arr[i]->username);
                printf("%-*s    ", maxgroupname,ls->arr[x]->arr[i]->groupname);
                printf("%-*ld   ", maxsize, ls->arr[x]->arr[i]->size);
                time = localtime(&ls->arr[x]->arr[i]->stats->st_mtime); 
                strftime(buf,50, "%b %e %Y %H:%M", time);
                printf("%s       ", buf);
                buf[0] = '\0';
                

            }
            printf("%s\n",ls->arr[x]->arr[i]->name);
        }
    }

    for(int i = 0; i < ls->length; i++){
        for(int x =0; x < ls->arr[i]->length; x++){
            if(l){
                
                free(ls->arr[i]->arr[x]->username);
                free(ls->arr[i]->arr[x]->groupname);
                free(ls->arr[i]->arr[x]->stats);
                free(ls->arr[i]->arr[x]->permissions);
            }
            free(ls->arr[i]->arr[x]->name);
            free(ls->arr[i]->arr[x]);
            
        }
        free(ls->arr[i]->name);
        free(ls->arr[i]);        
    }
    free(ls->arr);
    free(ls);
    return 0;
}

