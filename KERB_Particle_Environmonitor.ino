//KERBSPACE IOT SYSTEM CONTROLLER
//Written by Leif Summerfield - Fall 2016
//Rev 1   Oct 13  - First psudo code of the whole shebang
//                  and working well : temp sensors, humidity, logging to terminal and Thingspeak.
//Rev 2   April 2017  - Rework for Grove breakout board and sensors
//                    - Meaning we have fixed bits connected now like this

//Async pulse in support
#include "Particle.h"
//KERB specific definitions and variables
#include "KERB_IOT_DEFS.h"


#include "AsyncPulseIn.h"
// Initialize the AsyncPulseIn object to listen on D2 and measure the width of the HIGH part of the pulse
AsyncPulseIn dustPulse(D2, LOW);
int dustWidth = 0;
AsyncPulseIn soundPulse(A1, HIGH);


unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 500;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;





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


  //Adding IBM Watson IOT solution support
  /*
    *****************************************************************************************
    **** Visit https://www.thingspeak.com
    ****  https://h9vymq.internetofthings.ibmcloud.com/dashboard/
    *****************************************************************************************/
    char *IOT_CLIENT = "d:h9vymq:KERB_EnvMnt_particle:BOZ_002";
    char *IOT_HOST = "h9vymq.messaging.internetofthings.ibmcloud.com";
    char *IOT_PASSWORD = "823jjajio2300";
    char *IOT_PUBLISH_1 = "iot-2/evt/Sound/fmt/json";
    char *IOT_PUBLISH_2 = "iot-2/evt/Temperature/fmt/json";
    char *IOT_PUBLISH_3 = "iot-2/evt/Dust/fmt/json";
    char *IOT_PUBLISH_4 = "iot-2/evt/Humidity/fmt/json";
    char *IOT_PUBLISH_5 = "iot-2/evt/Fumes/fmt/json";

    char *IOT_USERNAME = "use-token-auth";
    char *IOT_SUBSCRIBE = "iot-2/cmd/lightUp/fmt/json";
    #include "MQTT.h"
    MQTT WatsonClient( IOT_HOST, 1883, callback );



//Adding Running average support section here
#include "RunningAverage.h"
RunningAverage gasRA(5);
RunningAverage particlesRA(5);
RunningAverage soundRA(2);
RunningAverage humidityRA(10);




//Setup for humidity sensors
#include "SHT31.h"
//variables on the ..DEFS.h page
SHT31 sht31 = SHT31();

//Setup for gas quality sensor on A4
int anGas;


void setup() {
    Serial.begin(9600);
    Serial.println("I'm Allllive !!!!!!");
// First, declare all of our pins. This lets our device know which ones will be used for outputting voltage, and which ones will read incoming voltage.
    pinMode(anNoise, INPUT);
    pinMode(gasSensor, INPUT);
    pinMode(D7, OUTPUT);


// SHT31 init
    sht31.begin();

    // We are going to declare a Spark.variable() here so that we can access the value of the photoresistor from the cloud.
  //  Particle.variable("Noise", &soundAveraged, INT);
  //  Particle.variable("Dust", &int(partclsAveraged), INT);


    //  ThingSpeak setup;
      ThingSpeak.begin(client);

    //  IBM Watson setup
    // Connect to IBM Watsion IOT Platform
    WatsonClient.connect( IOT_CLIENT, IOT_USERNAME, IOT_PASSWORD );

    // Subscribe to events if connected. Use LED for status.
    if( WatsonClient.isConnected() ) {
    Serial.println("connected");
    WatsonClient.subscribe( IOT_SUBSCRIBE );
    }


    // Grove Dust sensor on D2 needs async setup and pullup
    pinMode(D2, INPUT);
    // Call this to clear any previously saved data
    dustPulse.clear();
    soundPulse.clear();
    // We'll use the millis() function to set our start timer
    starttime = millis();

    // Running Averages init [first value is of what, 2nd is how many]
      gasRA.fillValue(600,5);
      particlesRA.clear();
      soundRA.clear();
      humidityRA.clear();
}


// Next is the loop function...
  /*****************************************************************************************/
