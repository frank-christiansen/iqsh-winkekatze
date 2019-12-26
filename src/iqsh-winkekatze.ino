#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

#define augenGPIO 2
#define armGPIO 12
#define armHoch 50
#define armRunter 180

#ifndef APSSID
#define APSSID "Escape Cat"
#define APPSK "cat"
#endif

const char* ssid = APSSID;
const char* password = APPSK;
int milis = millis();
int win = 0;

unsigned long ulReqcount;

Servo arm;
WiFiServer server(80);




void setup() {
 
  pinMode(augenGPIO, OUTPUT);
  arm.attach(armGPIO);
  arm.write(armHoch);
  Serial.begin(9600);
  delay(1);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  server.begin();
  
  
}

void loop() {

  //Morsecode Augen

   if (win==0){
      if (millis() < (milis + 1500)){
        digitalWrite(augenGPIO, HIGH);
        Serial.write("an1");
      }
      else if ( millis() < (milis + 2000)){
        digitalWrite(augenGPIO, LOW);
         Serial.write("aus1");
      }
       else if (millis() < (milis + 2500)){
        digitalWrite(augenGPIO, HIGH);
         Serial.write("an1");
      }
       else if (millis() < (milis + 3000)){
        digitalWrite(augenGPIO, LOW);
        Serial.write("aus1");
      }
       else if (millis() < (milis + 4500)){
        digitalWrite(augenGPIO, HIGH);
      }
       else if (millis() < (milis + 5000)){
        digitalWrite(augenGPIO, LOW);
      }

      else if (millis() < (milis + 5500)){
        digitalWrite(augenGPIO, HIGH);
      }

       else if (millis() < (milis + 7500)){
        digitalWrite(augenGPIO, LOW);
      }
       else if (millis() < (milis + 8000)){
        digitalWrite(augenGPIO, HIGH);
      }
       else if (millis() < (milis + 8500)){
        digitalWrite(augenGPIO, LOW);
      }
       else if (millis() < (milis + 10000)){
        digitalWrite(augenGPIO, HIGH);
      }

      else if ( millis() < (milis + 12000)){
        digitalWrite(augenGPIO, LOW);
      }
       else if (millis() < (milis + 13500)){
        digitalWrite(augenGPIO, HIGH);
      }
       else if (millis() < (milis + 15500)){
        digitalWrite(augenGPIO, LOW);
        
      }
      
       else if (millis() > (milis + 15500)){
        milis = millis();
      }
   }else{
    
    // activated on button click
    
        if (win == 1){
        milis = millis();
        win= 2;
        }

       //Winkroutine 
        
        else if (win == 2) {
          digitalWrite(augenGPIO, HIGH);
          if (millis() < (milis + 1000)){
             arm.write(armHoch);
          }
          else if (millis() < (milis + 6000)){
             arm.write(armRunter);
          }
          else if (millis() < (milis + 7000)){
             arm.write(armHoch);
          }
          else if (millis() < (milis + 8000)){
             arm.write(armRunter);
          }
          else if (millis() < (milis + 9000)){
             arm.write(armHoch);
          }
          else if (millis() < (milis + 10000)){
             arm.write(armRunter);
          }
          else if (millis() < (milis + 11000)){
             arm.write(armHoch);
          }
          else if (millis() > (milis + 11000)){
             milis = millis();
             win = 0;
          }
          
        }
       
    
   }
   
 // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) 
  {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  unsigned long ultimeout = millis()+250;
  while(!client.available() && (millis()<ultimeout) )
  {
    delay(1);
  }
  if(millis()>ultimeout) 
  { 
    Serial.println("client connection time-out!");
    return; 
  }
  
  // Read the first line of the request
  String sRequest = client.readStringUntil('\r');
  //Serial.println(sRequest);
  client.flush();
  
  // stop client, if request is empty
  if(sRequest=="")
  {
    Serial.println("empty request! - stopping client");
    client.stop();
    return;
  }
 
  String sPath="",sParam="", sCmd="";
  String sGetstart="GET ";
  int iStart,iEndSpace,iEndQuest;
  iStart = sRequest.indexOf(sGetstart);
  if (iStart>=0)
  {
    iStart+=+sGetstart.length();
    iEndSpace = sRequest.indexOf(" ",iStart);
    iEndQuest = sRequest.indexOf("?",iStart);
    
    // are there parameters?
    if(iEndSpace>0)
    {
      if(iEndQuest>0)
      {
        // there are parameters
        sPath  = sRequest.substring(iStart,iEndQuest);
        sParam = sRequest.substring(iEndQuest,iEndSpace);
      }
      else
      {
        // NO parameters
        sPath  = sRequest.substring(iStart,iEndSpace);
      }
    }
  }
  
  if(sParam.length()>0)
  {
    int iEqu=sParam.indexOf("=");
    if(iEqu>=0)
    {
      sCmd = sParam.substring(iEqu+1,sParam.length());
      Serial.println(sCmd);
    }
  }
  
  
  String sResponse,sHeader;
  
 // 404 not found
 
  if(sPath!="/")
  {
    sResponse="<html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL was not found on this server.</p></body></html>";
    
    sHeader  = "HTTP/1.1 404 Not found\r\n";
    sHeader += "Content-Length: ";
    sHeader += sResponse.length();
    sHeader += "\r\n";
    sHeader += "Content-Type: text/html\r\n";
    sHeader += "Connection: close\r\n";
    sHeader += "\r\n";
  }


// HTML page
  
  else
  {
    ulReqcount++;
    sResponse  = "<html><head><title>Demo f&uumlr ESP8266 Steuerung</title></head><body>";
    sResponse += "<font color=\"#000000\"><body bgcolor=\"#FFFFFF\">";
    sResponse += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">";
    sResponse += "<h1>Herzlichen Gl&uuml;ckwunsch!</h1><br>";
    sResponse += "<FONT SIZE=+1>";
    sResponse += "<p><a href=\"?pin=FUNCTION1ON\"><button>Gewonnen</button></a></p>";
    
  
//react to button
   
    if (sCmd.length()>0)
    {
   
    //  sResponse += "Kommando:" + sCmd + "<BR>";    Debug show Function on WebPage
      
      // switch GPIO
      if(sCmd.indexOf("FUNCTION1ON")>=0)
      {
        win = 1;
         
      }    
     
    }
    
    sHeader  = "HTTP/1.1 200 OK\r\n";
    sHeader += "Content-Length: ";
    sHeader += sResponse.length();
    sHeader += "\r\n";
    sHeader += "Content-Type: text/html\r\n";
    sHeader += "Connection: close\r\n";
    sHeader += "\r\n";
  }
  
  // Send the response to the client
  client.print(sHeader);
  client.print(sResponse);
  
  // and stop the client
  client.stop();
  Serial.println("Client disonnected");
}
