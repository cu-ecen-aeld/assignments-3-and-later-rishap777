#include<stdio.h>
#include<syslog.h>

int main(int argc,char *argv[]){

    openlog(NULL,0,LOG_USER);

    if(argc != 3)
    {
        printf("Arguments missing\n"); 
        syslog(LOG_ERR,"Arguments missing");   
        return 1;
    }
    
    
    FILE *fp;
    
    fp = fopen(argv[1],"w");
    if(fp == NULL){
        syslog(LOG_ERR,"File Cannot be opened/Created"); 
        return 1;
    }
    
    
    if(fprintf(fp,"%s",argv[2]) < 0){
        syslog(LOG_ERR,"File Cannot Written"); 
        return 1;
    }
    
    fclose(fp);
    
    syslog(LOG_DEBUG,"Writing %s to %s",argv[2],argv[1]); 
    
    closelog();
    
     return 0;
}
