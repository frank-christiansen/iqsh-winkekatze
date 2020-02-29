#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

#define augenGPIO 2
#define armGPIO 12
#define armHoch 50
#define armRunter 160

#define zeitKurz 500
#define zeitLang 1500
#define zeitPause 750
#define zeitNeuerBuchstabe 4500 //In Summe mit beiden Pausen 6000
#define zeitEnde 20000

#ifndef APSSID
#define APSSID "Manekineko"
#define APPSK "artikel26"
#endif

const char* ssid = APSSID;
const char* password = APPSK;
IPAddress local_IP(192,168,4,173);
IPAddress gateway(0,0,0,0);
IPAddress subnet(255,255,255,0);

int armUntenSeit;
int win = 0;

const char* morsecode = ".- .-. - .. -.- . .-.. ..--- -....";
const int morsecodeLen = 34;
int iMorsecode = 0;
bool iMorsecodeStart = true;
int nextMorsecodeChange = millis();

unsigned long ulReqcount;

Servo arm;
WiFiServer server(80);



void setup() {
  
  pinMode(augenGPIO, OUTPUT);
  arm.attach(armGPIO);
  arm.write(armHoch);
  Serial.begin(9600);
  delay(1);
  Serial.println("https://github.com/frank-christiansen/iqsh-winkekatze");

  WiFi.mode(WIFI_AP);
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed!");
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
  server.begin();
  
}



void loop() {


  //MORSECODE AUGEN
  if (millis() > nextMorsecodeChange){
    
    //DEBUG OUTPUT
    Serial.print(millis());
    Serial.print(" ");
    Serial.print(iMorsecode);
    Serial.print(" ");
    Serial.print(morsecode[iMorsecode]);
    
    // LED AN ODER AUS
    if (iMorsecodeStart && (morsecode[iMorsecode] != ' ')){
      digitalWrite(augenGPIO, HIGH);
      Serial.println(" on");
    }else{
      digitalWrite(augenGPIO, LOW);
      Serial.println(" off");
    }
    
    //ZEIT FÜR NÄCHSTES EREIGNISS ERMITTELN
    //Falls Start vom aktuellen Zeichen
    if (iMorsecodeStart) {
      //kurz, lang oder neuer Buchstabe
      switch (morsecode[iMorsecode]) {
        case '.':
          nextMorsecodeChange = millis() + zeitKurz;
          break;
        case '-':
          nextMorsecodeChange = millis() + zeitLang;
          break;
        default:
          nextMorsecodeChange = millis() + zeitNeuerBuchstabe;
          break;
      }
    } else {
      //Pause oder Ende
      if (iMorsecode >= (morsecodeLen - 1)){
        //Ende
        nextMorsecodeChange = millis() + zeitEnde;
        iMorsecode = 0;
      } else {
        //Pause
        nextMorsecodeChange = millis() + zeitPause;
        iMorsecode++;
      }
      
    }
    
    // START, ENDE, START, ...
    iMorsecodeStart = !iMorsecodeStart;
    
  }
      
    
  // WINKEN WENN BUTTON GEDRUECKT WURDE
  if (win == 1) {
    arm.write(armRunter);
    armUntenSeit = millis();
    win = 2;
  } else if (win ==2 && (millis() > armUntenSeit + 2000)) {
      arm.write(armHoch);
      win = 0;
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
  
  if(sPath!="/") {

    // 404 not found
    
    sResponse="<html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL was not found on this server.</p></body></html>";
    
    sHeader  = "HTTP/1.1 404 Not found\r\n";
    sHeader += "Content-Length: ";
    sHeader += sResponse.length();
    sHeader += "\r\n";
    sHeader += "Content-Type: text/html\r\n";
    sHeader += "Connection: close\r\n";
    sHeader += "\r\n";
    
  } else {



    // HTML page
    
    ulReqcount++;
    sResponse  = "<html><head><title>Manekineko</title></head><body bgcolor=\"#000000\" text=\"#FFFFFF\" link=\"#FFFFFF\" vlink=\"#FFFFFF\" alink=\"#FFFFFF\">";
    sResponse += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">";
    sResponse += "<font face=\"Verdana\" size=\"5\">";
    sResponse += "<h1 align=center><u>Rette die Welt:</u> Bitte Manekineko um Vergebung und hoffe auf ein Zeichen!</h1><br><br>";
    sResponse += "<div align=center><a href=\"?pin=FUNCTION1ON\"><button>Um Vergebung bitten...</button></a></div>";
    sResponse += "</font>";
    sResponse += "</body></html>";

    
  
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
