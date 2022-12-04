# "一万粉丝报废机”

*当你有了第一个粉丝，这个小机器人就会一直陪伴着你，尽职尽责实时地记录下你的粉丝数量，直到第9999个粉丝时，这个小机器人的使命也就达成了*

![image](https://user-images.githubusercontent.com/117577770/205493746-b3a793ef-a377-4f4f-bf1c-33f298c926f3.png)


## 快速运行

软件：装好相应的库（servo库，wifi库等），运行plot文件夹里面的plot.ino

硬件：PCB文件夹里面的文件在嘉立创下单

机械：3D打印件里面的stl文件全部进行3D打印，需要至少10mm长的2mm直径螺丝进行连接

注意舵机控制引脚，WiFi账号密码换成你自己的就行

## 操作流程
  机械建模软件是UG，效果图如下。将模型导出，交给3D打印机进行打印，组装到一起后，我们要在擦板上贴上磁铁条，裁剪合适的磁写板，用2mm直径螺丝固定。安装舵机时建议不要先安装舵机上的连接器，可以先把连接器安装到机械结构上，再把舵机运行项目代码，代码中的初始化代码会把舵机角度归正，之后再按照图片所示的位置安装上舵机连接器就可以了。这样可以保证机器人一初始化，机械结构都在预定位置上。

![image](https://user-images.githubusercontent.com/117577770/205493961-42d7d43e-5513-4437-baaa-a94a9a9684e0.png)

  主控板选择的是ESP8266-NodeMCU开发板（CP2102版本），它具有WIFI模块，可以联网获取信息，也有多个引脚可以输出PWM波，用来控制舵机。我特地设计了一块舵机扩展板配合ESP8266-NodeMCU开发板组成机器人的硬件，一方面可以留出排针直接连接舵机，一方面引出一个Type-c接口，可以使用手机充电器插上提供电源。当然这块扩展板只是方便了我们接线和供电，不会显得那么杂乱，你也可以不需要这块扩展板，尝试手工接线，这需要你有一定的动手能力。

![image](https://user-images.githubusercontent.com/117577770/205493988-78c11fc3-1810-4de9-8178-357ef7404348.png)

  在plotclock作者Johannes Heberlein开源代码上，我修改了擦写的执行代码，另外我还加入了获取实时粉丝数的功能，为了实现此效果，需要引入NTPClient,ESP8266WiFi,WiFiUdp和FansInfo四个头文件。其中前三个库都可以在arduino的库管理选项中进行下载，第四个库可以通过Github代码托管平台获取，文末会给出具体链接。自己下载的库要放到arduino编译会寻找库文件的目录下即可。  
  上面的库配置成功后，编译代码没有出错就说明软件环境基本成功了。不过呢要想机器人工作，还需要修改WIFI为你周围可用的账号密码，修改你想查询粉丝数UP主的uid（用户编号）和更换你的舵机信号引脚号。
  

const char *ssid     = "Don't worry";//wifi账号，需要更改为你自己的账号密码
const char *password = "xyymyylyyfyymyyrmmhtlhtl";//wifi密码 
FansInfo fansInfo("3493076135840688");//建立对象用于获取粉丝信息，括号中的参数是UP主的ID号


  完成前面的配置工作后，程序就进入到了setup()函数中，完成一些网络连接，机械臂归位等初始化工作，如果WIFI没有连接成功，则会一直打印小数点。在后面的主循环loop()函数中，利用以下代码实时拿到时间和粉丝数：

timeClient.update();//实时更新时间
  delay(100);
  
  int currenthour=timeClient.getHours();//拿到时间
  int currentminute=timeClient.getMinutes();
  int currentfansnum;
  
if(fansInfo.update())//实时更新粉丝数
 {
  Serial.print("Fans Number: ");
  Serial.println(fansInfo.getFansNumber());
  currentfansnum=fansInfo.getFansNumber();//拿到粉丝数
  }

成功拿到数据后，我们就可以把数据交给写字函数，控制机械臂完成对数字的书写任务。具体代码如下：

   wipe();//擦除
   number(0, 25, currenthour/10, 0.9);//时钟十位
   number(16, 25, currenthour%10, 0.9);//时钟个位
   number(28, 25, 11, 0.9);//冒号

   number(37, 25, currentminute/10, 0.9);//分钟十位
   number(53, 25, currentminute%10, 0.9);//分钟个位

同样的，对于拿到的粉丝数，由于他是一个0~9999的数字，我们对这个数字进行拆分，分别拿到个位、十位，百位和千位单个数字，再去交给机械臂书写。具体代码如下：

if(currentfansnum>=0&&currentfansnum<10)//粉丝数0~9
        {
          number(0, 25, 0, 0.9);//千位
          number(16, 25, 0, 0.9);//百位
          number(37, 25, 0, 0.9);//十位
          number(53, 25, currentfansnum, 0.9);//个位
        }
      if(currentfansnum>=10&&currentfansnum<100) //粉丝数10~99
        {
          number(0, 25, 0, 0.9);//千位
          number(16, 25, 0, 0.9);//百位
          number(37, 25, currentfansnum/10, 0.9);//十位
          number(53, 25, currentfansnum%10, 0.9);//个位
        }
      if(currentfansnum>=100&&currentfansnum<1000) //粉丝数100~999
        {
          number(0, 25, 0, 0.9);//千位
          number(16, 25, currentfansnum/100, 0.9);//百位
          number(37, 25, (currentfansnum/10)%10, 0.9);//十位
          number(53, 25, currentfansnum%10, 0.9);//个位
        }
      if(currentfansnum>=1000&&currentfansnum<10000) //粉丝数1000~9999
        {
          number(0, 25, currentfansnum/1000, 0.9);//千位
          number(16, 25, (currentfansnum/100)%10, 0.9);//百位
          number(37, 25, (currentfansnum/10)%10, 0.9);//十位
          number(53, 25, currentfansnum%10, 0.9);//个位
        }

以上是控制代码的主体，详细的控制工程可以查阅源代码。现在就只需要把代码编译下载到ESP8266-NodeMCU开发板中，小机器人就可以开始写字啦！

![image](https://user-images.githubusercontent.com/117577770/205494104-4aa9c9f5-ca2b-46cb-ae43-cb7d2bd54fce.png)![image](https://user-images.githubusercontent.com/117577770/205494114-7a3e8e08-d7be-47ca-b80c-788be91d8e22.png)



