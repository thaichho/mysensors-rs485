// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable RS485 transport layer
#define MY_RS485

// Static Node ID
// muss gesetzt werden damit Node gefunden wird durch Gateway und muss im Netzwerk eindeutig sein!
#define MY_NODE_ID 89

// Define this to enables DE-pin management on defined pin
#define MY_RS485_DE_PIN 2

// Set RS485 baud rate to use
#define MY_RS485_BAUD_RATE 9600

#include <MySensors.h>

#define CHILD_ID 1          // Id of the sensor child
#define RELAY_PIN 4         // Arduino Digital I/O pin number for first relay (second on pin+1 etc) erster Pin D4
#define NUMBER_OF_RELAYS 2  // Total number of attached relays
#define RELAY_ON 1          // GPIO value to write to turn on attached relay
#define RELAY_OFF 0         // GPIO value to write to turn off attached relay
bool ack = 1;               // set this to 1 if you want destination node to send ack back to this node

bool initialValueSent = false;
MyMessage msg[NUMBER_OF_RELAYS];  // Um initiale Messages zu schicken

void before() {
  for (int sensor = 1, pin = RELAY_PIN; sensor <= NUMBER_OF_RELAYS; sensor++, pin++) {
    // Then set relay pins in output mode
    pinMode(pin, OUTPUT);
    // Set relay to last known state (using eeprom storage)
    digitalWrite(pin, loadState(sensor) ? RELAY_ON : RELAY_OFF);
    // initialize messages
    msg[sensor - 1].sensor = sensor;
    msg[sensor - 1].type = V_LIGHT;
  }
}

void setup() {
}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Double Relay RS485", "1.0");

  for (int sensor = 1, pin = RELAY_PIN; sensor <= NUMBER_OF_RELAYS; sensor++, pin++) {
    // Register all sensors to gw (they will be created as child devices)
    present(sensor, S_LIGHT, "Light", ack);
  }
}


void loop() {
  if (!initialValueSent) {
    Serial.println("Sending initial value");
    for (int sensor = 1, pin = RELAY_PIN; sensor <= NUMBER_OF_RELAYS; sensor++, pin++) {
      send(msg[sensor - 1].set(loadState(sensor) ? RELAY_ON : RELAY_OFF));
    }
    initialValueSent = true;
    Serial.println("Sending initial value: Completed");
  }
}

void receive(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.getType() == V_STATUS) {
    // Change relay state
    digitalWrite(message.getSensor() - 1 + RELAY_PIN, message.getBool() ? RELAY_ON : RELAY_OFF);
    // Store state in eeprom
    saveState(message.getSensor(), message.getBool());
    // Write some debug info
    Serial.print("Incoming change for sensor:");
    Serial.print(message.getSensor());
    Serial.print(", New status: ");
    Serial.println(message.getBool());
  }
}