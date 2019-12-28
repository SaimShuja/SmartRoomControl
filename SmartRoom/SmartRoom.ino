

/*
 * NeoPixel Webserver
 * Copyright (c) 2017, Agis Wichert
 * https://www.youtube.com/watch?v=6U4JssgWjr8
 * 
 * 
 * ESP Webserver
 * Copyright (c) 2015, Majenko Technologies
 * 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */


// Neopixel
#include <Adafruit_NeoPixel.h>

// ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp8266.h>

ESP8266WebServer server ( 80 );
WiFiManager wifiManager;

const char* updateindex = "<form method='POST' action='/updater' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

// Neopixel Config
#define NeoPIN D4
#define NUM_LEDS 60
#define RELPIN2 D1
#define RELPIN1 D2
#define interruptPin1 D5
#define interruptPin2 D8

int brightness = 150;
String color="#FFFFFF";
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, NeoPIN, NEO_RGB + NEO_KHZ800);
char auth[] = "Your Blynk token"; //put your blynk token here
void setNeoColor(String value);
void handleNotFound();
void handleRoot();
//volatile byte interruptCounter1 = 0;
//volatile byte interruptCounter2 = 0;

int last1;
int last2;
int rel1 = 0;
int rel2 = 0;
const int led = 13;

void rel1on(){
  rel1 = 0;
  digitalWrite(RELPIN1,rel1);
  server.send (200);
}

void rel1off(){
  rel1 = 1;
  digitalWrite(RELPIN1,rel1);
  server.send (200);
}

void rel2on(){
  rel2 = 0;
  digitalWrite(RELPIN2,rel2);
  server.send (200);
}

void rel2off(){
  rel2 = 1;
  digitalWrite(RELPIN2,rel2);
  server.send (200);
}

//ICACHE_RAM_ATTR void handleInterrupt1() {
//  interruptCounter1++;
//}
//ICACHE_RAM_ATTR void handleInterrupt2() {
//  interruptCounter2++;
//}

void printhead(){
    int head = server.headers();
    Serial.printf("number of headers rec: ", head);
    int i;
    for(i=0;i<head;i++){
      Serial.printf(server.headerName(i).c_str(), server.header(server.headerName(i).c_str()).c_str());
    }
}
void brightn(){
  if (server.hasHeader("BRIGHTNESS")) {
    Serial.print("set brightness :");
    int b=server.header("BRIGHTNESS").toInt();
    Serial.println(b);
    strip.setBrightness(b);
    strip.show();
    server.send (200);
  }
 server.send (300);
}

void setNeoColor1(){
  if (server.hasHeader("COLOR")) {
    Serial.print("req color :");
    color=server.header("COLOR");
    Serial.println(color);
    setNeoColor(color);
    server.send (200);
  }
 server.send (300);
 }

BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
    rel1 = pinValue;
    digitalWrite(RELPIN1,rel1);

}
BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  rel2 = pinValue;
  digitalWrite(RELPIN2,rel2);
}
BLYNK_WRITE(V3)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  strip.setBrightness(pinValue);
  strip.show();
}
BLYNK_WRITE(V4)
{
  int r = param[0].asInt(); // assigning incoming value from pin V1 to a variable
  int g = param[1].asInt();
  int b = param[2].asInt();
  String col="#";
  col+=String(r, HEX);
  if (col.endsWith("0")) col += "0";
  col+=String(g, HEX);
  if (col.endsWith("0")) col += "0";
  col+=String(b, HEX);
  if (col.endsWith("0")) col += "0";
  color= col;
  setNeoColor(color);
}

void setup ( void ) {
  Serial.begin ( 115200 );
  pinMode(interruptPin1, INPUT);
  pinMode(interruptPin2, INPUT);
//  attachInterrupt(digitalPinToInterrupt(interruptPin1), handleInterrupt1, CHANGE);
//  attachInterrupt(digitalPinToInterrupt(interruptPin2), handleInterrupt2, CHANGE);
  // ##############
  // NeoPixel start
  Serial.println();
  strip.setBrightness(brightness);
  strip.begin();
  strip.show(); 
  delay(50);
  Serial.println("NeoPixel started");
  pinMode(RELPIN1, OUTPUT);
  pinMode(RELPIN2, OUTPUT);
  digitalWrite ( RELPIN1, rel1 );
  digitalWrite ( RELPIN2, rel2 );
  // #########
  // Webserver
  pinMode ( led, OUTPUT );
  digitalWrite ( led, 0 );
  
  //WiFi.begin ( ssid, password );
  
  
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("connected...yeey :)");
  Serial.println ( "" );
  Blynk.config(auth);
  ArduinoOTA.setHostname("RoomControl-Saim");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
       type = "sketch";
    } else { // U_FS
       type = "filesystem";
    }
    
    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
     Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();

  // Wait for connection
//  while ( WiFi.status() != WL_CONNECTED ) {
//    delay ( 500 );
//    Serial.print ( "." );
//  }

