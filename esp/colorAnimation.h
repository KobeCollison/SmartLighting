#include <Arduino_JSON.h>
#include <EEPROM.h>


#define EEPROM_SIZE 512
#define EEPROM_NAME 0
#define EEPROM_DATA 128

struct color{
  float r;
  float g;
  float b;
};

// List node of color data to cycle through for animation
class ColorNode{
public: 
  ColorNode* next;

  color colorVal;
  int transitionTime;
  int lingerTime;

  // listNode loops back to the head Node
  // for this reason, have a bool var to check if this is the final node if needed
  bool isEnd;
  JSONVar source;

  ColorNode(color cData,int tTime,int lTime);
  ColorNode(JSONVar nodeData);
  ColorNode();

  String toString();
};


// use 3 factor distance formula to quantify distacne between to color values
float colorDistance(color c1, color c2);

// Linked List of color data objects

class ColorAnimation {
  private: 
  ColorNode* head;
  bool debug;

  // the start and end target of a single transition
  ColorNode *currentColor, *targetColor;

//The JSON/String used to construct this object
  JSONVar source;

  bool isActive;
  int colorIndex;

  // gpio pins for each color
  int red_pin, green_pin, blue_pin;

  color displayColor;

  // decrements down to 0 each handleAnimation tick to clock when to go to next step
  int transitionTicksRemaining, lingerTicksRemaining;

  // delta values, change in color in a single tick
  float dR,dG,dB;

  public:
  
  ColorAnimation(bool debug = false){
    this->isActive = false;
    this->debug = debug;
  };

  bool isValid();

  void setPins(int rpin, int gpin, int bpin);
  void print();

  // runs every loop and coordinates other functions
  void handleAnimation();

  void startAnimation();
  void startTransition();
  void setColors(String newData, int writeAddr = EEPROM_DATA);

  void saveString(String data,int writeAddr = 0, int endAddr = EEPROM_SIZE);
  void loadString(String &data,int readAddr = 0, int endAddr = EEPROM_SIZE);

  String loadName(int readAddr = EEPROM_NAME, int endAddr = EEPROM_DATA); 
  String loadColors(int readAddr = EEPROM_DATA,int endAddr = EEPROM_SIZE);

  void saveName(String newName, int writeAddr = EEPROM_NAME, int endAddr = EEPROM_DATA);

  void appendColorNode(ColorNode* newColor);

  void setDebug(bool debug);
  String toString();

};