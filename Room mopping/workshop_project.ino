
#include <MPU6050_tockn.h>
#include <Wire.h>

MPU6050 mpu6050(Wire);

int t1,t2,t3;// trigs (f,l,r) 
int e1,e2,e3;//echos
int lw1,lw2,rw1,rw2;//wheels
int p; // pump

int flag = 0,rot =0, start = 0;
long th1 = 30 ,th2 = 20;

long front,left,right;

long distance ,duration;

void setup() {
  t1 = 3;t2 = 8;t3 = 5;//front left right
  e1 = 4; e2 = 7; e3 = 6;//front left right
  lw1 = 10; rw1 = 12;
  p = 9;
  Serial.begin(9600);
  pinMode(t1,OUTPUT);
  pinMode(t2,OUTPUT);
  pinMode(t3,OUTPUT);
  pinMode(e1,INPUT); 
  pinMode(e2,INPUT); 
  pinMode(e3,INPUT); 
  pinMode(lw1,OUTPUT);
  pinMode(rw1,OUTPUT);
  pinMode(p,OUTPUT);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

}

long Sonar(int trigPin,int echoPin)
{
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
distance = (duration/2) / 29.1;
return distance;
}

void done()
{
    digitalWrite(lw1,LOW);
    digitalWrite(rw1,LOW);
    delay(500);
}

void go()
{
    digitalWrite(lw1,HIGH);
    digitalWrite(rw1,HIGH);
    delay(100);
}

void spray()
{
  digitalWrite(p,HIGH);
  delay(2000);
  digitalWrite(p,LOW);
}

void turn(int a,int f)
{
  if(f==1)
  {
        while(mpu6050.getAngleZ() == 180)
        {
          digitalWrite(lw1,HIGH);
          digitalWrite(rw1,LOW);
          delay(10);
          mpu6050.update();
        }
  }
  else if(f==-1)
  {
        while(mpu6050.getAngleZ() == -180)
        {
          digitalWrite(lw1,LOW);
          digitalWrite(rw1,HIGH);
          delay(10);
          mpu6050.update();
        }
  }
}

void loop() 
{
  front =  Sonar(t1,e1); 
  Serial.print("front = ");
  Serial.println(front);
  if(start == 0)
  {
    if(front < th1 && front >0)
    {
      done();
    }
    else
    {
      start =  1;
      Serial.println("bot has started");
      spray();
    }
  }
  if(start == 1)
  {
   
    if(front < th1 && front >0 && front !=6)
    {
      Serial.println("front wall detected");
      left = Sonar(t2,e2);
      right = Sonar(t3,e3);
      Serial.print("left = "); 
      Serial.print(left); 
      Serial.print(" , right = "); 
      Serial.println(right); 
      if(rot == 0)
      {
        if(left>0 && right >0)
        {
          if(left<th2)
          {
            rot = 1;        
          }
          else
          {
            rot = -1;
          }
          turn(180,rot);
          Serial.println("first row done");
          go();
          spray();
        }
      }
      else if(rot == 1 || rot == -1)
      {
        if(left!=0 && right!=0 && (left<th2 || right<th2) )
        {
            flag = 1;
        }
        else if(rot == 1)
        {
          if(flag == 0)
          {
            rot = -1;
            turn(180,rot);
          }
        }
        else if(rot == -1)
        {
          if(flag == 0)
          {
            rot = 1;
            turn(180,rot);
          }
        }
        if(flag == 0)
        {
          go();
          spray();
        }
      }
      if(flag == 1)
      {
        done();
        Serial.println("reached end");
      }
    }
    else if(flag == 0)
    {
      go();
      Serial.println("moving forward");
    }  
  }
  delay(100);
}
