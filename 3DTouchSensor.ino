#include <ADCTouch.h>
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>

// Define pins for Multiplexer 1
int Mux1S0 = 5;
int Mux1S1 = 6;
int Mux1S2 = 7;w
int pin_In_Mux1 = A1;
int Mux1_State[8] = {0};
int Mux1_Ref[8] = {0};

// Define pins for Multiplexer 2
int Mux2S0 = 8;
int Mux2S1 = 9;
int Mux2S2 = 10;
int pin_In_Mux2 = A2;
int Mux2_State[8] = {0};
int Mux2_Ref[8] = {0};

// Define pins for Multiplexer 3
int Mux3S0 = 11;
int Mux3S1 = 12;
int Mux3S2 = 13;
int pin_In_Mux3 = A3;
int Mux3_State[8] = {0};
int Mux3_Ref[8] = {0};

// Threshold and volume settings
int th = 15;
int volume = 5;

// Define the number of Mux states
const int NUM_MUX_STATES = 3;
// Define the number of tracks per Mux state
const int TRACKS_PER_STATE = 8;

// Software Serial communication setup
SoftwareSerial mySerial(4, 2);  // RX, TX
DFPlayerMini_Fast myMP3;

enum State {
  WAITING,
  PLAYING
};

State currentState = WAITING;


// Setup function
void setup() {

  // Initialize serial communication
  Serial.begin(9600); // Initialize main serial port for debugging
  mySerial.begin(9600);
  myMP3.begin(mySerial);
  myMP3.volume(volume); // Set volume

  // Set pins for Multiplexer 1 as outputs
  pinMode(Mux1S0, OUTPUT);
  pinMode(Mux1S1, OUTPUT);
  pinMode(Mux1S2, OUTPUT);

  // Set pins for Multiplexer 2 as outputs
  pinMode(Mux2S0, OUTPUT);
  pinMode(Mux2S1, OUTPUT);
  pinMode(Mux2S2, OUTPUT);

  // Set pins for Multiplexer 3 as outputs
  pinMode(Mux3S0, OUTPUT);
  pinMode(Mux3S1, OUTPUT);
  pinMode(Mux3S2, OUTPUT);

  // Read reference values for all Multiplexers
  readRef();
}

// Main loop function
void loop() {
  // Update Multiplexer readings
  updateMux();
  
  // Calculate capacitance values
  capVal();
  
  // Print out Multiplexer 1 state
  for(int i = 0; i < 8; i ++) {
    if(i == 7) {
      Serial.println(Mux1_State[i]);
    } else {
      Serial.print(Mux1_State[i]);
      Serial.print(",");
    }
  }

  // Print out Multiplexer 2 state
  for(int i = 0; i < 8; i ++) {
    if(i == 7) {
      Serial.println(Mux2_State[i]);
    } else {
      Serial.print(Mux2_State[i]);
      Serial.print(",");
    }
  }

  // Print out Multiplexer 3 state
  for(int i = 0; i < 8; i ++) {
    if(i == 7) {
      Serial.println(Mux3_State[i]);
    } else {
      Serial.print(Mux3_State[i]);
      Serial.print(",");
    }
  }

  Serial.println("-----------------------");
  
  // Play audio based on Multiplexer 2 state
  //playAudio();
  myMP3.play(1);
  
  // Delay for stability
  delay(100);
}

// Function to read reference values for all Multiplexers
void readRef() {
  for (int i = 0; i < 8; i++){

    // Set Multiplexer 1 channel
    digitalWrite(Mux1S0, HIGH && (i & B00000001));
    digitalWrite(Mux1S1, HIGH && (i & B00000010));
    digitalWrite(Mux1S2, HIGH && (i & B00000100));
    Mux1_Ref[i] = ADCTouch.read(pin_In_Mux1, 500);

    // Set Multiplexer 2 channel
    digitalWrite(Mux2S0, HIGH && (i & B00000001));
    digitalWrite(Mux2S1, HIGH && (i & B00000010));
    digitalWrite(Mux2S2, HIGH && (i & B00000100));
    Mux2_Ref[i] = ADCTouch.read(pin_In_Mux2, 500);

    // Set Multiplexer 3 channel
    digitalWrite(Mux3S0, HIGH && (i & B00000001));
    digitalWrite(Mux3S1, HIGH && (i & B00000010));
    digitalWrite(Mux3S2, HIGH && (i & B00000100));
    Mux3_Ref[i] = ADCTouch.read(pin_In_Mux3, 500);    
  }
}

// Function to update Multiplexer readings
void updateMux() {
  for (int i = 0; i < 8; i++){

    // Set Multiplexer 1 channel
    digitalWrite(Mux1S0, HIGH && (i & B00000001));
    digitalWrite(Mux1S1, HIGH && (i & B00000010));
    digitalWrite(Mux1S2, HIGH && (i & B00000100));
    Mux1_State[i] = ADCTouch.read(pin_In_Mux1, 500);

    // Set Multiplexer 2 channel
    digitalWrite(Mux2S0, HIGH && (i & B00000001));
    digitalWrite(Mux2S1, HIGH && (i & B00000010));
    digitalWrite(Mux2S2, HIGH && (i & B00000100));
    Mux2_State[i] = ADCTouch.read(pin_In_Mux2, 500);

    // Set Multiplexer 3 channel
    digitalWrite(Mux3S0, HIGH && (i & B00000001));
    digitalWrite(Mux3S1, HIGH && (i & B00000010));
    digitalWrite(Mux3S2, HIGH && (i & B00000100));
    Mux3_State[i] = ADCTouch.read(pin_In_Mux3, 500);    
  }
}

