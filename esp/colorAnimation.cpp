#include "colorAnimation.h"
#include <cmath>


float colorDistance(color c1, color c2){

  // get RGB values form colors
  int x1,y1,z1,x2,y2,z2;

  x1 = c1.r;
  y1 = c1.g;
  z1 = c1.b;

  x2 = c2.r;
  y2 = c2.g;
  z2 = c2.b;

  float base = pow(x1-x2,2) + pow(y1-y2,2) + pow(z1-z2,2);
  return sqrt(base);
}

String ColorNode::toString(){
  Serial.print( this->source);
}

void ColorAnimation::setPins(int rpin, int gpin, int bpin){
    this->red_pin = rpin;
    this->green_pin = gpin;
    this->blue_pin = bpin;
}

void ColorAnimation::handleAnimation(){
  // if there is no active animation, return
  if (!this->isActive){
    return;
  }
  // if stil linger time remaining, reduce time then pass
  if(this->lingerTicksRemaining > 0){
    this->lingerTicksRemaining--;
    return;

  // after linger period is finished, start transition ticks
  // 
  } else if(this->transitionTicksRemaining > 0){

    // apply change in color
    this->displayColor.r = displayColor.r + this->dR;
    this->displayColor.g = displayColor.g + this->dG;
    this->displayColor.b = displayColor.b + this->dB;

    // // print values
    // Serial.print("r_");
    // Serial.print(this->displayColor.r);
    // Serial.print("g_");
    // Serial.print(this->displayColor.g);
    // Serial.print("b_");
    // Serial.println(this->displayColor.b);

    this->transitionTicksRemaining--;

    // update LED pins
    analogWrite(this->red_pin, int(this->displayColor.r));
    analogWrite(this->green_pin, int(this->displayColor.g));
    analogWrite(this->blue_pin, int(this->displayColor.b));

    
    return;
  }
  // if linger ticks and transition ticks have finished, move to next node
  if(this->lingerTicksRemaining==0 and this->transitionTicksRemaining==0){
    this->currentColor = this->targetColor;
    this->startTransition();
  }
  
}

ColorNode::ColorNode(color cData,int tTime,int lTime){
    this->colorVal = cData;
    this->transitionTime = tTime;
    this->lingerTime = lTime;
    this->next = nullptr;
  }

ColorNode::ColorNode(JSONVar nodeData){
  this->source = nodeData;
  this->next = nullptr;

  color rgb;
  rgb.r = int(nodeData["color"]["r"]);
  rgb.g = int(nodeData["color"]["g"]);
  rgb.b = int(nodeData["color"]["b"]);

  this->colorVal = rgb;
  this->transitionTime = nodeData["transitionTime"];
  this->lingerTime = nodeData["lingerTime"];
}

void ColorAnimation::saveString(String data, int writeAddr, int endAddr){

    if(data.length() > endAddr){
        Serial.println("Data too long to save");
        return;
    } else if (data.length() == 0){
        Serial.println("Data is empty");
        return;
    }

    for (int i = 0; i < data.length(); i++) {
        EEPROM.write(writeAddr + i, data[i]);
    }

    EEPROM.write(writeAddr + data.length(), '\0');
    if (EEPROM.commit()) {
      Serial.println("EEPROM successfully committed");
    } else {
      Serial.println("ERROR! EEPROM commit failed");
    }
    EEPROM.end();
}

void ColorAnimation::loadString(String &data, int readAddr, int endAddr){
    char c;
    data = "";
    int i = 0;
    while(i<endAddr){ {
        c = EEPROM.read(readAddr);

        readAddr++;
        if (c == '\0' || c == 0xFF) {
            break;
        }
        data += c;
    }
  }
}

void ColorAnimation::saveName(String newName, int writeAddr, int endAddr){
  if(newName.length() > endAddr){
    Serial.println("Name too long to save");
    return;
  } else if (newName.length() == 0){
    Serial.println("Name is empty");
    return;
  }

  for (int i = 0; i < newName.length(); i++) {
    EEPROM.write(writeAddr + i, newName[i]);
  }

  EEPROM.write(writeAddr + newName.length(), '\0');
  if (EEPROM.commit()) {
    Serial.println("EEPROM successfully committed");
  } else {
    Serial.println("ERROR! EEPROM commit failed");
  }
  EEPROM.end();
}

String ColorAnimation::loadName(int readAddr, int endAddr){
  String loadData;
  this->loadString(loadData,readAddr,endAddr);
  if (loadData.length() == 0){
    Serial.println("No name to load");
    loadData = "Unnamed Node";
  }
  else{
    Serial.print("Loaded name: ");
    Serial.println(loadData);
  }
  return loadData;
}

