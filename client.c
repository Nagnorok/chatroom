
#include <sys/types.h>
#include <sys/socket.h>                         
#include <stdio.h>
#include <netinet/in.h>                         
#include <arpa/inet.h>                          
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
 
#define  PORT 8080
#define  IP_ADDR "127.0.0.1"
 
#define SIZE 10240
 
int main()
{  
     
    struct tm *timeptr;
    time_t timeval;
    char tm[50];
    int sockfd;                                 
    int len;                                    
    struct sockaddr_in address;                 
    int result;

   
 
    sockfd = socket(AF_INET,SOCK_STREAM, 0);    
    address.sin_family = AF_INET;              
   
    address.sin_addr.s_addr = inet_addr(IP_ADDR);                           
   
    address.sin_port = htons(PORT);                
    char buf[100];                             
    memset(buf,0,100);
    char str[120], meme[100];                     
   
    char shmaddr[SIZE];                    //chat history
    int i=0;
 
    int sendbytes=0; 
    char myname[100];
    char say[10]={": "};
    char blank[10] = {"     "};
    printf("Please enter your name:\n");
    scanf("%s",myname);
	fgets(str,120, stdin);
 
   len = sizeof(address);
   result = connect(sockfd, (struct sockaddr *) &address, len); // request link
 


   if (result == -1)  
    {
      perror("Connect failed");
      return 1;
    }
    printf("%s login server success!!\n",myname);
   
 
    pid_t pid;
   
    pid=fork();
    if(pid==-1)
    {
        printf("fork failed");
    }
   
 
    if(pid==0)              //child send message
    {
		strcpy(buf,myname);
                 if((sendbytes=write(sockfd, buf, 100))==-1)
                 {
                    perror("send to server failed:");
                 }

       while(1)
        {            
               	fgets(str,120, stdin);
		
                (void)time(&timeval);
                strcpy(tm,ctime(&timeval));
 
                strcpy(buf,myname);             
                strcat(buf,say); 
              strcat(buf,str);                

               
                 if((sendbytes=write(sockfd, buf, 100))==-1)
                 {
                    perror("send to server failed:");
                 }  // send to server
 
		if(strcmp(str,"=\n") == 0)
		{
			exit(0);
		}

                 usleep(1000);
                
                memset(buf,0,100);
                memset(tm,0,50);
		
        }      
    }
   
    if(pid>0)               //parent recieve message
    {
        while(1)
        {
		/*int status;
			if(waitpid(pid, &status, WNOHANG) != 0)
		{
			 if (WIFEXITED(status))
			{
				exit(0);
			}		
		}*/


            read(sockfd,shmaddr,SIZE);

            if(*(shmaddr+i*100)!=0)
            {
		char *pch;
		int leng;
   		 
		pch=strchr((shmaddr+i*100),':');
		leng = strlen((shmaddr+i*100)) - strlen(pch);


		if(shmaddr[i*100+leng+2] == '=')
		{	
			if(strncmp(myname,shmaddr+(i*100),strlen(myname)) == 0)
			exit(0);
		}
		else if(shmaddr[i*100+leng+2] != '@')
		{

                printf("	%s",(shmaddr+i*100)) ;
                i++;
		}
		else
		{
			if(strncmp(myname,shmaddr+(i*100+leng+3),strlen(myname)) == 0)
			{
				strcpy(meme,(shmaddr+(i*100)));

				char *test = strtok(meme, "@");
				
		                printf("%s", test) ;
				test = strtok(NULL, " ");
				test = strtok(NULL, "\n");
				printf("	%s\n",test);
                		
				i++;
			}
			else
			{
			}
			i++;
		}

            }
           
            usleep(1000);
        }
   
       
    }
    close(sockfd);
    return 0;
 
 
}
