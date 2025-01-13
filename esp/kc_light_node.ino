// #include <Arduino_JSON.h>

#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include "colorAnimation.h"

#ifndef STASSID
// #define STASSID "Verizon_9RX94B"
// #define STAPSK  "ripe3-sky-clash"
#endif



// const char* ssid     = STASSID;
// const char* password = STAPSK;

ESP8266WebServer server(80);

// IPAddress connectedIPs[10];
// int connectedDevices=0;
JSONVar connectedIPs;

WiFiClient client;
HTTPClient http, http2;
bool isHost = true;
IPAddress hostIP,currentIP;

String childNodes[5];
int childCount = 0;
const String domain_name = "kc_lumen";

// JsonDocument doc;

const int led = LED_BUILTIN;

// Red, green, and blue pins for PWM control
const int redPin = 13;     // 13 corresponds to GPIO13
const int greenPin = 12;   // 12 corresponds to GPIO12
const int bluePin = 14;    // 14 corresponds to GPIO14

// Setting PWM bit resolution
const int resolution = 256;

ColorAnimation nodeLED;

String nodeName, colorData;

const String postForms = "<html>\
  <head>\
    <title>ESP8266 Web Server POST handling</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>POST plain text to /postplain/</h1><br>\
    <form method=\"post\" enctype=\"text/plain\" action=\"/postplain/\">\
      <input type=\"text\" name=\'{\"hello\": \"world\", \"trash\": \"\' value=\'\"}\'><br>\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
    <h1>POST form data to /postform/</h1><br>\
    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postform/\">\
      <input type=\"text\" name=\"hello\" value=\"world\"><br>\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
  </body>\
</html>";

void handleRoot() {
  String ipHeader = "<h1>Node Name: " + currentIP.toString() + "</h1><br>";

  digitalWrite(led, 1); 
  server.send(200, "text/html", postForms);
  server.send(200,"text/html",ipHeader);
  digitalWrite(led, 0);
}

void handlePlain() {
  if (server.method() != HTTP_POST) {
    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  } else {
    digitalWrite(led, 1);
    server.send(200, "text/plain", "POST body was:\n" + server.arg("plain"));
    digitalWrite(led, 0);
    // JSONVar myObject = JSON.parse(server.arg("plain"));
    // root.printTo(Serial);
    Serial.println("POST body was:\n" + server.arg("plain"));
  }
}


void handleChildIP() {
  if (server.method() != HTTP_POST) {
    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  } else {
    
    digitalWrite(led, 1);
    server.send(200, "text/plain", "POST body was:\n" + server.arg("plain"));
    digitalWrite(led, 0);
    // Serial.println("POST body was:\n" + server.arg("plain"));

    const String newChildIP = server.arg("plain");
    newChildNode(newChildIP);

    Serial.print("connectedIPs: ");
    Serial.println(connectedIPs);
  }
}

void newChildNode(String nodeData) {
    Serial.println("Adding new child node");
    Serial.println(nodeData);
    JSONVar newNode = JSON.parse(nodeData);

    //verify json parse
    if (JSON.typeof(newNode) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }

    connectedIPs[JSON.stringify(newNode["address"])] = JSON.stringify(newNode["nodeData"]);
}

void handleColorChange() {
  if (server.method() != HTTP_POST) {
    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  } else {
    digitalWrite(led, 1);
    server.send(200, "text/plain", "POST body was:\n" + server.arg("plain"));
    digitalWrite(led, 0);
    Serial.println("POST body was:\n" + server.arg("plain"));
    nodeLED.setDebug(true);
    nodeLED.saveString(server.arg("plain"), EEPROM_DATA, EEPROM_SIZE);
    nodeLED.setColors(server.arg("plain"),0);
    if (nodeLED.isValid()){
      nodeLED.print();
      nodeLED.startAnimation();
    }
    return;
    // TEMPOARY, CHANGE THIS
    // experimentation with color http request
    JSONVar colorPacketData = JSON.parse(server.arg("plain"));
    // verify json parse
    if (JSON.typeof(colorPacketData) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }

    // Serial.println(myObject.keys());

    String index1 = colorPacketData.keys()[0];
    JSONVar color1 = colorPacketData[index1]["color"];
    Serial.print("Displaying color: ");
    Serial.println(color1);
    
    //change LED Color
    analogWrite(redPin, color1["r"]);
    analogWrite(greenPin, color1["g"]);
    analogWrite(bluePin, color1["b"]);
  }
}

