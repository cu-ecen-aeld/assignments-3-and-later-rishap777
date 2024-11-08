#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include<syslog.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>



#define PORT	"9000"      

struct data{
	char * buff;
	struct data *next;
};

static int ssockfd;
static int csockfd;
static struct addrinfo *serverInfo;
static FILE *fptr=NULL;

static void handler(int signo){
	fclose(fptr);
	remove("/var/tmp/aesdsocketdata");
	freeaddrinfo(serverInfo);
	close(ssockfd);
	closelog();
	printf("Exiting the code from Handler \n");
	exit(0);
}

int main(int argc, char * argv[]){	

	int ret;
	struct addrinfo hints;

	openlog(NULL,0,LOG_USER);
/***  Invoking Signal Action for SIGINT and SIGTERM ***/

	struct sigaction act;
	memset(&act,0,sizeof(struct sigaction));
	
	act.sa_handler = handler;
	if(sigaction(SIGTERM,&act,NULL) != 0){
		perror("SIGTERM signal FAILED \n");
		syslog(LOG_ERR,"SIGTERM signal Failed");
		return -1;
	}
	if(sigaction(SIGINT,&act,NULL) != 0){
		perror("SIGINT signal FAILED \n");
		syslog(LOG_ERR,"SIGINT signal Failed");
		return -1;
	}
/***  Invoking Signal Action for SIGINT and SIGTERM *** END***/	

	ssockfd = socket(AF_INET , SOCK_STREAM , 0);
	
	if(ssockfd == -1){
		perror("Socket Not Created\n");
		syslog(LOG_ERR,"Socket Not Created");
		return -1;
	}	
	
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	ret = getaddrinfo(NULL,PORT,&hints,&serverInfo);
	
	if(ret != 0 ){
		perror("getaddrinfo Error\n");
		syslog(LOG_ERR,"getaddrinfo Error");
		return -1;
	}
	
	if(bind(ssockfd,serverInfo->ai_addr,(socklen_t)sizeof(struct sockaddr)) != 0)
	{
		perror("Bind of Socket Failed\n");
		syslog(LOG_ERR,"Binding Failed");
		return -1;
	}
	
	if((argc > 1) && (strcmp(argv[1],"-d") == 0)){
		if(daemon(0,0)){
			perror("Daemon not created\n");
			syslog(LOG_ERR,"Daemon Failed");
			return -1;	
		}
	}
	
	if(listen(ssockfd,5) != 0 ){
		perror("Listening failed\n");
		syslog(LOG_ERR,"listen Failed");
		return -1;		
	}
	printf("Listening on Socket :: %s\n",(serverInfo->ai_addr)->sa_data);
  

	/****************** Writing to File *****************/

	printf("Opening File \n");	
	
	fptr = fopen("/var/tmp/aesdsocketdata","w+");
	
	if(fptr == NULL){
		perror("Opening File /var/tmp/aesdsocketdata FAILED \n");
		syslog(LOG_ERR,"Accepting client Failed");
		return -1;
	}
	printf("File Opened \n");
	printf("memset: msgbuff \n");
	
	char *msgbuf =(char *)malloc(128);
	memset(msgbuf,'\0',128);
	
	char *writebuf =(char *)malloc(128);
	memset(writebuf,'\0',128);
	
	struct data *head = NULL;
	
	char *temp=NULL;
	
	struct sockaddr_in caddr;
	socklen_t len;
	
	while(1){

		csockfd = accept(ssockfd,(struct sockaddr *)&caddr,&len);
		
		if(csockfd == -1){
			perror("Accepting client failed\n");
			syslog(LOG_ERR,"Accepting client Failed");
			return -1;
		}
		printf("Connection Acepted from Client %s\n",inet_ntoa(caddr.sin_addr));
		syslog(LOG_USER,"Accepted connection from Client %s \n",inet_ntoa(caddr.sin_addr));    
		
		memset(msgbuf,'\0',128);
		memset(writebuf,'\0',128);
		temp=NULL;
		
		ret = recv(csockfd,(void *)msgbuf,127,0);
		printf("MSGBUFF:: %s   len=%d\n",msgbuf,ret);
		
		while(ret != 0){
		 	temp = strchr(msgbuf,'\n');
			if( temp == NULL){
			
				printf("In If \n");
				struct data *temp2 = head;
				struct data *temp1 = malloc(sizeof(struct data));
				temp1->buff = (char *)malloc(ret+1);	
				temp1->next = NULL;
				
				strncpy(temp1->buff,msgbuf,ret+1);
				
				if(head==NULL) {head = temp1;}
				else{
					while(temp2->next != NULL){
						temp2=temp2->next;
					}
					temp2->next = temp1;
				}
				
			}else{
				printf("In Else \n");
			/*** writing received msg in file ***/
			
				struct data *temp2 = head;
				struct data *temp1=NULL;
				
				while(temp2 != NULL){
					printf("CheckBuff =%sThis Should be on newLine\n",temp2->buff);
					fprintf(fptr,"%s",temp2->buff);
					temp1=temp2;
					temp2 = temp2->next;
					free(temp1->buff);
					free(temp1);
				}
				head=NULL;
	/*** writing remaining str before \n in file ***/
				size_t len = (temp-msgbuf)+1;
				printf("checking length =%ld\n",len);
				
				char *tempdata = (char *)malloc(128);
				memset(tempdata,'\0',128);
				strncpy(tempdata,msgbuf,len);
				printf("tempdata = %s",tempdata);
				fprintf(fptr,"%s",tempdata);
				//fwrite(tempdata,1,len,fptr);
				fflush(fptr);
#if 1				
				if(fseek(fptr,0L,SEEK_SET) == 0){
					printf("File Position Set to start \n");
				}
		
				int s = fgetc(fptr);;
				while(s != EOF){
					printf("%c",(unsigned char)s);
					send(csockfd,&s,1,0);
					s = fgetc(fptr);
				}
#endif

#if 0
				printf("Opening File for reading \n");	
	
				FILE *ftmp = fopen("/var/tmp/aesdsocketdata","r");
	
				if(ftmp == NULL){
					perror("Opening File /var/tmp/aesdsocketdata FAILED \n");
					syslog(LOG_ERR,"Accepting client Failed");
					return -1;
				}
				printf("File Opened \n");

				char s = fgetc(ftmp);
				while(feof(ftmp)){
					printf("%c",s);
					send(csockfd,&s,1,0);
					s = fgetc(ftmp);
				}
				fclose(ftmp);
				
#endif


				
				if((ret-(temp-msgbuf)+1) > 0){
					head = malloc(sizeof(struct data));
					head->buff = (char *)malloc(ret - (temp-msgbuf)+1);
					head->next = NULL;
						printf("Ok Before Copy \n");
					strcpy(head->buff,temp+1);
				}
			}
			
			memset(msgbuf,'\0',128);
			ret = recv(csockfd,(void *)msgbuf,127,0);
				printf("MSGBUFF:: %s \n",msgbuf);
				fflush(fptr);
		}//while(ret != 0)
		printf("Connection Closed from Client %s\n",inet_ntoa(caddr.sin_addr));
		syslog(LOG_USER,"Closed connection from Client %s\n",inet_ntoa(caddr.sin_addr));
		close(csockfd);
	}//while (1)
/*** Returning the full Content of the file to the client socket  and closing it***/

	printf("Returning the full content of the file to the client \n");

	
	printf("Exiting the code from Main \n");
			
return 0;
}

