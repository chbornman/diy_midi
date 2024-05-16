/************************* INCLUDES *****************************/
#include <BLEMidi.h>
#include <Wire.h>
#include <Encoder.h>
#include <Bounce2.h>


/************************* CONFIGS *****************************/
#define DEBOUNCE_TIME_MS 5 // Debounce time in milliseconds
#define NUM_ENCODERS 8
#define NUM_CHANNEL_BUTTONS 6

/************************* IO *****************************/

Bounce2::Button encoderButtons[NUM_ENCODERS]; 
Bounce2::Button channelButtons[NUM_CHANNEL_BUTTONS];

int channelButtonPins[NUM_CHANNEL_BUTTONS] = {40, 41, 42, 43, 44, 45}; 
int channelLEDPins[NUM_CHANNEL_BUTTONS] = {25, 26, 27, 28, 29, 30}; 
int encoderButtonPins[NUM_ENCODERS] = {25, 26, 27, 28, 29, 30, 31, 32}; 


// Define encoder pins. Each encoder has 2 pins: {pinA, pinB}
int encoderPins[NUM_ENCODERS][2] = {
  {0, 1}, // Encoder 0
  {2, 3}, // Encoder 1
  {4, 5}, // Encoder 2
  {6, 7}, // Encoder 3
  {8, 9}, // Encoder 4
  {10, 11}, // Encoder 5
  {12, 13}, // Encoder 6
  {14, 15}  // Encoder 7
};

// Encoder and button objects arrays
Encoder encoders[NUM_ENCODERS] = {
  Encoder(encoderPins[0][0], encoderPins[0][1]),
  Encoder(encoderPins[1][0], encoderPins[1][1]),
  Encoder(encoderPins[2][0], encoderPins[2][1]),
  Encoder(encoderPins[3][0], encoderPins[3][1]),
  Encoder(encoderPins[4][0], encoderPins[4][1]),
  Encoder(encoderPins[5][0], encoderPins[5][1]),
  Encoder(encoderPins[6][0], encoderPins[6][1]),
  Encoder(encoderPins[7][0], encoderPins[7][1])
};

long encoderPositions[NUM_ENCODERS] = {-999, -999, -999, -999, -999, -999, -999, -999};


int midi_channel = 1;
int encoderButtonNotes[NUM_ENCODERS] = {1, 2, 3, 4, 5, 6, 7, 8}; 
int encoderCCValue[NUM_ENCODERS] = {1, 2, 3, 4, 5, 6, 7, 8}; 

/************************* SETUP *****************************/

void setup() {
  Serial.begin(115200);
  
  // Initialize BLE MIDI
  BLEMidiServer.begin("DIY_MIDI");

  // Setup I2C
  //TODO Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize encoders and buttons
  initEncoders();
  initButtons();
}

/************************* LOOP *****************************/
void loop() {
  if(BLEMidiServer.isConnected()) {      
    handleEncoders();
    handleEncoderButtons();
    handleChannelButtons();
  }
}



/********************** PRIVATE FUNCTIONS *************************/
void initEncoders() {
  //Nothing needed? TODO remove function
}

void initButtons() {
  // Initialize each encoder button
  for (int i = 0; i < NUM_ENCODERS; i++) {
    pinMode(encoderButtonPins[i], INPUT); // Initialize the button pin as an input
    encoderButtons[i].attach(encoderButtonPins[i], INPUT); // Use external pull-up
    encoderButtons[i].interval(DEBOUNCE_TIME_MS); // Debounce interval in milliseconds
    encoderButtons[i].setPressedState(HIGH); // High state corresponds to physically pressing the button
  }
}

void handleEncoders() {
  // Placeholder for encoder handling logic
  // This should include reading each encoder and sending MIDI control changes if there's movement
  for (int i = 0; i < NUM_ENCODERS; i++) {
    long newPos = encoders[i].read();
    if (newPos != encoderPositions[i]) {
      encoderPositions[i] = newPos;
      BLEMidiServer.controlChange(midi_channel, encoderCCValue[i], newPos);
    }
  }
}

void handleEncoderButtons() {
  for (int i = 0; i < NUM_ENCODERS; i++) {
    // Update the Bounce instance (YOU MUST DO THIS EVERY LOOP)
    encoderButtons[i].update();

    // <Bounce>.changed() RETURNS true IF THE STATE CHANGED (FROM HIGH TO LOW OR LOW TO HIGH)
    if ( encoderButtons[i].changed() ) {
      // THE STATE OF THE INPUT CHANGED
      // GET THE STATE
      int deboucedInput = encoderButtons[i].read();
      // IF THE CHANGED VALUE IS LOW
      if ( deboucedInput == LOW ) {
        BLEMidiServer.noteOn(midi_channel, encoderButtonNotes[i], 127);
      }
      else
      {
        BLEMidiServer.noteOff(midi_channel, encoderButtonNotes[i], 127);
      }
    }
  }
}

void handleChannelButtons() {
  for (int i = 0; i < NUM_CHANNEL_BUTTONS; i++) {
    // Update the Bounce instance (YOU MUST DO THIS EVERY LOOP)
    channelButtons[i].update();

    // <Bounce>.changed() RETURNS true IF THE STATE CHANGED (FROM HIGH TO LOW OR LOW TO HIGH)
    if ( channelButtons[i].changed() ) {
      // THE STATE OF THE INPUT CHANGED
      // GET THE STATE
      int deboucedInput = channelButtons[i].read();
      // IF THE CHANGED VALUE IS LOW
      if ( deboucedInput == LOW ) {
        midi_channel = i;
      }
    }
  }
}
