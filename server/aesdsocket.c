#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <syslog.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/queue.h>
#include <sys/time.h>
#include <fcntl.h>
#include <poll.h>


#define PORT	9000      

struct data{
	char * buff;
	struct data *next;
};

struct thread_args{
	int clientSockfd;
	struct sockaddr_in caddr;
};

struct threadinfo{
	pthread_t thread_id;
	struct thread_args client;
	SLIST_ENTRY(threadinfo) tinfo;
};

SLIST_HEAD(slisthead,threadinfo);

static struct slisthead head;

static int ssockfd;
static struct addrinfo *serverInfo;
//static FILE *fptrw=NULL;
//static FILE *fptrr=NULL;
static pthread_mutex_t filewlock,filerlock;
static volatile int signal_flag = 0;
static int fileSize = 0;


int writeFile(char *, int );
int sendToClient(int);


/*******Thread function for time printing in file********/

void *print_time(void* args){
	struct timeval tv;
	struct tm *tm;
	char time_string[40];
	struct timespec stime;
	
	do{
		clock_gettime(CLOCK_MONOTONIC,&stime);
		stime.tv_sec += 10;   //adding 10 seconds
		gettimeofday(&tv,NULL);
		tm = localtime(&tv.tv_sec);
		strftime(time_string,sizeof(time_string),"timestamp:%Y-%m-%d %H:%M:%S\n",tm);

		//write to file with Lock 
		
		
		writeFile(time_string,40);
		//fprintf(fptrw,"timestamp:%s\n",time_string);

	
		
		//sleeping with nanosleep for 10s 
		clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&stime,NULL);
	}while(signal_flag == 0);
	
	pthread_exit(NULL);
}
	
static void handler(int signo){
	signal_flag = 1;
	struct threadinfo *n1; 
   	while (!SLIST_EMPTY(&head)) {
		n1 = SLIST_FIRST(&head);
		SLIST_REMOVE_HEAD(&head,tinfo);
		//free(n1->)
		pthread_join(n1->thread_id,NULL);
		free(n1);
   	}
	pthread_mutex_destroy(&filewlock);   //free mutex write 

	//fclose(fptrr);
	remove("/var/tmp/aesdsocketdata");
	//freeaddrinfo(serverInfo);
	close(ssockfd);
	closelog();
	printf("Exiting the code from Handler \n");
	exit(0);
}

void *thread_function(void *);
//******************MMAAAAAAAAIIIIIIIIIIIIIIINNNNNNNNNNNNNNN*********************************************
int main(int argc, char * argv[]){	

	int ret;
	struct addrinfo hints;
	struct threadinfo *t1;
	struct sockaddr_in servaddr, cli; 
	
	SLIST_INIT(&head);
	
	
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

#if 0	
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
#endif
	servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT);
    
	if(bind(ssockfd,(struct sockaddr *)&servaddr,(socklen_t)sizeof(servaddr)) != 0)
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
	
	if(listen(ssockfd,1024) != 0 ){
		perror("Listening failed\n");
		syslog(LOG_ERR,"listen Failed");
		return -1;		
	}
	//printf("Listening on Socket :: %s\n",(serverInfo->ai_addr)->sa_data);
  

	/****************** Opening aesdsocketdata File *****************/
#if 0
	printf("Opening File \n");	
	
	fptrw = fopen("/var/tmp/aesdsocketdata","a+");
	
	if(fptrw == NULL){
		perror("Opening File /var/tmp/aesdsocketdata FAILED \n");
		syslog(LOG_ERR,"Opening File Failed");
		return -1;
	}
	printf("File Opened for writing \n");
#endif

#if 0	
	fptrr = fopen("/var/tmp/aesdsocketdata","r");
	
	if(fptrr == NULL){
		perror("Opening File /var/tmp/aesdsocketdata FAILED \n");
		syslog(LOG_ERR,"Opening File Failed");
		return -1;
	}
	printf("File Opened for Reading \n");
#endif	
	
	struct sockaddr_in caddr;
	int csockfd;	
	socklen_t len=sizeof(caddr);
	int count=0;
	pthread_mutex_init(&filewlock,NULL);  // Mutex Init for writing file
	//pthread_mutex_init(&filerlock,NULL);  // Mutex Init for reading file
#if 1	
	t1 = malloc(sizeof(struct threadinfo));
	
	if(pthread_create(&(t1->thread_id),NULL,print_time,NULL)){
			perror("Thread Creation failed\n");
			syslog(LOG_ERR,"Thread Creation Failed");
			free(t1);
	}else{
		SLIST_INSERT_HEAD(&head,t1,tinfo);
	}
