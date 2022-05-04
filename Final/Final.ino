// include the library code:
#include <LiquidCrystal.h>
#include <SimpleDHT.h>
#include <SoftwareSerial.h>
#define RX 2
#define TX 3

String AP = "Akhil";       // AP NAME
String PASS = "123456789"; // AP PASSWORD
String API = "5RIEKI8IYGHCB4UB";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
String field3 = "field3";
String field4 = "field4";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
SoftwareSerial esp8266(RX,TX);

int pinDHT11 = 4;
SimpleDHT11 dht11(pinDHT11);


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

void setup() {
  // set up the LCD's number of columns and rows:
  Serial.begin(115200);
  lcd.begin(16, 2);
  // Print a message to the LCD.
//  lcd.print("ERROR 404");
//  lcd.setCursor(0,0);                 //added
//  lcd.print("Connecting...");         //added
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
//  lcd.setCursor(0,0);                 //added
//  lcd.print("Connected!!!");          //added
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  
  // print the number of seconds since reset:
//  lcd.print(millis() / 1000);
  int mq135=analogRead(A0);
  int mq7 = analogRead(A1);
  //lcd screen print
  lcd.setCursor(0,0);
  lcd.print("AQ:");
  lcd.print(mq135);
//  lcd.print("PPM");
  lcd.setCursor(0,1);
  lcd.print("CO:");
  lcd.print(mq7);
//  lcd.print("PPM");
  
  //serial moniter print
  Serial.print("NEW DATA");Serial.print("\n");
  
  Serial.print("Air Quality Index= ");Serial.print(mq135);Serial.println("PPM");
  Serial.print("Carbon Monoxide="); Serial.print(mq7);Serial.println("PPM");
//DHT11
byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(1000);
    return;
  }
  
//  Serial.print("Sample OK: ");
  Serial.print("Temperature= ");Serial.print((int)temperature); Serial.print("°C, "); 
  Serial.print("Humidity= ");Serial.print((int)humidity); Serial.println("H");
  lcd.setCursor(10,0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print("C");
  lcd.setCursor(10,1);
  lcd.print("H:");
  lcd.print(humidity);
  lcd.print("H");

  //sendig data from sensor to thingspeak
  String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+String(mq135)+"&"+ field2 +"="+String(mq7)+"&"+ field3 +"="+String(temperature)+"&"+ field4 +"="+String(humidity);
  sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
 Serial.print(valSensor);
 Serial.print("\n");
  
  
  // DHT11 sampling rate is 1HZ.
  delay(100);                                           //added
//  lcd.noBlink();
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command); //at+cipsend
    if(esp8266.find(readReplay))  //ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("Success");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
