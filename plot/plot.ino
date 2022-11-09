// /* 
//    原项目Plotclock         by Johannes Heberlein 
//    原链接https://www.thingiverse.com/thing:248009
// */

#include <Servo.h>
#include "Arduino.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "FansInfo.h"


// When in calibration mode, adjust the following factors until the servos move exactly 90 degrees
#define SERVOFAKTORLEFT 600
#define SERVOFAKTORRIGHT 600

// Zero-position of left and right servo
// When in calibration mode, adjust the NULL-values so that the servo arms are at all times parallel
// either to the X or Y axis
#define SERVOLEFTNULL 1950
#define SERVORIGHTNULL 815


#define SERVOPINLIFT  D5 //底
#define SERVOPINLEFT  D6 //左
#define SERVOPINRIGHT D7 //右


#define LIFT0 1650  // on drawing surface 写字状态
#define LIFT1 1500  // between numbers  移动状态
#define LIFT2 950  // sweeper  擦左极限
#define LIFT3 1300 // sweeper  擦右极限


#define LIFTTIME 1 //抬起速度


#define L1 35//臂长
#define L2 55.1
#define L3 13.2
#define L4 45


#define O1X 24// 舵机转轴在平面的坐标
#define O1Y -25
#define O2X 49
#define O2Y -25


int servoLift = 1500;//底部舵机角度过渡，作用是控制速度

Servo servo1;  //底部
Servo servo2;  //左
Servo servo3;  //右

volatile double lastX = 34;//画板中心位置
volatile double lastY = 34;

int last_min = 0;


const char *ssid     = "6";//wifi账号，需要更改为你自己的账号密码
const char *password = "qwertyui";//wifi密码

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com",60*60*8, 30*60*1000);//服务器，获取时间


FansInfo fansInfo("123321123");       // 建立对象用于获取粉丝信息，括号中的参数是UP主的ID号
                                     







void setup() 
{ 
  Serial.begin(9600);//初始化串口

  servo1.attach(SERVOPINLIFT,500,2500);  //后两个参数有利于精确控制0到180度
  servo2.attach(SERVOPINLEFT,500,2500);  
  servo3.attach(SERVOPINRIGHT,500,2500);  
  
  lift(1);
  drawTo(34, 34);//中心位置
  

  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }


  timeClient.begin();

  
  delay(100);

} 








void loop() 
{ 
  
  timeClient.update();
  delay(100);
  Serial.print(timeClient.getHours());
  Serial.println(timeClient.getMinutes());
  
  int currenthour=timeClient.getHours();//拿到时间
  int currentminute=timeClient.getMinutes();

  int currentfansnum;
  
if(fansInfo.update())
 {
  Serial.print("Fans Number: ");
  Serial.println(fansInfo.getFansNumber());
  currentfansnum=fansInfo.getFansNumber();//拿到粉丝数
  }





   
  // if (last_min != currentminute)//每分钟书写一次时间
   if (1)//不间断书写
   {


     lift(1);

     
     wipe();
     number(0, 25, currenthour/10, 0.9);//时钟十位
     number(16, 25, currenthour%10, 0.9);//时钟个位
     number(28, 25, 11, 0.9);//冒号


     number(37, 25, currentminute/10, 0.9);//分钟十位
     number(53, 25, currentminute%10, 0.9);//分钟个位
     
     last_min = currentminute;
     
   }





   
   delay(500);
   wipe();








// if (last_min != currentminute)//每分钟书写一次粉丝数
   if(1)
     {
      if(currentfansnum>=0&&currentfansnum<10)
        {
          number(0, 25, 0, 0.9);//千位
          number(16, 25, 0, 0.9);//百位
          number(37, 25, 0, 0.9);//十位
          number(53, 25, currentfansnum, 0.9);//个位
        }
      if(currentfansnum>=10&&currentfansnum<100)
        {
          number(0, 25, 0, 0.9);//千位
          number(16, 25, 0, 0.9);//百位
          number(37, 25, currentfansnum/10, 0.9);//十位
          number(53, 25, currentfansnum%10, 0.9);//个位
        }
      if(currentfansnum>=100&&currentfansnum<1000)
        {
          number(0, 25, 0, 0.9);//千位
          number(16, 25, currentfansnum/100, 0.9);//百位
          number(37, 25, (currentfansnum/10)%10, 0.9);//十位
          number(53, 25, currentfansnum%10, 0.9);//个位
        }
      if(currentfansnum>=1000&&currentfansnum<10000)
        {
          number(0, 25, currentfansnum/1000, 0.9);//千位
          number(16, 25, (currentfansnum/100)%10, 0.9);//百位
          number(37, 25, (currentfansnum/10)%10, 0.9);//十位
          number(53, 25, currentfansnum%10, 0.9);//个位
        }
      
     }
   

}












/***********************************************************************************************
 功能：字库，每个数字的书写轨迹控制
 输入：横坐标，纵坐标，要写的数字，缩放比例
 返回：无
 **********************************************************************************************/
