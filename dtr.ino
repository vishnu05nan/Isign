
#include <GSM.h>
#include <RCSwitch.h>

// PIN Number for the SIM
#define PINNUMBER ""

// initialize the library instances
GSM gsmAccess;
GSM_SMS sms;
RCSwitch mySwitch = RCSwitch();

// Array to hold the number and message a SMS is retreived from
char senderNumber[20];  
char smsmessage[100];

//declare variables for temp sensor
float tempC;
int tempPin = A0;
int threshold= 30;
char tempspeed[]="00110000";

void setup() {

  Serial.begin(9600);

  // Transmitter is connected to Arduino Pin #10  
  mySwitch.enableTransmit(10);
  Serial.println("DIGITAL TRAFFIC ROAD SIGN");

  // connection state
  boolean notConnected = true;

  // Start GSM connection
  while(notConnected)
  {
    if(gsmAccess.begin(PINNUMBER)==GSM_READY)
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("GSM initialized");
  Serial.println("Waiting for messages");
}

void loop() 
{
  int i=0;

  // If there are any SMSs available()  
  if (sms.available())
  {
    Serial.println("Message received from:");

    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    // An example of message disposal    
    // Any messages starting with # should be discarded
    if(sms.peek()=='#')
    {
      Serial.println("Discarded SMS");
      sms.flush();
    }

    // Read message bytes and print them
    while(smsmessage[i]=sms.read())
      i++;

    tempC = analogRead(tempPin);
    tempC = (5.0 * tempC * 100.0)/1024.0;
    if(tempC < threshold)
    {
      Serial.println("wet road! Speed limit – 30km/hr");
      //Serial.println(byte(tempC));
      int j=0;
      while(tempspeed[j]!='\0')
      {
        smsmessage[j]=tempspeed[j];
        j++;
      }
    }

    Serial.println(byte(tempC));
    Serial.println(smsmessage);

    Serial.println("\nEND OF MESSAGE");

    // Delete message from modem memory
    sms.flush();
    Serial.println("MESSAGE DELETED");
  } 
  mySwitch.send(smsmessage);
  delay(1000);
}