void loop() {

digitalWrite(D7,HIGH);
//SECTION 1  - Collect data           ==========================================

//      sum = analogRead(anNoise);
//      soundTrigger = soundTrigger + digitalRead(A1);

  //    sound_Width = pulseIn(A1, HIGH);
    //  sound_Duration = sound_Duration+sound_Width;


    AsyncPulseInPulseInfo soundPulseInfo;
    if(soundPulse.getNextPulse(soundPulseInfo)) {
      //We have a pulse!
      sound_Width = soundPulseInfo.widthMicros;
      sound_Duration=sound_Duration+sound_Width;  //total them up

    }

      //Dust sensor pulse widths
        // Unlike pulseIn(), getNextPulse() is non-blocking. It returns the most recent pulses measured
        // and stored in a FIFO (first-in, first-out) buffer. If you have a sensor that you need to trigger,
        // you'd typically call asyncPulse.clear(), trigger the sensor, then you could either wait until
        // getNextPulse() returns true, or return from loop and handle it later.

        AsyncPulseInPulseInfo pulseInfo;
      	if(dustPulse.getNextPulse(pulseInfo)) {
          //We have a pulse!
          dustWidth = pulseInfo.widthMicros;
          lowpulseoccupancy = lowpulseoccupancy+dustWidth;
      	}

    //  dustPulse.clear();


//SECTION 2  - Once a second we'll step in here, reduce datea and throw the data to the cloud         ==========================================
      if ((millis()-starttime) > sampletime_ms)
      {
      digitalWrite(D7,LOW);

      // Do some calculations, run the numbers,
      ratio = lowpulseoccupancy/(sampletime_ms*1000.0);  // Integer percentage 0=>100
      concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
      particlesRA.addValue(concentration);
      partclsAveraged = particlesRA.getAverage();

      //Sound signal averages
      soundRA.addValue(sound_Duration);   // Integer percentage 0=>100 (PulseIn is in microsecs)
      soundAveraged = soundRA.getAverage();

      //Grab the non time critical bits to send out
      //Humidity and Temp sening
      temp = sht31.getTemperature();
      temp = temp*9/5 + 32; //convert to F
      humidityRA.addValue(sht31.getHumidity());
      humidity = humidityRA.getAverage();

      //Gas quality sensor is just an analog voltage divider, so...
      // Read the sensor, convert to concentration, and run an average, then we'll post it
      gasRA.addValue(analogRead(gasSensor));
      badGasContrn= badGasConversion*gasRA.getAverage() + badGasOffset;


        // handle disconnects
        if( !WatsonClient.isConnected() )
          {
          Serial.println("Reconnecting!");
          WatsonClient.connect( IOT_CLIENT, IOT_USERNAME, IOT_PASSWORD );
          WatsonClient.subscribe( IOT_SUBSCRIBE );
          }

        WatsonClient.publish(IOT_PUBLISH_1, "{\"Sound\":"+ String(soundAveraged)+"}");
        WatsonClient.publish(IOT_PUBLISH_2, "{\"Temperature\":"+ String(temp)+"}");
        WatsonClient.publish(IOT_PUBLISH_3, "{\"Dust\":"+ String(partclsAveraged)+"}");
        WatsonClient.publish(IOT_PUBLISH_4, "{\"Humidity\":"+ String(humidity)+"}");
        WatsonClient.publish(IOT_PUBLISH_5, "{\"Fumes\":"+ String(badGasContrn)+"}");

        /*ThingSpeak.setField(1,soundAveraged);
        ThingSpeak.setField(2,temp);
        ThingSpeak.setField(3,humidity);
        ThingSpeak.setField(4,partclsAveraged);
        ThingSpeak.setField(5,anGas);
        ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);*/

        //Print this to Paricle
        Particle.publish("Noise",String(soundAveraged));
        Particle.publish("Dust",String(partclsAveraged));
        Particle.publish("Temp",String(temp));
        Particle.publish("Humidity",String(humidity));
        Particle.publish("Fumes",String(badGasContrn));

        //and telemetry out to Serial
        String telem = "";
        telem += String(sound_Duration) + ",";             // Plot 2: A1
        telem += String(soundAveraged);
        telem += "\r\n"; // Add a new line and we are DONE
        Serial.print(telem);

//Reset variables before going back out there to real-time land
        lowpulseoccupancy = 0;  //Reset our bin for adding bit low times
        starttime = millis();   //reset our
        sound_Duration = 0;     // reset the noisy counter bin


      }// end timer section in main loop that happens once a sec

}   // end main loop
// -----------------------------------------------------end main loop -----------------------------
//-------------------------------------------------------------------------------------------------



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


// handle commands from the Watson IOT Platform
// light up one LED, color based on the parameter
// loop around from 1 to 11, so it shows a history of the values
int s; // used to decode the incoming character, declare outside the function to keep it off the stack
int num = 1; // tracks which LED is lit, valid values 1 to 11 (unless heartbeat is on - then change to 1 to 10)
void callback( char* topic, byte* payload, unsigned int length ) {

 // Wrap around LED indexer. Use 1 and 12 unless heartbeat is on then use 1 to 11
 if (num > 11)
 num = 1;

 // convert incoming character to integer
 s = payload[0]-'0';

 // decode number to color
 switch (s) {
 case 1:

 break;

 case 2:

 break;
 case 3:

 break;
 default:

 break;
 }
 num++;
}


/*Saving for later   Here's the example sketch to convert sensor down time percentage into dust concentration
It just a linear fit of % down time as X, concentration as Y
void loop() {
  duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;

  if ((millis()-starttime) > sampletime_ms)
  {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
    Serial.print(lowpulseoccupancy);
    Serial.print(",");
    Serial.print(ratio);
    Serial.print(",");
    Serial.println(concentration);
    lowpulseoccupancy = 0;
    starttime = millis();
  }
}*/
