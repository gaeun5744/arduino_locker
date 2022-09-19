#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27,16,2);

// 밤에 불을 밝힐 LED
int light1=12;
int light2=11;
int cds=A0;
int cdsValue=0;

// 무인택배함에 물건 존재여부를 확인할 초음파 센서
int trigPin=7;
int echoPin=6;
long duration=0;
long distance=0;

void setup() {

  Serial.begin(9600);

  // 초음파센서
  pinMode(echoPin,INPUT);
  pinMode(trigPin,OUTPUT);

  // 밤에 불 밝히는 LED
  pinMode(light1,OUTPUT);
  pinMode(light2,OUTPUT);
  digitalWrite(light1,LOW);
  digitalWrite(light2,LOW);

  // 물건 존재 여부 표시 LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  
}

void loop() {
  
  // 택배함에 물건 존재 여부 표시
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  duration=pulseIn(echoPin,HIGH);
  distance=((float)(340*duration)/1000)/2; // 단위 : mm

  lcd.setCursor(0,0);
  
  if(distance<=100){ // 만약 25cm 이하에 무언가 존재한다면
    Serial.println("택배함안에 무언가가 존재!!");
    lcd.clear();
    lcd.print("State : Exist");
  }
  else{
    lcd.clear();
    lcd.print("State : None");
  }

  // 조도센서로 불 밝히기
  cdsValue=analogRead(cds);
  delay(1);
  if(cdsValue<120){ /// 추후에 실험으로 조도센서값 기준 바꾸기!!
    Serial.println(cdsValue);
    digitalWrite(light1,HIGH);
    digitalWrite(light2,HIGH);
  }
  else{
    digitalWrite(light1,LOW);
    digitalWrite(light2,LOW);
  }

}
