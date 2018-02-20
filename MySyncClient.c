#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>
int main(int arrc ,char * argv[])
{
    int sockfd = 0,n=0;
    int bytesReceived = 0;
    char recvBuff[256];
    char folder[20];
    char file[20];
    memset(recvBuff, '0', sizeof(recvBuff));
    memset(folder, '0', sizeof(folder));
    memset(file, '0', sizeof(file));
    struct sockaddr_in serv_addr;
    int PORT = atoi((argv[2]));
    printf("%d\n",PORT);
    /* Create a socket first */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
   
    /* Initialize sockaddr_in data structure */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT); // port
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Attempt a connection */
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }
    printf("connected\n");
    mode_t process_mask =umask(0);
    int result_code = mkdir(argv[1],S_IRWXU | S_IRWXG |  S_IRWXO);
    umask(process_mask);
    unsigned char buff[20]={0};
    strcpy(buff,argv[1]);
    //printf("%s\t buff =%s\n",argv[1],buff);
    write(sockfd,buff,sizeof(buff));
    
while(1)
 {  
    if((bytesReceived = read(sockfd, file, 20)) > 0)
    {
      // recvBuff[bytesReceived]='\0';
       printf("%s\n",file);
       if(strcmp(file,"done")==0)
        {
           printf("transfer completed\n");
		exit(0);
        }

    }
    else
    {
      perror("recv error\n");
      exit(0);
    }
    /* Create file where data will be stored */
    FILE *fp;
    char filename[20];
    bzero(filename,20);
    sprintf(filename,"%s%s%s",argv[1],"/",file);
    printf("opened file %s\n",filename);
    fp = fopen(filename, "a+"); 
    if(NULL == fp)
    {
        printf("Error opening file");
        return 1;
    }

    //fprintf(fp,"%s\n","mona");
    bytesReceived=0;
    //bzero(recvBuff,256);
    /* Receive data */
    while((bytesReceived = read(sockfd, recvBuff, 256)) > 0)
    {
        //recvBuff[bytesReceived]='\0';
        printf("received data %s\n",recvBuff);
        printf("Bytes received %d\n",bytesReceived);    
        if(strcmp(recvBuff,"completed")==0)
        {
                printf("file transferred\n");
                
		break;
        }
        //recvBuff[bytesReceived] = '\0';
        fwrite(recvBuff, 1,bytesReceived,fp);
         printf("%s \n", recvBuff);
        bytesReceived =0;
        memset(recvBuff, '0', sizeof(recvBuff));
     }
     if(bytesReceived < 0)
     {
          printf("\n Read Error \n");
     }
    /* if((bytesReceived = read(sockfd, recvBuff, 256)) > 0)
    {
        if(strcmp(recvBuff,"completed")==0)
        {
                printf("file transferred\n");

               
        }
        //recvBuff[n] = 0;
       // fwrite(recvBuff, 1,bytesReceived,fp);
         printf("%s \n", recvBuff);
     }
*/

     //fclose(fp);
    }//while for files
       
     return 0;
}