void handleGetNetwork() { 
    digitalWrite(led, 1);
    //   String message = "Connected devices:\n";
    //   for (uint8_t i = 0; i < connectedIPs.keys().length(); i++) {
    //     message += " " + String(connectedIPs.keys()[i]) + ": " + String(connectedIPs[connectedIPs.keys()[i]]) + "\n";
    //   }
    String message = JSON.stringify(connectedIPs);
    Serial.println("GET connectedIPs");
    Serial.println(message);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", message);
    digitalWrite(led, 0);
}

void handleForm() {
  if (server.method() != HTTP_POST) {
    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  } else {
    digitalWrite(led, 1);
    String message = "POST form was:\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(200, "text/plain", message);
    Serial.println("POST body was:\n" + message);
    digitalWrite(led, 0);
  }
}

void handleRename() {
  if (server.method() != HTTP_POST) {
    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  } else {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    String message = server.arg("plain");

    Serial.println("POST body was:\n" + message);

    JSONVar messageData = JSON.parse(message);
    // verify json parse
    if (JSON.typeof(messageData) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }

    String newName = messageData["name"];

    digitalWrite(led, 1);
    server.send(200, "text/plain", "Changing name to:\n" + newName);
    digitalWrite(led, 0);
    Serial.println("Changing name to:\n" + newName);

    nodeLED.saveName(newName,EEPROM_NAME);

  }
}



void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  // configure LED PWM resolution/range and set pins to LOW
  analogWriteRange(resolution);
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);


  Serial.begin(115200);
  // WiFi.begin(ssid, password);
  EEPROM.begin(EEPROM_SIZE);

  delay(2000);
  nodeName = nodeLED.loadName(EEPROM_NAME);
  colorData = nodeLED.loadColors(EEPROM_DATA); 
  // nodeLED.setColors(colorData,EEPROM_DATA);
  // if (nodeLED.isValid()){
  //   nodeLED.print();
  //   nodeLED.startAnimation();
  // }   

  Serial.println("");


  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  bool res;
  res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected to wifi");
    }

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  // Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  currentIP = WiFi.localIP();

  server.begin();
  
  Serial.println("HTTP server started");

  // add server node to connectedIPs
  connectedIPs[WiFi.localIP().toString()]["name"] = nodeName;
  connectedIPs[WiFi.localIP().toString()]["colorData"] = nodeLED.toString();;
  
  Serial.print("connectedIPs: ");

  
    int httpCode = atteptConnection();
    // if client could not connect http, create a server    
    if (httpCode <= 0) {
        if (MDNS.begin(domain_name.c_str())) { 
            Serial.println("MDNS responder started");
        }
        startServer();
    }
}

bool checkIfHost(){
  if(MDNS.isRunning()){
    Serial.println("MDNS is running");
    isHost = true;

  }
  else{
    Serial.println("MDNS is not running");
    isHost = false;
  }
  return isHost;
}


void startServer(){
  server.on("/", handleRoot);
  server.on("/postplain/", handlePlain);
  server.on("/postform/", handleForm);
  server.on("/postchildip/", handleChildIP);
  server.on("/colorChange/", handleColorChange);
  server.on("/renameDevice/", handleRename);
  server.on("/network/", HTTP_GET, handleGetNetwork);
  server.on("/host/", HTTP_GET, handleGetHost);
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("HTTP server started");
  if (MDNS.begin(domain_name.c_str())) { 
        Serial.println("MDNS responder started");
      }
} 


void handleGetHost(){ 
  
}


int atteptConnection(){// attempt client connection 
  int httpCode;
  Serial.print("[HTTP] begin...\n");
  String postMessage;

  // send post of the IP address of the child node and nodeLED.source
  String post_addr = WiFi.localIP().toString();
  String post_name = nodeName;
  String post_colors = colorData;

  JSONVar postMessageData;
  postMessageData["address"] = post_addr;
  postMessageData["name"] = post_name;
  postMessageData["colorData"] = post_colors;

  postMessage = JSON.stringify(postMessageData);


  for (int i = 0; i < 5; i++) {
    if (http.begin(client,"http://"+domain_name+".local/postchildip/")) {  // HTTP
      http.addHeader("Content-Type", "application/json");

      Serial.print("[HTTP] POST...\n");
      // start connection and send HTTP header
      httpCode = http.POST(postMessage);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
          break;
        }
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
    delay(250);
    }
    return httpCode;}



int tick = 0;
int postStatus = 0;
void loop(void) {
  // handleClients();  
    server.handleClient();
    MDNS.update();
    nodeLED.handleAnimation();

    // if server of domain name is taken by another device, become a client


    if(tick%1000==0 ){
      Serial.print("Checking if host ");
      checkIfHost();
      Serial.println(MDNS.begin(domain_name.c_str()));
        postStatus = atteptConnection();
        if (postStatus <= 0) {
            startServer();
        }
    }

    tick++; 
    delay(10);
  
}