//  Serial.println ( "" );
//  Serial.print ( "Connected to " );
//  Serial.println ( ssid );
//  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }

  // what to do with requests
  server.on ( "/", handleRoot );
  server.on ( "/11", rel1on );
  server.on ( "/10", rel1off );
  server.on ( "/21", rel2on );
  server.on ( "/20", rel2off );
  server.on ( "/Bright", brightn);
  server.on ( "/color", setNeoColor1);
  server.on("/update", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", updateindex);
    });
  server.on("/updater", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
    }, []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace)) { //start with max available size
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      yield();
    });
  server.onNotFound ( handleNotFound );
  const char * headerkeys[] = {"COLOR","BRIGHTNESS"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  server.collectHeaders(headerkeys, headerkeyssize );
  server.begin();
  Blynk.run();
  Serial.println ( "HTTP server started" );
  setNeoColor("#FFFFFF");
}

void loop ( void ) {
  // waiting fo a client
  server.handleClient();
  ArduinoOTA.handle();
  Blynk.run();
//  if(interruptCounter1>0){
//      interruptCounter1=0;
//      Serial.println("An interrupt has occurred for Input 1");
//      if(rel1==1){
//        rel1=0;
//      }
//      else{
//        rel1=1;
//      }
//      digitalWrite(RELPIN1,rel1);
//  }
//  if(interruptCounter2>0){
//      interruptCounter2=0;
//      Serial.println("An interrupt has occurred for Input 2");
//      if(rel2==1){
//        rel2=0;
//      }
//      else{
//        rel2=1;
//      }
//      digitalWrite(RELPIN2,rel2);
//  }
}


