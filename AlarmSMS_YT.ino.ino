#include <SoftwareSerial.h>
SoftwareSerial M590(10, 11);  // Define Rx, Tx pins
const int AlarmSwitch = 2;     // Alarm switch interrupt pin
int SwitchState = 0;         // variable for reading the microswitch
volatile int AlarmState = 1;     // Has alarm been sent
int ch = 0;                     //For reading serial characters
String message = "";            //message string
#define MASTER "+34123456789"  //MASTER control phone number

void setup()
{
  delay(15000);        //Wait for M590 to connect to network
  M590.begin(19200);  // Set M590 to this rate. Default 115200 is too high
  Serial.begin (19200); // Serial port for debugging
  pinMode(AlarmSwitch, INPUT);  //Alarm swith input
  pinMode(LED_BUILTIN, OUTPUT); //LED output
  attachInterrupt(0, Alarm, FALLING);  //attach interrupt to pin 2 and Alarm ISR (Interrupt Service Routine)
  M590.begin(19200);    // Start M590 serial
  delay(500);
  M590.print("AT+CMGF=1\r");  // set SMS mode to text
  delay(500);
  M590.print("AT+CSCS=\"GSM\"\r"); // set SMS character set to GSM
  delay(500);
  M590.print("AT+CNMI=2,2,0,0,0\r"); // Output Text message when recieved
  delay(500);
  M590.print("AT+CMGD=4\r"); // delete all SMS messages
  Serial.println("Ready...");
     
}

void loop()
{
if (AlarmState == 0) {  //Check if alarm triggered
    sendSMS();    //If triggered send SMS
    AlarmState ++; //Alarm has been triggered
    }
  
if (M590.available()) {          //Check for GSM message
    delay(500);                  //Wait for GSM message
    while (M590.available()) {   //read message to string message
      ch = M590.read();
      message += char(ch);
    delay(100);
     }
    Serial.println(message);     // write message to monitor
    }
      if (message.indexOf("+CMT") > -1) {     //is message SMS?
      delay(200);
      if (message.indexOf(MASTER) > -1) //is message from MASTER control?
        {   
      Serial.println("--- MASTER SMS ---");
      if (message.indexOf("Rearm") > -1) {  //Rearm received?
      digitalWrite(LED_BUILTIN, LOW);   //Turn off LED
      message = "";
      M590.print("AT+CMGD=4\r"); // delete all SMS messages
      delay(1000);
      AlarmState ++;  //indicate alarm
      EIFR = (1 << INTF0);    //Clear interrupt buffer!!!
      attachInterrupt(0, Alarm, FALLING);   //Rearm Alarm interrupt
      Serial.println("Rearm");
      delay(1000);
    } 
      else {
        Serial.println("NOT MASTER SMS");
      message = "";
      M590.print("AT+CMGD=4\r"); // delete all SMS messages
      delay(1000);
}
   }
  }
}
  

void Alarm()
{
    digitalWrite(LED_BUILTIN, HIGH);  //Turn on ALarm LED
    AlarmState = 0;
    Serial.println("Alarm Triggered");
    detachInterrupt(0);   //Disable interrupt until rearmed
 } 
  
  void sendSMS()
{
  M590.print("AT+CMGS=\"+34123456789\"\r");  // Set recieving mobile number, with country code
  delay(100);
  M590.print("Trouble at mill!!\r");        // This is the text message
  delay(100);
  M590.print((char)26);                       // End AT command with a ^Z, ASCII code 26
  M590.println();
  String message = "";
  Serial.println("Alarm Sent");
  delay(2000); 
  }
