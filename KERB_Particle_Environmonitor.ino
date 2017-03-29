//KERBSPACE IOT SYSTEM CONTROLLER
//Written by Leif Summerfield - Fall 2016
//Rev 1   Oct 13  - First psudo code of the whole shebang
//                  and working well : temp sensors, humidity, logging to terminal and Thingspeak.

//KERB specific definitions and variables
#include "KERB_IOT_DEFS.h"

//Adding THingspeak support
/*
  *****************************************************************************************
  **** Visit https://www.thingspeak.com
  ****  The video tutorial http://community.thingspeak.com/tutorials/thingspeak-channels/
  *****************************************************************************************/
#include "Thingspeak.h"
  TCPClient client;
  unsigned long myChannelNumber = 249857;
  const char * myWriteAPIKey = "VFZPOB94MR02BG5E";

//Adding Running average support section here
#include "RunningAverage.h"

//Setup for noise sensors
int sum = 0;
char noiseString[40];

void setup() {

// First, declare all of our pins. This lets our device know which ones will be used for outputting voltage, and which ones will read incoming voltage.
    pinMode(anNoise, INPUT);
    pinMode(TMP36_One, INPUT);
    pinMode(TMP36_Two, INPUT);
    pinMode(photoRes2, INPUT);  // Our photoresistor pin is input (reading the photoresistor)
    pinMode(photoRes1, INPUT);

    pinMode(switch1, OUTPUT);   digitalWrite(switch1,LOW);
    pinMode(switch2, OUTPUT);   digitalWrite(switch2,LOW);
    pinMode(switch3, OUTPUT);   digitalWrite(switch3,LOW);
    pinMode(switch4, OUTPUT);   digitalWrite(switch4,LOW);


    // We are going to declare a Spark.variable() here so that we can access the value of the photoresistor from the cloud.
    Particle.variable("Noise", &sum, INT);

    // We are also going to declare a Spark.function so that we can turn the LED on and off from the cloud.
    //Particle.function("led",ledToggle);
    Particle.function("Switch1",toggler1);
    Particle.function("Switch2",toggler2);
    Particle.function("Switch3",toggler3);
    Particle.function("Switch4",toggler4);


    //  ThingSpeak setup;
      ThingSpeak.begin(client);



}


// Next is the loop function...
  /*****************************************************************************************/
void loop() {

//SECTION 1  - Collect data           ==========================================

for(int i=0; i<100; i++)
{
    sum += analogRead(anNoise);
    sum = sum - 900;    //background subtraction factor
    delay(10);
}


      sprintf(noiseString,"%d",sum);
      Particle.publish("Noise",String(sum));
      //Particle.publish("OneSample", String(analogRead(anNoise)-900));
      digitalWrite(D7,HIGH);
      // Write the fields that you've set all at once.
        ThingSpeak.setField(1,sum);
        ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

      delay(1000);
      digitalWrite(D7,LOW);
      delay(100);
      sum = 0;






}
// -----------------------------------------------------end main loop -----------------------------
//-------------------------------------------------------------------------------------------------


double readTmp36(int channel){
  // check to see what the value of the photoresistor is and store it in the int variable analogvalue
  analogvalue = analogRead(channel);

// converting that reading to voltage, for 3.3v arduino use 3.3

analogvalue = analogvalue*3.3;
analogvalue = analogvalue/4095;

// now print out the temperature
 //converting from 10 mv per degree wit 500 mV offset
analogvalue = (analogvalue) * 100 - 50;
                                             //to degrees ((voltage - 500mV) times 100)
// now convert to Fahrenheit
return analogvalue = (analogvalue * 9.0 / 5.0) + 32.0;


}

// check to see what the value of the photoresistor is and store it in the int variable analogvalue
double readLight(int channel){
double  light = analogRead(channel)*3.3/4095;

//light = map(light,0,330,0,100);

// now convert to Fahrenheit
return light;
}


// And now, here's our temp control function
int isIt_TooHot(int TempIn, int tooHot){

if (TempIn > tooHot){
  return 1;

}
  else{
    return 0;
  }


}



// Here's our alarm function, pass in the ON & OFF times, and get a 1 or a 0 depending on if we're between them
//note this is in 24 hour times
//and note for this algorithm we'll assume ON time is in the PM and OFF time in the AM
int isIt_Time(int Hour,int OnTime,int OffTime){

//IF Hour is great than OFFTime...
if (Hour > OffTime){
  //AND IF Hour is less than OnTime
  if (Hour < OnTime){
     //THEN we need to keep alarm OFF
     Particle.publish("Alarm", "Clock Says OFF" + String(0));
     return 0;
  }
    //ELSE it's time to turn the alarm ON
  else {
    Particle.publish("Alarm", "Clock Says ON" + String(1));
    return 1;
    }

  }

}


//And for a general FET ON/OFF control function
void fetControl(int State, int Pin){

  if(State==1){
     digitalWrite(Pin, HIGH);   //This turns the FET ON
   }
     else if(State == 0){
     digitalWrite(Pin, LOW);  //this turnse the FET OFF
   }

}

// Finally, we will write out our ledToggle functions, which is referenced by the Spark.function() called "ToggerN"

int toggler1(String command) {
    if (command=="on") {
        fetControl(1,switch1);
        return 1;
    }
    else if (command=="off") {
        fetControl(0,switch1);
        return 0;
    }
    else {
        return -1;
    }
}

int toggler2(String command) {
    if (command=="on") {
        fetControl(1,switch2);
        return 1;
    }
    else if (command=="off") {
        fetControl(0,switch2);
        return 0;
    }
    else {
        return -1;
    }
}
int toggler3(String command) {
    if (command=="on") {
        fetControl(1,switch3);
        return 1;
    }
    else if (command=="off") {
        fetControl(0,switch3);
        return 0;
    }
    else {
        return -1;
    }
}
int toggler4(String command) {
    if (command=="on") {
        fetControl(1,switch4);
        return 1;
    }
    else if (command=="off") {
        fetControl(0,switch4);
        return 0;
    }
    else {
        return -1;
    }
}

int ledToggle(String command) {

    if (command=="on") {
        digitalWrite(led,HIGH);
        return 1;
    }
    else if (command=="off") {
        digitalWrite(led,LOW);
        return 0;
    }
    else {
        return -1;
    }

}
