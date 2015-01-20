const char SERIAL_CODE_PLAYER1      =  0x10;
const char SERIAL_CODE_PLAYER2      =  0x20;

const int car1Pin = 5;
const int car2Pin = 6;

typedef unsigned int uint;

char car1State = 0;
char car2State = 0;

char buffer[0x10]; //16 element buffer

// Initialise Arduino on start
void setup() {
  pinMode(car1Pin, OUTPUT);
  pinMode(car2Pin, OUTPUT);

  Serial.begin(9600); //serial port setup (baud rate)
}

void loop() {
  if(Serial.available() > 1) {
    uint numBytes = 0;

    while(Serial.available()) {
        buffer[numBytes++] = Serial.read();
    }

    parseBuffer(numBytes);
  }

  analogWrite(car1Pin, car1State);
  analogWrite(car2Pin, car2State);
}

void parseBuffer(uint bufferSize) {
  if ( bufferSize % 2 != 0) {
      // buffer has bad size so return
      return;
  }

  for (uint i = 0; i!=bufferSize; i++) {
      switch (buffer[i]) {
      case SERIAL_CODE_PLAYER1:
          car1State = 0xFF * (buffer[++i]/100.00);
          break;
      
      case SERIAL_CODE_PLAYER2:
          car2State = 0xFF * (buffer[++i]/100.00);
          break;

      default:
          break;
      }
  }
}