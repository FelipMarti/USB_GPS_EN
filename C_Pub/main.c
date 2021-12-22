#define PORT 32100
#define IP_ADDR "127.0.0.1"


#include "com.h"
#include "gps.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int fd;
char read_buffer[BUFFER_SIZE];
int read_buffer_size, now;



int main()
{

   int sockfd;
   struct sockaddr_in servaddr;
   char sendline[1000];

   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr(IP_ADDR);
   servaddr.sin_port=htons(PORT);


  //Define Port
  fd = open_port("/dev/ttyACM0");
  //Define Serial Communication Configuration
  if(set_com_config(fd, 115200, 8, 'N', 1) < 0) //8-bits, Not Parity but, 1 end bit
  { 
    perror("set_com_config"); 
    return 1; 
  }

  while (1) {
    memset(read_buffer,0, BUFFER_SIZE);
    read_buffer_size= read_Buffer(fd, read_buffer);
    if(read_buffer_size > 0){
      read_GPS_Data(read_buffer);
      parse_GpsDATA();
      if(Save_Data.ParseData_Flag == 1 && Save_Data.Usefull_Flag ==1){
        insert_array(Save_Data.Slatitude);
        insert_array(Save_Data.Slongitude);
        printf("%s,%s\n",Save_Data.Slatitude, Save_Data.Slongitude);
        
        sprintf(sendline, "%s,%s\n", Save_Data.Slatitude, Save_Data.Slongitude); 
        sendto(sockfd,sendline,strlen(sendline),0,
             (struct sockaddr *)&servaddr,sizeof(servaddr));

        Save_Data.Usefull_Flag=0;
        Save_Data.ParseData_Flag=0;
      }else{
        printf("Invalid GPS data\n");
      }
    }
  }
  close(fd);
  return 1;
}

