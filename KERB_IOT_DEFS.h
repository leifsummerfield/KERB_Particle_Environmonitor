// -----------------------------------------
// Function and Variable with Photoresistors
// -----------------------------------------

// At the top if you please, very Functional variables...stuff that we might like to change
int OnTime = 19;  //  and this is 5 PM in Euro 24h rubbish
int OffTime = 6;  //that's 6:00 AM

int tempTripPoint_1 = 67;
float badGasContrn = 0;
float badGasConversion = 0.0153;
int   badGasOffset = -4;
float partclsAveraged =0;

// Sound detection addition
unsigned long sound_Width;
unsigned long sound_Duration=0;
float soundAveraged = 0;

// We're going to start by declaring which pins everything is plugged into.

// PIN Definitions
int anNoise = A0;
int gasSensor = A4;


int led = D7;
int switch1 = D7; // This is where your LED is plugged in. The other side goes to a resistor connected to GND.
int switch2 = D6; //
int switch3 = D5; //
int switch4 = D4; //

int pDustIn = D2;


float temp;  //From the SI7021
float humidity;  // From SI7021
