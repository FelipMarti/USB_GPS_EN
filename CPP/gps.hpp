#ifndef _GPS_H_
#define _GPS_H_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

class GPS {

private:
    void RST_Buffer(char *Buffer);
    
    void insert_array(char *buff);




public:

    GPS (void);

    ~GPS (void);

    void read_GPS_Data(char *Gps_Buffer);
    
    void parse_GpsDATA();
    
    void print_Save();

    struct
    {
      char GPS_DATA[80];
      char GetData_Flag; //获取到 GPS 数据标志位
      char ParseData_Flag; //解析完成标志位
      char UTCTime[12]; //UTC 时间
      char Slatitude[15]; //纬度
      char N_S[2]; //N/S
      char Slongitude[15]; //经度
      char E_W[2]; //E/W
      char Usefull_Flag; //定位信息是否有效标志位
    } Save_Data;
    
 
};

#endif
