#include <SoftwareSerial.h>
#include <GyverMAX6675.h>
#include <Servo.h>

SoftwareSerial mySerial(10, 11); // RX, TX

#define PIN_Stra  A0
#define PIN_CLK   3  //  SCK接线3
#define PIN_CS    4  //  CS接线4
#define PIN_DATA  5  //  SO接线5
#define PIN_Servo 9
#define PIN_Hot   8
GyverMAX6675<PIN_CLK, PIN_DATA, PIN_CS> temp; //调用GyverMAX6675子函数
Servo MyServo;

int Strain_analog;
float Strain_Read;
unsigned long HeatTime = 0;
float Temp_Read;
String receivedString;

// 记录开始时间和结束时间的变量
unsigned long startTime=0;
unsigned long endTime=0;

void setup() 
{
  pinMode(PIN_Hot, OUTPUT);
  MyServo.attach(PIN_Servo);
  Serial.begin(9600);
  mySerial.begin(9600);
  MyServo.write(0);
  digitalWrite(PIN_Hot, HIGH);
}

void loop() 
{
  delay(500);
  Serial_Receive();
  Temp_Get();
  Strain_Check();
  Heat_Time_Read();
}

void Serial_Receive(void)
{
  if(Serial.available())
  {
    receivedString = Serial.readStringUntil('\n');
  }
}

void Temp_Get(void)
{
  if (temp.readTemp()) 
  {   
    Temp_Read = temp.getTemp();
    Serial.print("温度:");
    Serial.println(Temp_Read);
  }
}

void Heat_Time_Read(void)
{
  if(receivedString == "Start")
  {
    if(Temp_Read <= 30 && startTime == 0)
    {
      digitalWrite(PIN_Hot, LOW);
      startTime = millis();
      MyServo.write(100);
    }
    else if(Temp_Read > 60 && startTime != 0)
    {
      digitalWrite(PIN_Hot, HIGH);
      endTime = millis();
      // 计算温度达到目标的时间间隔
      HeatTime = endTime - startTime;
      startTime=0;
      endTime=0;
      Serial.println(HeatTime);
      MyServo.write(0);
      receivedString="";
    }
    if(HeatTime > 10000 && HeatTime < 20000 && Strain_Read > 0 && Strain_Read < 1)
    {
      Serial.println("玻璃");
      mySerial.println("A7:00001");
    }
    else if(HeatTime >= 40000 && Strain_Read >= 3 && Strain_Read < 5)
    {
      Serial.println("金属");
      mySerial.println("A7:00002"); 
    } 
    else if(HeatTime >= 40000 && HeatTime < 45000 && Strain_Read >= 3 && Strain_Read < 3.5)
    {
      Serial.println("木材");
      mySerial.println("A7:00003");
    }
    else if(HeatTime >= 40000 && HeatTime < 50000 && Strain_Read >= 3.5 && Strain_Read < 4.5)
    {
      Serial.println("橡胶");
      mySerial.println("A7:00004");
    }
    else if(HeatTime >= 40000 && HeatTime < 45000 && Strain_Read >= 2 && Strain_Read < 2.5)
    {
      Serial.println("塑料");
      mySerial.println("A7:00005");
    }
      HeatTime=0;
  }
}

void Strain_Check(void)
{
  Strain_analog = analogRead(PIN_Stra);
  Strain_Read =  Strain_analog * 5.0 / 1024;
  Serial.print("应变:");
  Serial.println(Strain_Read);
}
