#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11);

String id;
String receiver;
String data;
bool isSend = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  mySerial.begin(57600);

  Serial.print("Enter ID :");
  while(!Serial.available());
  id = Serial.readStringUntil("\n");
  Serial.print("\nMy ID is : " + id + "\n");
}

void flushRx()
{
  while(mySerial.available())
    uint8_t tmp = mySerial.read();
}

void frame(String receiver, String data, int order)
{
  //Flag ~!
  //Address id + receiver
  //data
  //Check sum of ascii
  //Flag !~

  bool check = true;
  String frame;
  String header = id.substring(0,1) + receiver.substring(0,1);
  while(data.length() != 8)
  {
    //Serial.println(data.length());
    data = data + " ";
    //Serial.print("Loop");
  }
  int count = 0;
  for(int i = 0 ; i < 8 ; i++)
  {
   count += data[i];
  }
  String stringCount = String(count);
  frame = "~!" + header + data + stringCount[0] + stringCount[1] + order%2 + "!~\n"; 
  int timeOutcounter = 1;
  while(check)
  {    
    if(timeOutcounter == 5)
    {
      check = false;
      isSend = false;
    }
    for(int i = 0 ; frame[i] != '\0'; i++)
    {
      mySerial.write(frame[i]);
      delay(10);
      flushRx();
    }
    Serial.println("Send frame : " + String(order%2));
    Serial.println("Data       : " + data + "\n");
    long start = millis();
    long timeEiei = start;
    bool isTimeout = true;
    int timer = 5;
    while(millis() - start < 6000){
      if(int(millis() - timeEiei) > 1000)
      {
        //Serial.println(int(millis() - start));
        timeEiei = millis();
        Serial.println("Time out in " + String(timer--));
      }
      if(mySerial.available())
      {
        if(int(mySerial.read()) == int((order+1)%2))
        { 
          //Serial.println("Hello world");
          while(mySerial.available());
          String ackFrame = "This is ack frame";
          ackFrame = mySerial.readStringUntil("\n");
          //Serial.println(ackFrame);
          if(ackFrame[3] == id[0])
          {
            Serial.println("Receive frame");
            Serial.println("Header    : " + ackFrame.substring(2,4));
            Serial.println("ACK No. : " + String(ackFrame[4]));
            //Serial.println("Data      : " + receivedFrame.substring(4,receivedFrame.length()-6));
            Serial.println("Checking  : " + ackFrame.substring(5,7));
            Serial.println("Received\n");
            //Serial.println("Send ACK" + String(int((receivedFrame[receivedFrame.length()-4])+1)%2) + "\n");
            //delay(3000);
            //mySerial.write(char(int(int(receivedFrame[receivedFrame.length()-4])+1)%2));
            //Serial.print(receivedFrame);
            if(ackFrame.substring(5,7) == frame.substring(frame.length()-6,frame.length()-4))
            {
              isTimeout = false;
              check = false;
              goto out;
            }
            else
            {
              Serial.println(ackFrame.substring(5,7));
              Serial.println(frame.substring(frame.length()-6,frame.length()-4));
              Serial.println("Checking Is Incorrect\n");
            }
            
          }
          else
          {
            Serial.print("Shit");  
          }
          
        }
        else{
          Serial.println("NO!!!!!!");
        }
      }    
    }
    out:
    if(isTimeout){
      Serial.println("!!! Time Out " + String(timeOutcounter) + " !!!\n");
      timeOutcounter++;
      Serial.println("Retransmit frame " + String(order%2));      
    }
  }

   
}

void ack(String header, int ackNum, String checking){
  String frame = "";
  frame = "~!" + header + String(ackNum) + checking + "!~\n";
  //Serial.print(frame);
  for(int i = 0 ; frame[i] != '\0'; i++)
    {
      mySerial.write(frame[i]);
      delay(10);
      flushRx();
    }
}


void loop() {
  // put your main code here, to run repeatedly:
  while(!Serial.available())
  {
    if(mySerial.available())
    {
      //Serial.print("Sentder Send ");
      String receivedFrame = " ";
      receivedFrame = mySerial.readStringUntil("\n");
      //Serial.print(receivedFrame[3]);
      if(receivedFrame[3] == id[0]){
        Serial.println("Receive frame");
        Serial.println("Header    : "+receivedFrame.substring(2,4));
        Serial.println("Frame No. : "+String(receivedFrame[receivedFrame.length()-4]));
        Serial.println("Data      : " + receivedFrame.substring(4,receivedFrame.length()-6));
        Serial.println("Checking  : " + receivedFrame.substring(receivedFrame.length()-6,receivedFrame.length()-4));
        Serial.println("Received\n");
        Serial.println("ACK Send IN 3");
        delay(1000);
        Serial.println("ACK Send IN 2");
        delay(1000);
        Serial.println("ACK Send IN 1");
        delay(1000);
        mySerial.write(char(int(int(receivedFrame[receivedFrame.length()-4])+1)%2));
        ack(receivedFrame.substring(3,4)+receivedFrame.substring(2,3), int(int(receivedFrame[receivedFrame.length()-4])+1)%2, receivedFrame.substring(receivedFrame.length()-6,receivedFrame.length()-4));
        Serial.println("Send ACK" + String(int((receivedFrame[receivedFrame.length()-4])+1)%2) + "\n");
        //delay(1000);
        
        //Serial.print(receivedFrame);
      }
      else{
        if(String(receivedFrame[0] != "~")){
          Serial.println(receivedFrame.substring(1));  
        }
        else
        {
          Serial.println(receivedFrame); 
        }
        
      }
      
    }
  }
  while(!Serial.available());
  receiver = Serial.readStringUntil("\n");
  Serial.print("Reciver is : "+ receiver + "\n");  

  while(!Serial.available());
  data = Serial.readStringUntil("\n");
  data = data.substring(0,data.length()-1);
  //int dataLength = data.length();
  //int tempLen = dataLength;
  int order = 0;
  isSend = true;
  while(data.length() > 0 && isSend)
  {      
      frame(receiver, data.substring(0,8),order);
      //Serial.println(data);
      data = data.substring(8);
      delay(500);
      order += 1;
  }
  if(isSend){
    Serial.println("!!!!!!!!!!!!!!!");
    Serial.println("Send Completely");
    Serial.println("!!!!!!!!!!!!!!!\n\n");
    Serial.println("-------------------------\n\n");
  }
  else
  {
    Serial.println("Send Failed\n\n");
    Serial.println("-------------------------\n\n");
  }
  
  //Serial.print("Send file : "+ data + "\n");
  //frame(receiver, data);
  

   
  /*char myString[] = "Computer Engineering";
  for(int i = 0 ; myString[i] != '\0' ; i++)
  {
    mySerial.write(myString[i]);
    delay(10);
    flushRx();
  }
  delay(500);
  mySerial.write('\n');
  
  if(mySerial.available())
    Serial.write(mySerial.read());
  */
}
