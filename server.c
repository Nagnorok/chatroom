#include <sys/types.h>
#include <sys/socket.h>                         
#include <stdio.h>
#include <netinet/in.h>                        
#include <arpa/inet.h>                     
#include <unistd.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
 
#define PORT    8080
#define MYKEY   12345
#define SIZE    10240
 
int main()
{  
    int shmid;
    char *shmaddr;                      
    shmid = shmget(MYKEY, SIZE, IPC_CREAT | 0600);  
    shmaddr= (char *) shmat(shmid, 0, 0);
   	char online[10][100];
	int onl=0;

    if(shmid==-1)
    {
        printf("shmid error\n");
    }
    memset(shmaddr,0,SIZE);
   
    int i=0;
    char buf[100];
    memset(buf,0,100);
   
   
    int server_sockfd,client_sockfd;   
    int server_len,client_len;
   
    struct sockaddr_in server_sockaddr,client_sockaddr;
   
   
    server_sockfd = socket(AF_INET,SOCK_STREAM, 0); 
   
    server_sockaddr.sin_family=AF_INET;
    server_sockaddr.sin_port=htons(PORT);
    server_sockaddr.sin_addr.s_addr=INADDR_ANY;
   
    server_len=sizeof(server_sockaddr);
   

    int j=1;
    setsockopt(server_sockfd,SOL_SOCKET,SO_REUSEADDR,&j,sizeof(j));
   
    //bind port
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,server_len)==-1)
    {
        perror("bind:");
        exit(1);       
    }
   
   
    if(listen(server_sockfd,5)==-1)
    {
        perror("listen:");
        exit(1);   
    }
    printf("Listening...\n");
   
    client_len=sizeof(client_sockaddr);
   
    pid_t ppid,pid;
  
   while(1)
   {  
    if((client_sockfd=accept(server_sockfd,(struct sockaddr *)&client_sockaddr,&client_len))==-1)
        {
             perror("accept error:");
             exit(1);
        }

     int recvbytes;  
                 if((recvbytes=recv(client_sockfd,buf,100,0))==-1)
               {
                    perror("read client_sockfd failed:");
                  
               }
    printf("%s login server\n", buf);
   
             /* for(onl=0;onl<10;onl++)
                {
                    if(*(online[onl])==0)
                    { 
                        strcpy(online[onl],buf);
printf("%d %s\n",onl, online[onl]);
                        break;
                    
                    }
			printf("%d %s\n",onl, online[onl]);
                }*/


    ppid=fork();
   
    if(ppid==-1)
    {
        printf("fork 1 failed:");
    }
   
    if(ppid==0)                  //recieve and send messgae
    {
        pid=fork();
        if(pid==-1)
        {
            printf("fork 2 failed:");
            exit(1);
        }
    

         
        if(pid==0)              //recieve message
        {  
            while(1)
            {
               
               if((recvbytes=recv(client_sockfd,buf,100,0))==-1)
               {
                    perror("read client_sockfd failed:");
                  
               }
   
                usleep(10000);
               printf("%s\n",buf);
               
                for(i=0;i<1000;i++)
                {
                    if(*(shmaddr+100*i)==0)
                    {
                        strcpy(shmaddr+100*i,buf);  
                        break;
                    
                    }
                }
               
               
            }
        }
       
        if(pid>0)               //send to chat history 
        {
           while(1)
            {

                if(*(shmaddr+i*100)!=0)
               {
  
                    write(client_sockfd,shmaddr,SIZE);

                    i++;
                   
                }
               /* if(*(online[onl])!=0)
               {
                    write(client_sockfd,online[onl],100);

                    onl++;
                   
                }*/
            }
       
        }    
      
    }
   
   
    if(ppid>0)             
    {
        close(client_sockfd);
    }
       
    
    }
 
}