String ColorAnimation::loadColors(int readAddr, int endAddr){
  String loadData;
  this->loadString(loadData,readAddr,endAddr);
  if (loadData.length() == 0){
    Serial.println("No color data to load");
    loadData = "";
  }
  else{
    Serial.print("Loaded color data: ");
    Serial.println(loadData);
  }
  return loadData;
  
  
}   

void ColorAnimation::setDebug(bool debug){
  this->debug = debug;
}

String ColorAnimation::toString(){
  return this->source;
}

void ColorAnimation::setColors(String newData, int writeAddr){
  this->source = JSON.parse(newData);

  // check if message is valid to change colors
  // JSON.typeof(jsonVar) can be used to get the type of the variable
  if (JSON.typeof(this->source) == "undefined") {
    Serial.println("Parsing input failed!");
    this->source = JSON.parse("{}");
    return;
  }

  // if valid, delete current list and replace 
  //if(WIP)

  //note, clear memory
  this->head = nullptr;

  // saveString(newData, writeAddr, EEPROM_SIZE);


  ColorNode* currentColorNode;
  // assemble color list from the data
  for (int i=0; i<this->source.keys().length();i++){
    JSONVar currentObject = this->source[this->source.keys()[i]];
    currentColorNode = new ColorNode(currentObject);
    // color rgb;
    // rgb.r = currentObject["color"]["r"];
    // rgb.g = currentObject["color"]["g"];
    // rgb.b = currentObject["color"]["b"];

    // // add the current color node data to the linked list
    // currentColorNode = new ColorNode(rgb,currentObject["transitionTime"],currentObject["lingerTime"]);
    
    this->appendColorNode(currentColorNode);
  }
  

  // this->startAnimation();
}

bool ColorAnimation::isValid(){

  if (JSON.typeof(this->source) == "undefined") {
    
    Serial.println("is valid ");
    return false;
  }else{
    Serial.println("is valid ");
    return true;
  }
}

void ColorAnimation::appendColorNode(ColorNode* newColor){

// if the list is empty, set as the head
  if (this->head == nullptr){
    this->head = newColor;
  } else {
  // if list is already populated, go to the final element and add this node as the next
    ColorNode *currentNode = this->head;
    while(currentNode->next != nullptr){
      currentNode = currentNode->next;
    }
    currentNode->next = newColor;
  }

}

void ColorAnimation::print(){
  ColorNode *currentNode = this->head;
    while(currentNode->next != nullptr){
    Serial.print("(r:");
    Serial.print(currentNode->colorVal.r);
    Serial.print("g:");
    Serial.print(currentNode->colorVal.g);
    Serial.print("b_:");
    Serial.print(currentNode->colorVal.b);
    Serial.print(") >>");
    
    currentNode = currentNode->next;
    }
    Serial.print("(r:");
    Serial.print(currentNode->colorVal.r);
    Serial.print("g:");
    Serial.print(currentNode->colorVal.g);
    Serial.print("b_:");
    Serial.print(currentNode->colorVal.b);
    Serial.println(")");
}


void ColorAnimation::startAnimation(){
  this->currentColor = this->head;
  this->startTransition();
  this->isActive  = true;
}

void ColorAnimation::startTransition(){
  this->transitionTicksRemaining = this->currentColor->transitionTime;
  this->lingerTicksRemaining = this->currentColor->lingerTime;
  
  // set up transition variables
  this->targetColor = currentColor->next;

  // if at end of list, loop back to beginning
  if(this->targetColor == nullptr){
    this->targetColor = this->head;
  }

  if (this->debug){
    Serial.print("Transitioning from ");
    Serial.print(this->currentColor->source);
    Serial.print(" to ");
    Serial.println(this->targetColor->source);
  } 


  // update delta values
  this->dR = (this->targetColor->colorVal.r - this->currentColor->colorVal.r)/this->currentColor->transitionTime;
  this->dG = (this->targetColor->colorVal.g - this->currentColor->colorVal.g)/this->currentColor->transitionTime;
  this->dB = (this->targetColor->colorVal.b - this->currentColor->colorVal.b)/this->currentColor->transitionTime;

  // assign display color
  this->displayColor.r = this->currentColor->colorVal.r;
  this->displayColor.g = this->currentColor->colorVal.g;
  this->displayColor.b = this->currentColor->colorVal.b;

  analogWrite(this->red_pin, this->displayColor.r);
  analogWrite(this->green_pin, this->displayColor.g);
  analogWrite(this->blue_pin, this->displayColor.b);

}