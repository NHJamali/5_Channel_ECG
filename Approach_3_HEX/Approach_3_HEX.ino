#include "lead_ii_signal.h" // Include the generated header file

const int dacPin0 = DAC0; // DAC output pin
const int signalSamplingRate = 1000; // Sampling rate in Hz
const int signalDelay = 100000000 / signalSamplingRate; // Microseconds delay
const int totalSamples = sizeof(leadIISignal) / sizeof(leadIISignal[0]);

void setup() {
  // Initialize Serial (optional, can be removed if not needed)
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }

  // Print total samples for verification
  Serial.print("Total samples stored: ");
  Serial.println(totalSamples);
}

void loop() {
  // Produce the signal continuously from the stored data
  for (int i = 0; i < totalSamples; i++) {
    analogWrite(dacPin0, leadIISignal[i]);
    delayMicroseconds(signalDelay);
  }
}