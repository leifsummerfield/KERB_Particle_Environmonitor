// -----------------------------------------
// Function and Variable with Photoresistors
// -----------------------------------------

// At the top if you please, very Functional variables...stuff that we might like to change
int OnTime = 19;  //  and this is 5 PM in Euro 24h rubbish
int OffTime = 6;  //that's 6:00 AM

int tempTripPoint_1 = 67;

// We're going to start by declaring which pins everything is plugged into.

// PIN Definitions
int anNoise = A0;

int TMP36_Two = A1;
int TMP36_One = A2;
int photoRes2 = A3; // This is where your photoresistor is plugged in. The other side goes to the "power" pin (below).
int photoRes1 = A4;
int notConnected2 = A5;

int led = D7;
int switch1 = D7; // This is where your LED is plugged in. The other side goes to a resistor connected to GND.
int switch2 = D6; //
int switch3 = D5; //
int switch4 = D4; //

int neoPixlePin = D2;


double tempOne;  //From the SI7021
double tempTwo;  //From TMP36_One
double tempThree; //From TMP36_Two
double humidity;  // From SI7021

double lightOne;  //From photo1
double lightTwo;  //From photo2

double analogvalue; // Here we are declaring the integer variable analogvalue, which we will use later to store the value of the photoresistor.
double voltage;

float rHumidity;
// Measure Temperature
float temp;



// Alarm clock variables
byte hour;
