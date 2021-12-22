#define PORT 32100
#define IP_ADDR "127.0.0.1"

#include "com.h"
#include "gps.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int fd;
char read_buffer[BUFFER_SIZE];
int read_buffer_size, now;


int from_degrees_to_decimal(char *degrees, double *decimal) 
{
    // GPS coordinates are given in a weird format between degrees and decimal.
    // This function translates that to decimal

    int i = 0;
    int spacePos=0;
    char lat_int[4] = "    ";       //Important to clean rubbish 
    char lat_dec[10] = "          ";//Important to clean rubbish
    insert_array(degrees);

    while (degrees[i] != '\0') {
        if (degrees[i] == ' ') {  // If we find the space that separates the first value with the second one
            spacePos = i;
        }
        else if (spacePos == 0) {             // If we are in the degrees part
            lat_int[i] = degrees[i];
        }
        else {                                // If we are in the minutes part
            lat_dec[i-spacePos-1] = degrees[i];
        }
        i++;
    }

    *decimal = atof(lat_int) + atof(lat_dec)/60;
    return 0;
}


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

  double latitude = 0.0;
  double longitude = 0.0;

  while (1) {   //Endless loop that every 1s gets new data from serial bus
    memset(read_buffer,0, BUFFER_SIZE);
    read_buffer_size= read_Buffer(fd, read_buffer);
    if(read_buffer_size > 0){
      read_GPS_Data(read_buffer);
      parse_GpsDATA();
      if(Save_Data.ParseData_Flag == 1 && Save_Data.Usefull_Flag ==1){

        from_degrees_to_decimal(Save_Data.Slatitude, &latitude); 
        from_degrees_to_decimal(Save_Data.Slongitude, &longitude); 

        if (*Save_Data.N_S == 'S') {
            latitude=latitude*-1;
        }
        if (*Save_Data.E_W == 'W') {
            longitude=longitude*-1;
        }
        // printf("DOUBLE: %f, %f\n",latitude,longitude);
        // printf("STRING: %s,%s\n",Save_Data.Slatitude, Save_Data.Slongitude);
        
        sprintf(sendline, "%f,%f\n", latitude, longitude); 
        sendto(sockfd,sendline,strlen(sendline),0,
             (struct sockaddr *)&servaddr,sizeof(servaddr));

        Save_Data.Usefull_Flag=0;
        Save_Data.ParseData_Flag=0;
      }else{
        printf("Invalid GPS data\n");
      }
    }
    usleep(100); // Sleep for 0.1s to save resources
  }
  close(fd);
  return 1;
}