// Writing numeral with bx by being the bottom left originpoint. Scale 1 equals a 20 mm high font.
// The structure follows this principle: move to first startpoint of the numeral, lift down, draw numeral, lift up
void number(float bx, float by, int num, float scale) {

  switch (num) {

  case 0:
    drawTo(bx + 12 * scale, by + 6 * scale);//bx,by
    lift(0);
    bogenGZS(bx + 7 * scale, by + 10 * scale, 10 * scale, -0.8, 6.7, 0.5);
    lift(1);
    break;
  case 1:
    drawTo(bx + 10 * scale, by + 20 * scale);//
    lift(0);
    drawTo(bx + 10 * scale, by + 20 * scale);
    drawTo(bx + 10 * scale, by + 0 * scale);
    lift(1);
    break;
  case 2:
    drawTo(bx + 1 * scale, by + 15 * scale);
    lift(0);
    bogenUZS(bx + 8 * scale, by + 14 * scale, 6 * scale, 2, -0.8, 1);
    drawTo(bx + 1 * scale, by + 0 * scale);
    drawTo(bx + 12 * scale, by + 4 * scale);//y方向略微偏移
    lift(1);
    break;
  case 3:
    drawTo(bx + 2 * scale, by + 17 * scale);
    lift(0);
    bogenUZS(bx + 5 * scale, by + 15 * scale, 5 * scale, 3, -2, 1);
    bogenUZS(bx + 5 * scale, by + 5 * scale, 5 * scale, 1.57, -3, 1);
    lift(1);
    break;
  case 4:
    drawTo(bx + 10 * scale, by + 0 * scale);
    lift(0);
    drawTo(bx + 10 * scale, by + 20 * scale);
    drawTo(bx + 2 * scale, by + 6 * scale);
    drawTo(bx + 12 * scale, by + 6 * scale);
    lift(1);
    break;
  case 5:
    drawTo(bx + 2 * scale, by + 5 * scale);
    lift(0);
    bogenGZS(bx + 5 * scale, by + 6 * scale, 6 * scale, -2.5, 2, 1);
    drawTo(bx + 5 * scale, by + 20 * scale);
    drawTo(bx + 12 * scale, by + 20 * scale);
    lift(1);
    break;
  case 6:
    drawTo(bx + 2 * scale, by + 10 * scale);
    lift(0);
    bogenUZS(bx + 7 * scale, by + 6 * scale, 6 * scale, 2, -4.4, 1);
    drawTo(bx + 11 * scale, by + 20 * scale);
    lift(1);
    break;
  case 7:
    drawTo(bx + 2 * scale, by + 20 * scale);
    lift(0);
    drawTo(bx + 12 * scale, by + 20 * scale);
    drawTo(bx + 2 * scale, by + 0);
    lift(1);
    break;
  case 8:
    drawTo(bx + 5 * scale, by + 10 * scale);
    lift(0);
    bogenUZS(bx + 5 * scale, by + 15 * scale, 5 * scale, 4.7, -1.6, 1);
    bogenGZS(bx + 5 * scale, by + 5 * scale, 5 * scale, -4.7, 2, 1);
    lift(1);
    break;

  case 9:
    drawTo(bx + 9 * scale, by + 11 * scale);
    lift(0);
    bogenUZS(bx + 7 * scale, by + 15 * scale, 5 * scale, 4, -0.5, 1);
    drawTo(bx + 5 * scale, by + 0);
    lift(1);
    break;


  case 11:
    drawTo(bx + 5 * scale, by + 15 * scale);
    lift(0);
    bogenGZS(bx + 5 * scale, by + 15 * scale, 0.1 * scale, 1, -1, 1);
    delay(10);
    lift(1);
    drawTo(bx + 5 * scale, by + 5 * scale);
    lift(0);
    bogenGZS(bx + 5 * scale, by + 5 * scale, 0.1 * scale, 1, -1, 1);
    delay(10);
    lift(1);
    break;

  }
}




/***********************************************************************************************
 功能：擦写函数
 输入：无
 返回：无
 **********************************************************************************************/
void wipe(void)
    {
      delay(300);
      lift(2);
      delay(300);
      lift(3);
      delay(300);
      lift(2);
      delay(300);
      lift(3);
    }



/***********************************************************************************************
 功能：抬臂到不同高度
 输入：希望的抬臂高度
 返回：无
 **********************************************************************************************/
