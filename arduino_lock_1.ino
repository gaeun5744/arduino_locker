#include <SoftwareSerial.h> // 블루투스 연결에 사용
#include <Servo.h> // 서보모터에 사용
#include <LiquidCrystal_I2C.h> // LCD
#include <Wire.h> // LCD

// 비밀번호 입력시 사용하는 변수 및 핀번호
int button1=4;
int button10=5;
int OKButton=6;
bool OKButton_previous_state=false;
bool OKButton_current_state=false;
int count_1=0;
int count_10=0;
int password_count=0;
bool button1_previous_state=false;
bool button1_current_state=false;
bool button10_previous_state=false;
bool button10_current_state=false;

// 닫힘 버틈
int closeButton=7;
bool closeButton_previous_state=false;
bool closeButton_current_state=false;

// 무인택배함에 물건 존재 여부 표시
int LED=13;

// 블루투스에서 받은 값
String blueTooth="";

// 블루투스에서 받을 새 비밀번호
String password="";
String new_password="";

// 서보모터
Servo myServo;
int servoPin=8;
int angle=0;


LiquidCrystal_I2C lcd(0x27,16,2);

const int HC06TX=2;
const int HC06RX=3;
SoftwareSerial HC06(HC06TX,HC06RX);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  HC06.begin(9600); // 블루투스

  // 잠금 OFF 라고 LED에 표시
  pinMode(LED,OUTPUT); 
  digitalWrite(LED,LOW);

  // 비밀번호 입력용 버튼
  pinMode(button1,INPUT);
  pinMode(button10,INPUT);
  Serial.println("Start");

  // 닫힘 버튼
  pinMode(closeButton,INPUT);

  // 확인 버튼
  pinMode(OKButton,INPUT);

  //LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("password");

  // 서보모터 
  myServo.attach(servoPin);
  myServo.write(angle); //처음에는 잠겨있음
}

void loop() {
  // 블루투스에서 값을 전달받음
  while(HC06.available()){
    char c=(char)HC06.read(); 
    blueTooth+=c; // 수신되는 문자를 blueTooth에 모두 붙임(1바이트씩 전송되는 것을 연결)
    delay(5); // 수신 문자열 끊김 방지
  }

  if(blueTooth.equals("open")){ // 만약 블루투스에서 전달받은 값이 open일 경우
    angle=120;
    myServo.write(angle);
    digitalWrite(LED,HIGH);
    lcd.clear();
    lcd.print("open");
    blueTooth="";
  }
  else if(blueTooth.equals("close")){ // 만약 전달받은 값이 close일 경우
    angle=0;
    myServo.write(angle);
    digitalWrite(LED,LOW);
    lcd.clear();
    lcd.print("close");
    blueTooth="";
  }
  else{ // 위의 경우가 아니라면 비밀번호임
    new_password=blueTooth;
    if(!new_password.equals(password)){
    password=new_password;
    Serial.println("new password : "+password);
    blueTooth="";
    }
  }

  // 버튼으로 비밀번호 입력
  button1_current_state=digitalRead(button1);
  button10_current_state=digitalRead(button10);

  // 1의 자리 수 증가
  if(button1_current_state){ // 버튼을 누른 경우
    if(button1_previous_state==false){ // 이전 상태와 비교
      if(count_1>=0 && count_1<9){ // 만약 count_1이 0~8 사이일 경우
        count_1++;
      }
      else{
        count_1=0;
      }
      button1_previous_state=true;
      Serial.println("count_1 : "+String(count_1));
    }
  }
  else{
    button1_previous_state=false;
  }

  // 10의 자리 수 증가
  if(button10_current_state){ // 버튼을 누른 경우
    if(button10_previous_state==false){ // 이전 상태와 비교
      if(count_10>=0 && count_10<9){ // 만약 count_1이 0~8 사이일 경우
        count_10++;
      }
      else{
        count_10=0;
      }
      button10_previous_state=true;
      Serial.println("count_10 : "+String(count_10));
      
    }
  }
  else{
    button10_previous_state=false;
  }

  // 만약 숫자가 바뀔 경우, 바뀐 숫자를 LCD 판에 표시
  if(button1_previous_state==true || button10_previous_state==true){
    lcd.clear();
    lcd.print(count_10);
    lcd.print(count_1);
  }

  // 패스워드 확인 버튼
  OKButton_current_state=digitalRead(OKButton);
  
  if(OKButton_current_state){ // 버튼을 누른 경우
    if(OKButton_previous_state==false){ // 이전 상태와 비교
      if(String(count_10)+String(count_1)==password){ // 만약 패스워드가 성공할 경우
        lcd.clear();
        lcd.print("success!!");
        angle=120;
        myServo.write(angle); // 문 열기
        digitalWrite(LED,HIGH); // LED에 on/off 표시
      }
      else{
        password_count++;
        Serial.println("password_count: "+String(password_count));
        Serial.println("wrong_password: "+String(count_10*10+count_1));
        Serial.println("real password:" +password);
        lcd.clear();
        lcd.print("Please try again");
        if(password_count==3){
          lcd.clear();
          lcd.print("error!!");
          delay(30000); // 30초 동안 입력할 수 없음
          // 초기화
          count_1=0;
          count_10=0;
          password_count=0;
          lcd.clear();
          lcd.print("password");
          
        }
        OKButton_previous_state=true;
      }
    }
  }
  else{
    OKButton_previous_state=false;
  }

  // 닫힘 버튼
  closeButton_current_state=digitalRead(closeButton);
  
  if(closeButton_current_state){ // 버튼을 누른 경우
    if(closeButton_previous_state==false){ // 이전 상태와 비교
      if(angle==120){
        angle=0;
        myServo.write(angle);
        closeButton_previous_state=true;
        lcd.clear();
        lcd.print("close");
        digitalWrite(LED,LOW);
      }
    }
  }
  else{
    closeButton_previous_state=false;
  }
  

}
