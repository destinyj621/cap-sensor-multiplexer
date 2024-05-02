#include <ADCTouch.h> // Library for capacitive touch sensing
#include <SoftwareSerial.h> // Library for software serial communication
#include <DFPlayerMini_Fast.h> // Library for controlling an audio player module

// Define pins for Multiplexer 1
int Mux1S0 = 5;
int Mux1S1 = 6;
int Mux1S2 = 7;
int pin_In_Mux1 = A1; // Input pin for Multiplexer 1
int Mux1_State[8] = {0}; // Array to store states of Multiplexer 1
int Mux1_Ref[8] = {0}; // Array to store reference values for Multiplexer 1

// Define pins for Multiplexer 2
int Mux2S0 = 8;
int Mux2S1 = 9;
int Mux2S2 = 10;
int pin_In_Mux2 = A2; // Input pin for Multiplexer 2
int Mux2_State[8] = {0}; // Array to store states of Multiplexer 2
int Mux2_Ref[8] = {0}; // Array to store reference values for Multiplexer 2

// Define pins for Multiplexer 3
int Mux3S0 = 11;
int Mux3S1 = 12;
int Mux3S2 = 13;
int pin_In_Mux3 = A3; // Input pin for Multiplexer 3
int Mux3_State[8] = {0}; // Array to store states of Multiplexer 3
int Mux3_Ref[8] = {0}; // Array to store reference values for Multiplexer 3

// Threshold and volume settings
int th = 20; // Threshold for triggering audio playback
int volume = 30; // Audio volume

// Define the number of Mux states
const int NUM_MUX_STATES = 3;
// Define the number of tracks per Mux state
const int TRACKS_PER_STATE = 8;

// Software Serial communication setup
SoftwareSerial mySerial(4, 2); // RX, TX
DFPlayerMini_Fast myMP3; // Object for controlling the audio player module

enum State {
  WAITING, // State indicating that the system is waiting
  PLAYING // State indicating that audio is currently playing
};

State currentState = WAITING; // Initialize the current state as waiting
bool audioPlaying = false; // Variable to track whether audio is currently playing or not

// Setup function
void setup() {

  // Initialize serial communication
  Serial.begin(9600); // Initialize main serial port for debugging
  mySerial.begin(9600); // Initialize software serial communication for the MP3 module
  Serial.println("Setup started."); // Debug message to confirm setup execution

  myMP3.begin(mySerial); // Initialize the MP3 module
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
  playAudio();
  
  // Delay for stability
  delay(100);
}

// Function to read reference values for all Multiplexers
void readRef() {
  for (int i = 0; i < 8; i++){

    // Set Multiplexer 1 channel
    digitalWrite(Mux1S0, i & B00000001);
    digitalWrite(Mux1S1, i & B00000010);
    digitalWrite(Mux1S2, i & B00000100);
    Mux1_Ref[i] = ADCTouch.read(pin_In_Mux1, 500);

    // Set Multiplexer 2 channel
    digitalWrite(Mux2S0, i & B00000001);
    digitalWrite(Mux2S1, i & B00000010);
    digitalWrite(Mux2S2, i & B00000100);
    Mux2_Ref[i] = ADCTouch.read(pin_In_Mux2, 500);

    // Set Multiplexer 3 channel
    digitalWrite(Mux3S0, i & B00000001);
    digitalWrite(Mux3S1, i & B00000010);
    digitalWrite(Mux3S2, i & B00000100);
    Mux3_Ref[i] = ADCTouch.read(pin_In_Mux3, 500);    
  }
}

// Function to update Multiplexer readings
void updateMux() {
  for (int i = 0; i < 8; i++){

    // Set Multiplexer 1 channel
    digitalWrite(Mux1S0, i & B00000001);
    digitalWrite(Mux1S1, i & B00000010);
    digitalWrite(Mux1S2, i & B00000100);
    Mux1_State[i] = ADCTouch.read(pin_In_Mux1, 500);

    // Set Multiplexer 2 channel
    digitalWrite(Mux2S0, i & B00000001);
    digitalWrite(Mux2S1, i & B00000010);
    digitalWrite(Mux2S2, i & B00000100);
    Mux2_State[i] = ADCTouch.read(pin_In_Mux2, 500);

    // Set Multiplexer 3 channel
    digitalWrite(Mux3S0, i & B00000001);
    digitalWrite(Mux3S1, i & B00000010);
    digitalWrite(Mux3S2, i & B00000100);
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
              // Update state
              currentState = PLAYING;
              audioPlaying = true;
              // Exit function
              return;
          }
      }
  }

  // If no audio is playing, update state
  if (audioPlaying) {
    currentState = WAITING;
    audioPlaying = false;
  }
}