#endif	
		
	while(1){
		
		csockfd = accept(ssockfd,(struct sockaddr *)&caddr,&len);
		
		if(csockfd == -1){
			perror("Accepting client failed\n");
			syslog(LOG_ERR,"Accepting client Failed");
			return -1;
		}
		
		t1 = malloc(sizeof(struct threadinfo));
		
		t1->client.clientSockfd = csockfd;
		t1->client.caddr = caddr;

		printf("Connection Acepted from Client %s\n",inet_ntoa(caddr.sin_addr));
		syslog(LOG_USER,"Accepted connection from Client %s \n",inet_ntoa(caddr.sin_addr));    
		
				
		if(pthread_create(&(t1->thread_id),NULL,thread_function,(void *)&(t1->client))){
			perror("Thread Creation failed\n");
			syslog(LOG_ERR,"Thread Creation Failed");
			free(t1);
		}else{
			SLIST_INSERT_HEAD(&head,t1,tinfo);
			count++;
			printf("thread Count %d \n",count);
		}
	

	}//while (1)
/*** Returning the full Content of the file to the client socket  and closing it***/

	printf("Returning the full content of the file to the client \n");

	
	printf("Exiting the code from Main \n");
			
return 0;
}



/************ Thread_Run_Function ************************/

void *thread_function(void *args){
		
		struct thread_args *myargs = (struct thread_args *) args;
		char *msgbuf =(char *)malloc(4096);
		memset(msgbuf,'\0',4096);

		//int len=0;
		int ret=0;
		static int count1=0;
		struct data *head = NULL;
		char *temp=NULL;
		
		volatile int csockfd =0;
		csockfd = myargs->clientSockfd;
		
		
		ret = recv(csockfd,(void *)msgbuf,4095,0);

		count1++;
		//printf("CountTHREAD = %d csockfd =%d argSock=%d ret=%d \n",count1,csockfd,myargs->clientSockfd,ret);
		while(ret != 0){
		 	
				
						//lock
				
	/*** writing remaining str before \n in file ***/
				
				//printf("checking length =%ld\n",len);
				
				fileSize += ret;
				//printf("filesize= %d buf = %s ",fileSize,msgbuf);
				writeFile(msgbuf, ret);
				//printf("writen to a file \n");
			
				temp = strchr(msgbuf,'\n');	
				if(temp != NULL){
					sendToClient(csockfd);
				}
			temp=NULL;
			memset(msgbuf,'\0',4096);
			ret = recv(csockfd,(void *)msgbuf,4095,0);
		}//while(ret != 0)
		printf("Connection Closed from Client \n");
		//syslog(LOG_USER,"Closed connection from Client %s\n",inet_ntoa(caddr.sin_addr));
		
		//free(filebuf);
		free(msgbuf);
		//close(myargs->clientSockfd);	
		pthread_exit(NULL);
}


int writeFile(char *wbuff, int len){
	printf("%s",wbuff);
	pthread_mutex_lock(&filewlock);  //lock
#if 1
	//printf("Opening File for writing \n");	
	
	FILE *fptrw = fopen("/var/tmp/aesdsocketdata","a+");
	
	if(fptrw == NULL){
		perror("Opening File /var/tmp/aesdsocketdata FAILED \n");
		syslog(LOG_ERR,"Opening File Failed");
		return -1;
	}
	fsync(fileno(fptrw));
	//printf("File Opened for writing \n");
#endif
	
	fprintf(fptrw,"%s",wbuff);
	fsync(fileno(fptrw));
	fclose(fptrw);
	pthread_mutex_unlock(&filewlock);  //unlock
	
}

int sendToClient(int csockfd){
	char *filebuf =(char *)malloc(4096);
	memset(filebuf,'\0',4096);
	pthread_mutex_lock(&filewlock);  //lock
#if 1
	//printf("Opening File for Reading \n");	
	
	FILE *fptrw = fopen("/var/tmp/aesdsocketdata","r");
	
	if(fptrw == NULL){
		perror("Opening File /var/tmp/aesdsocketdata FAILED \n");
		syslog(LOG_ERR,"Opening File Failed");
		return -1;
	}
	//printf("File Opened for Reading \n");
#endif

		fsync(fileno(fptrw));

		char *s = fgets(filebuf,4095,fptrw);
		//printf("string= %s \n",filebuf);
		while(s != NULL){
			//printf("%c",(unsigned char)s);
			int len = strlen(filebuf);
			//printf("len = %d string= %s \n",len,filebuf);
			send(csockfd,filebuf,len,0);
			//pthread_mutex_lock(&filelock);		//lock
			memset(filebuf,'\0',4096);
			s = fgets(filebuf,4095,fptrw);
			//pthread_mutex_unlock(&filelock);	//unlock
		}
		fclose(fptrw);
		pthread_mutex_unlock(&filewlock);  //unlock
		free(filebuf);
}