void lift(char lift) {
  switch (lift) {
    // room to optimize  !

  case 0: //
      if (servoLift >= LIFT0) {
      while (servoLift >= LIFT0) 
      {
        servoLift-=20;
        servo1.writeMicroseconds(servoLift);       
        delay(LIFTTIME);
      }
    } 
    else {
      while (servoLift <= LIFT0) {
        servoLift+=20;
        servo1.writeMicroseconds(servoLift);
        delay(LIFTTIME);

      }

    }

    break;

  case 1: //
    if (servoLift >= LIFT1) {
      while (servoLift >= LIFT1) {
        servoLift-=20;
        servo1.writeMicroseconds(servoLift);
        delay(LIFTTIME);

      }
    } 
    else {
      while (servoLift <= LIFT1) {
        servoLift+=20;
        servo1.writeMicroseconds(servoLift);
        delay(LIFTTIME);
      }

    }

    break;

  case 2:
    if (servoLift >= LIFT2) {
      while (servoLift >= LIFT2) {
        servoLift-=20;
        servo1.writeMicroseconds(servoLift);
        delay(LIFTTIME);
      }
    } 
    else {
      while (servoLift <= LIFT2) {
        servoLift+=20;
        servo1.writeMicroseconds(servoLift);        
        delay(LIFTTIME);
        
      }
    }
    break;

     case 3:
    if (servoLift >= LIFT3) {
      while (servoLift >= LIFT3) {
        servoLift-=20;
        servo1.writeMicroseconds(servoLift);
        delay(LIFTTIME);
      }
    } 
    else {
      while (servoLift <= LIFT3) {
        servoLift+=20;
        servo1.writeMicroseconds(servoLift);        
        delay(LIFTTIME);
        
      }
    }
    break;
  }
}

/***********************************************************************************************
 功能：顺时针书写
 输入：横坐标，纵坐标，半径，开始角度，结束角度
 返回：无
 **********************************************************************************************/
void bogenUZS(float bx, float by, float radius, int start, int ende, float sqee) {
  float inkr = -0.1;//画圈速度
  float count = 0;

  do {
    drawTo(sqee * radius * cos(start + count) + bx,
    radius * sin(start + count) + by);
    count += inkr;
  } 
  while ((start + count) > ende);

}

/***********************************************************************************************
 功能：逆时针书写
 输入：横坐标，纵坐标，半径，开始角度，结束角度
 返回：无
 **********************************************************************************************/
void bogenGZS(float bx, float by, float radius, int start, int ende, float sqee) {
  float inkr = 0.1;//画圈速度
  float count = 0;

  do {
    drawTo(sqee * radius * cos(start + count) + bx,
    radius * sin(start + count) + by);
    count += inkr;
  } 
  while ((start + count) <= ende);
}

/***********************************************************************************************
 功能：将希望的两个点连线上的点坐标求出，说人话就是连点成线
 输入：横坐标，纵坐标
 返回：无
 **********************************************************************************************/
void drawTo(double pX, double pY) {
  double dx, dy, c;
  int i;

  // dx dy of new point
  dx = pX - lastX;
  dy = pY - lastY;
  //path lenght in mm, times 4 equals 4 steps per mm
  c = floor(0.8 * sqrt(dx * dx + dy * dy));//数字用来调节写字速度，c是每毫米走几步*毫米数

  if (c < 1) c = 1;

  for (i = 0; i <= c; i++) {
    // draw line point by point
    set_XY(lastX + (i * dx / c), lastY + (i * dy / c));

  }

  lastX = pX;
  lastY = pY;
}
/***********************************************************************************************
 功能：利用三角函数计算角度
 输入：三角形边长
 返回：角度
 **********************************************************************************************/
double return_angle(double a, double b, double c) {
  // cosine rule for angle between c and a
  return acos((a * a + c * c - b * b) / (2 * a * c));
}




/***********************************************************************************************
 功能：将坐标转化为舵机角度
 输入：横坐标，纵坐标
 返回：无
 **********************************************************************************************/
void set_XY(double Tx, double Ty) 
{
  //delay(1);
  double dx, dy, c, a1, a2, Hx, Hy;

  // calculate triangle between pen, servoLeft and arm joint
  // cartesian dx/dy
  dx = Tx - O1X;
  dy = Ty - O1Y;

  // polar lemgth (c) and angle (a1)
  c = sqrt(dx * dx + dy * dy); // 
  a1 = atan2(dy, dx); //
  a2 = return_angle(L1, L2, c);

  servo2.writeMicroseconds(floor(((a2 + a1 - M_PI) * SERVOFAKTORLEFT) + SERVOLEFTNULL));

  // calculate joinr arm point for triangle of the right servo arm
  a2 = return_angle(L2, L1, c);
  Hx = Tx + L3 * cos((a1 - a2 + 0.621) + M_PI); //36,5�?
  Hy = Ty + L3 * sin((a1 - a2 + 0.621) + M_PI);

  // calculate triangle between pen joint, servoRight and arm joint
  dx = Hx - O2X;
  dy = Hy - O2Y;

  c = sqrt(dx * dx + dy * dy);
  a1 = atan2(dy, dx);
  a2 = return_angle(L1, L4, c);

  servo3.writeMicroseconds(floor(((a1 - a2) * SERVOFAKTORRIGHT) + SERVORIGHTNULL));

}


    