// Function to calculate capacitance values
void capVal(){
  for (int i = 0; i < 8; i++) {
    // Calculate difference between current state and reference
    Mux1_State[i] -= Mux1_Ref[i];
    Mux2_State[i] -= Mux2_Ref[i];
    Mux3_State[i] -= Mux3_Ref[i];
  }
}

// Function to play audio based on multiplexer states
void playAudio(){
/*
switch (currentState) {
    case WAITING:
      if (abs(Mux1_State[4]) > th) {
        currentState = PLAYING;
        myMP3.play(1); // Play MP3 file 1 on MP3
      } else if (abs(Mux1_State[6]) > th) {
        currentState = PLAYING;
        myMP3.play(2); // Play MP3 file 2 on MP3
      }
      break;
      

    case PLAYING:
      if (abs(Mux1_State[6]) <= th && abs(Mux1_State[4]) <= th) {
        currentState = WAITING;
        // Stop playing MP3 file when all sensor readings drop below the threshold
        myMP3.stop();
      }
      break;
  }
  */

  // Define arrays to hold Mux states and corresponding track numbers
  int Mux_States[NUM_MUX_STATES][TRACKS_PER_STATE] = {
      {Mux1_State[0], Mux1_State[1], Mux1_State[2], Mux1_State[3], Mux1_State[4], Mux1_State[5], Mux1_State[6], Mux1_State[7]},
      {Mux2_State[0], Mux2_State[1], Mux2_State[2], Mux2_State[3], Mux2_State[4], Mux2_State[5], Mux2_State[6], Mux2_State[7]},
      {Mux3_State[0], Mux3_State[1], Mux3_State[2], Mux3_State[3], Mux3_State[4], Mux3_State[5], Mux3_State[6], Mux3_State[7]}
  };

  // Iterate through each Mux state
  for (int i = 0; i < NUM_MUX_STATES; ++i) {
      // Iterate through each track in the current Mux state
      for (int j = 0; j < TRACKS_PER_STATE; ++j) {
          // Check if the absolute value of the current Mux state is greater than or equal to the threshold
          if (abs(Mux_States[i][j]) >= th) {
              // Calculate the track number based on the current Mux state and track index
              int trackNumber = i * TRACKS_PER_STATE + j + 1;
              // Play the corresponding track
              myMP3.play(trackNumber);
              // Break out of the loop to play only one track per Mux state
              break;
          }
      }
  }

/*
  if (abs(Mux1_State[0]) >= th)
      myMP3.play(1); // Play track 1

  else if (abs(Mux1_State[1]) >= th)
      myMP3.play(2); // Play track 2

  else if (abs(Mux1_State[2]) >= th)
      myMP3.play(3); // Play track 3

  else if (abs(Mux1_State[3]) >= th)
      myMP3.play(4); // Play track 4

  else if (abs(Mux1_State[4]) >= th)
      myMP3.play(5); // Play track 5

  else if (abs(Mux1_State[5]) >= th)
      myMP3.play(6); // Play track 6

  else if (abs(Mux1_State[6]) >= th)
      myMP3.play(7); // Play track 7

  else if (abs(Mux1_State[7]) >= th)
      myMP3.play(8); // Play track 8

  else if (abs(Mux2_State[0]) >= th)
      myMP3.play(9); // Play track 9

  else if (abs(Mux2_State[1]) >= th)
      myMP3.play(10); // Play track 10

  else if (abs(Mux2_State[2]) >= th)
      myMP3.play(11); // Play track 11

  else if (abs(Mux2_State[3]) >= th)
      myMP3.play(12); // Play track 12

  else if (abs(Mux2_State[4]) >= th)
      myMP3.play(13); // Play track 13

  else if (abs(Mux2_State[5]) >= th)
      myMP3.play(14); // Play track 14

  else if (abs(Mux2_State[6]) >= th)
      myMP3.play(15); // Play track 15

  else if (abs(Mux2_State[7]) >= th)
      myMP3.play(16); // Play track 16

  else if (abs(Mux3_State[0]) >= th)
      myMP3.play(17); // Play track 17

  else if (abs(Mux3_State[1]) >= th)
      myMP3.play(18); // Play track 18

  else if (abs(Mux3_State[2]) >= th)
      myMP3.play(19); // Play track 19

  else if (abs(Mux3_State[3]) >= th)
      myMP3.play(20); // Play track 20

  else if (abs(Mux3_State[4]) >= th)
      myMP3.play(21); // Play track 21

  else if (abs(Mux3_State[5]) >= th)
      myMP3.play(22); // Play track 22

  else if (abs(Mux3_State[6]) >= th)
      myMP3.play(23); // Play track 23

  else if (abs(Mux3_State[7]) >= th)
      myMP3.play(24); // Play track 24
*/
}