void handleRoot() {
  Serial.println("Client connected");
  digitalWrite ( led, 1 );
  // data from the colorpicker (e.g. #FF00FF)
  // setting the color to the strip 
  // building a website
  int sec = millis() / 1000;
  int mint = sec / 60;
  int hr = mint / 60;
  String page = "<html lang=fr-FR><head>\n";
  page += "<title>Saim's Room</title>\n";
  page += "<style> body { background-color: #ffffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>\n";
  page += "<meta name='viewport' content='width=device-width, height=device-height, initial-scale=1.0, user-scalable=0, minimum-scale=1.0, maximum-scale=1.0' />\n";
  page += "<link href='data:image/x-icon;base64,AAABAAEAEBAAAAAAAABoBAAAFgAAACgAAAAQAAAAIAAAAAEAIAAAAAAAAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAJuWADKgmgAAm5UAFpqVAAablgAsnJYASpmUAAKblgAnm5UAKJuWAD6clgBKm5YAJZqUAASalQAJm5YAXZyXAACalQAXm5YAo1xmAACblgDrm5UAGpuWAHablgD5m5YA8ZuWAOCblgAAm5YAfpuWAIOblgDnm5UAPZuWAAeblgCam5UAAJuWAP+TkAACm5YA4ZuWADWblgDtm5YAiJuWAPGblgDgm5YAv5uWAPKblgB+mZUAEJqVAB+blgDgm5YAMQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAm5YAJZuWAP+clwD/nJcA/5uWAP6blgAiAAAAAAAAAACblgAbm5YA/pyXAP+clwD/m5YA/5uWAC4AAAAAm5UAGZyXAP+blgDVmpQAC5qVAA2blgDZnJcA/5uWAFGblgBCnJcA/5uWAOGalAAQmpUACpuWAMyclwD/m5YAJZuWAPublgDmmZUAAAAAAAAAAAAAAAAAAJuWAJKclwD/nJcA/5uWAKackwAAm5UANpqUABWZlQAAm5YA25uWAP6clwD/m5YAQZuXAACblwD5m5cA+ZqXAAaZlAAAm5YA/5uWAP+alQAAmpcABpuWAPublgD6nZgAAJuWACyclwD/nJcA/5uWAJ8AAAAAAAAAAAAAAAAAAAAAm5YAGpyXAP+clwD/mpYAIgAAAACblgBcm5UAJAAAAACblgCLnJcA/5uWAHmclwD/m5YALAAAAAAAAAAAm5YAL5yXAP+blgDXm5YAy5yXAP+blQA3AAAAAAAAAACblQAknJcA/5uWAIyZlQAAm5YAxpyXAP+clwD/nJcA/5yXAP+blgDHmpQAAJqXAACblgC7nJcA/5yXAP+clwD/nJcA/5uWANGakQAGAAAAAAAAAACYkgACm5YAVZuWAFGYlAABAAAAAAAAAAAAAAAAAAAAAJeUAACblgBOm5YAWJmSAAOZjwAAm5MAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA//8AAP//AAD//wAArGYAAKgdAAD//wAAw8MAAJmZAAA8PAAAZmYAAD58AAC8PAAAgYEAAP//AAD//wAA//8AAA==' rel='icon' type='image/x-icon' />\n";
  page += "</head><body style='text-align: center;'><h1>S✯S Technologies</h1>\n";
  page += "<h2>Control</h2>\n";
  page += "<hr>";
  page += "<h3>Status</h3>\n";
  page += "<p>Uptime : ";
  page += hr;
  page += ":";
  page += mint;
  page += ":";
  page += sec;
  page += "</p>\n";
  page += "<hr>";
  page += "<p> current clr : ";
  page += "<span id='pikachu'>"+color+"</span></p>\n";
  page += "<p><input type='color' onchange='setTextColor()' id='picker' value="+color+"></p>\n";
  page += "<p><input type='range' onchange='setBrightness()' id='Brightness' value="+String(brightness)+" min='1' max='255'>\n";
  page += "</p>\n";
  page += "<script>\n";
  page += "var xhttp=new XMLHttpRequest();\n";
  page += "function setTextColor() {\n";
  page += "var x = document.getElementById('picker').value;\n";
  page += "document.getElementById('pikachu').innerHTML= x.toString();\n";
  page += "xhttp.open('POST', '/color', true);\n";
  page += "xhttp.setRequestHeader('COLOR', x.toString());\n";
  page += "xhttp.send();\n";
  page += "}\n";
  page += "function setBrightness() {\n";
  page += "var x = document.getElementById('Brightness').value;\n";
  page += "xhttp.open('POST', '/Bright', true);\n";
  page += "xhttp.setRequestHeader('BRIGHTNESS', x.toString());\n";
  page += "xhttp.send();\n";
  page += "}\n";
page += "function ON1Rel() {\n";
page += "var xhttp = new XMLHttpRequest();\n";
page += "xhttp.onreadystatechange = function() {\n";
page += "if (this.readyState == 4 && this.status == 200) {\n";
page += "document.getElementById('Rel1B').innerHTML = 'OFF';\n";
page += "document.getElementById('Rel1B').onclick = OFF1Rel;\n";
page += "}\n";
page += "};\n";
page += "xhttp.open('GET', '/11', true);\n";
page += "xhttp.send();\n";
page += "}\n";
page += "function OFF1Rel() {\n";
page += "var xhttp = new XMLHttpRequest();\n";
page += "xhttp.onreadystatechange = function() {\n";
page += "if (this.readyState == 4 && this.status == 200) {\n";
page += "document.getElementById('Rel1B').innerHTML = 'ON';\n";
page += "document.getElementById('Rel1B').onclick = ON1Rel;\n";
page += "}\n";
page += "};\n";
page += "xhttp.open('GET', '/10', true);\n";
page += "xhttp.send();\n";
page += "}\n";
page += "function ON2Rel() {\n";
page += "var xhttp = new XMLHttpRequest();\n";
page += "xhttp.onreadystatechange = function() {\n";
page += "if (this.readyState == 4 && this.status == 200) {\n";
page += "document.getElementById('Rel2B').innerHTML = 'OFF';\n";
page += "document.getElementById('Rel2B').onclick = OFF2Rel;\n";
page += "}\n";
page += "};\n";
page += "xhttp.open('GET', '/21', true);\n";
page += "xhttp.send();\n";
page += "}\n";
page += "function OFF2Rel() {\n";
page += "var xhttp = new XMLHttpRequest();\n";
page += "xhttp.onreadystatechange = function() {\n";
page += "if (this.readyState == 4 && this.status == 200) {\n";
page += "document.getElementById('Rel2B').innerHTML = 'ON';\n";
page += "document.getElementById('Rel2B').onclick = ON2Rel;\n";
page += "}\n";
page += "};\n";
page += "xhttp.open('GET', '/20', true);\n";
page += "xhttp.send();\n";
page += "}\n";
  page += "\n";
  page += "\n";
  page += "</script>\n";
  if (rel1==1) {
  page += "<p><button id='Rel1B' onclick='ON1Rel()'>ON</button></p>\n";
  } else {
  page += "<p><button id='Rel1B' onclick='OFF1Rel'>OFF</button></p>\n";
  }
  if (rel2==1) {
  page += "<p><button id='Rel2B' onclick='ON2Rel()'>ON</button></p>\n";
  } else {
  page += "<p><button id='Rel2B' onclick='OFF1Rel'>OFF</button></p>\n";
  }
  page += "<a href='/update'>Update</a>\n";
  page += "</body></html>\n";
  server.send ( 200, "text/html", page);
  digitalWrite ( led, 0 );
}

void handleNotFound() {
  digitalWrite ( led, 1 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite ( led, 0 );
}



void setNeoColor(String value){
  Serial.print("Setting Neopixel...");
  // converting Hex to Int
  int number = (int) strtol( &value[1], NULL, 16);
  
  // splitting into three parts
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;
  
  // DEBUG
  Serial.print("RGB: ");
  Serial.print(r, DEC);
  Serial.print(" ");
  Serial.print(g, DEC);
  Serial.print(" ");
  Serial.print(b, DEC);
  Serial.println(" ");
  
  // setting whole strip to the given color
  for(int i=0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color( g, r, b ) );
  }
  // init
  strip.show();
  
  Serial.println("on.");
}
