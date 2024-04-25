// Code for Gate Control (DRAFT VERSION)
#define SN "Gate Control RS485"
#define SV "1.0"

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

// Internal representation of the cover state.
enum State {
  IDLE,
  UP, // Window covering. Up.
  DOWN, // Window covering. Down.
};

static int state = IDLE;
static int status = 0; // 0=cover is down, 1=cover is up

bool initialValueSent = false;
MyMessage upMessage(CHILD_ID, V_UP);
MyMessage downMessage(CHILD_ID, V_DOWN);
MyMessage stopMessage(CHILD_ID, V_STOP);
MyMessage statusMessage(CHILD_ID, V_STATUS);

void sendState() {
  // Send current state and status to gateway.
  send(upMessage.set(state == UP));
  send(downMessage.set(state == DOWN));
  send(stopMessage.set(state == IDLE));
  send(statusMessage.set(status));
}

void before() {
  for (int sensor = 1, pin = RELAY_PIN; sensor <= NUMBER_OF_RELAYS; sensor++, pin++) {
    // Then set relay pins in output mode
    pinMode(pin, OUTPUT);
    // set relays to off
    digitalWrite(pin, RELAY_OFF);
  }
}

void setup() {
}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SN, SV);
  present(CHILD_ID, S_COVER);
}


void loop() {
  if (!initialValueSent) {
    sendState();
    initialValueSent = true;
  }
  if (!initialValueSent) {
    Serial.println("Sending initial value");
//    for (int sensor = 1, pin = RELAY_PIN; sensor <= NUMBER_OF_RELAYS; sensor++, pin++) {
//      send(msg[sensor - 1].set(loadState(sensor) ? RELAY_ON : RELAY_OFF));
//    }
    initialValueSent = true;
    Serial.println("Sending initial value: Completed");
  }
}

void receive(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type == V_UP) {
    // open gate
    Serial.print("start open gate\n");
    digitalWrite(RELAY_PIN + 1, RELAY_OFF);
    digitalWrite(RELAY_PIN + 0, RELAY_ON);
    // wait time
    sleep(200);
    digitalWrite(RELAY_PIN + 1, RELAY_ON);
    Serial.print("gate opened\n");
  }

  if (message.type == V_DOWN) {
    // close gate
    // open gate
    Serial.print("start open gate before closing\n");
    digitalWrite(RELAY_PIN + 1, RELAY_OFF);
    digitalWrite(RELAY_PIN + 0, RELAY_ON);
    // wait time
    sleep(200);
    digitalWrite(RELAY_PIN + 1, RELAY_ON);

    // general wait
    Serial.print("Wait before closing\n");
    sleep(30000);

    // close gate
    Serial.print("start closing gate\n");
    digitalWrite(RELAY_PIN + 1, RELAY_ON);
    digitalWrite(RELAY_PIN + 0, RELAY_ON);
    // wait time
    sleep(200);
    digitalWrite(RELAY_PIN + 1, RELAY_ON);
  }

}