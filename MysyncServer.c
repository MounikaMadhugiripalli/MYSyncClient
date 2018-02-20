#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
int main(int argc,char * argv[])
{
    int listenfd = 0;
    int connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[1025];
    int numrv;
    int PORTNUMBER=atoi(argv[1]);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    printf("Socket retrieve success\n");

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORTNUMBER);

    bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

    if(listen(listenfd, 10) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }

    printf("listening... \n");
    while(1)
    {
        if(-1==(connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL)))
	{
		perror("acceped\n");
		exit(1);
	}
        unsigned char dirname[20]={0};
        read(connfd,dirname,20);
        DIR* dirFile = opendir(dirname);
        struct dirent* hFile;
        if ( dirFile )
        {
            while (( hFile = readdir( dirFile )) != NULL )
            {
             if ( !strcmp( hFile->d_name, "."  )) continue;
             if ( !strcmp( hFile->d_name, ".." )) continue;
             printf( "%s\n", hFile->d_name );
             /* Open the file that we wish to transfer */
             char filename[20];
             sprintf(filename,"%s%s%s",dirname,"/",hFile->d_name);
             printf("%s",filename);
             write(connfd, hFile->d_name, sizeof(hFile->d_name));
             FILE *fp = fopen(filename,"rb");
             if(fp==NULL)
             {
            	printf("File opern error");
            	return 1;   
             }   

       	      /* Read data from file and send it */
               	while(1)
               	{
                /* First read file in chunks of 256 bytes */
                 unsigned char buff[256]={0};
                 int nread = fread(buff,1,256,fp);
                 printf("Bytes read %d \t%s\n", nread,buff);        

                 /* If read was success, send data. */
                 if(nread > 0)
                 {
                     printf("Sending \n");
                     write(connfd, buff, nread);
                 }

                  /*
                   * There is something tricky going on with read .. 
                   * Either there was error, or we reached end of file.
                   */
                   if (nread < 256)
                   {
                      if (feof(fp))
                      {
                        printf("End of file\n");
                        strcpy(buff,"completed");
                        write(connfd, buff, 10);

                      }
                      if (ferror(fp))
                       printf("Error reading\n");
                      break;
                   }
                  
                  }//while for file read
                  fclose(fp);
              }//while for direcory contents
              printf("sending done\n");
              unsigned char buffer[5]="done";
              write(connfd, buffer, 5);
              closedir( dirFile );
          }//if directory is present

        close(connfd);
        sleep(1);
    }//while for socket connection


    return 0;
}
