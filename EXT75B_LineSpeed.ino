//Define Modbus Library
#include <ModbusMaster.h>

//Nodbus Setting
#define MODBUS_DIR_PIN 4
#define MODBUS_RX_PIN 0
#define MODBUS_TX_PIN 1
#define MODBUS_SERIAL_BAUD 9600

uint16_t data_register = 0x03E9;

ModbusMaster node;

//-=-=-=Define Connection Line Speed-=-=-=-=-=-=-=
byte segmentClock_Speed = 24;
byte segmentLatch_Speed = 28;
byte segmentData_Speed = 26;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//-=-=-=Define Connection Length Counter-=-=-=-=-= EXT75B separate line speed and length counter
//byte segmentClock_Counter = 30;
//byte segmentLatch_Counter = 29;
//byte segmentData_Counter = 31;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Define 7-segments
#define a 1<<0
#define b 1<<6
#define c 1<<5
#define d 1<<4
#define e 1<<3
#define f 1<<1
#define g 1<<2

unsigned long speed = 0;
//unsigned long counter = 0;

//Modbus Enable ON
void modbusPreTransmission(){
  delay(50);
  digitalWrite(MODBUS_DIR_PIN, HIGH);
}

//Modbus Enable OFF
void modbusPostTransmission(){
  digitalWrite(MODBUS_DIR_PIN, LOW);
  delay(50);
}

void showNumber_LineSpeed(int value) //show line speed (4 digits) MPM
{
  byte digits[4] = {0};
  byte index_speed = 0;
  int temp_speed = value; //Remove negative signs and any decimals
  
  //Reverse the digits
  for (byte i = 0; i < 4; i++) {
    digits[i] = temp_speed % 10;
    temp_speed /= 10;
  }
  
  // Display digits MSB first
  for (int x = 3; x >= 0; x--) {
    postNumber_LineSpeed(digits[x]);
  }
  
  //Latch the current segment data
  digitalWrite(segmentLatch_Speed, LOW);
  digitalWrite(segmentLatch_Speed, HIGH); //Register moves storage register on the rising edge of RCK
}

void postNumber_LineSpeed(byte speeds)
{
  byte segments_speed;

  //Showing the number based on each segment in 7-segments
  switch (speeds)
  {
    case 1: segments_speed = b | c; break;
    case 2: segments_speed = a | b | d | e | g; break;
    case 3: segments_speed = a | b | c | d | g; break;
    case 4: segments_speed = f | g | b | c; break;
    case 5: segments_speed = a | f | g | c | d; break;
    case 6: segments_speed = a | f | g | e | c | d; break;
    case 7: segments_speed = a | b | c; break;
    case 8: segments_speed = a | b | c | d | e | f | g; break;
    case 9: segments_speed = a | b | c | d | f | g; break;
    case 0: segments_speed = a | b | c | d | e | f; break;
  }

  //Clock these bits out to the drivers
  for (byte j = 0 ; j < 8 ; j++)
  {
    digitalWrite(segmentClock_Speed, LOW);
    digitalWrite(segmentData_Speed, segments_speed & 1 << (7 - j));
    digitalWrite(segmentClock_Speed, HIGH); //Data transfers to the register on the rising edge of SRCK
  }
}

void setup() {
  Serial.begin(MODBUS_SERIAL_BAUD, SERIAL_8N1);
  while (!Serial){
    ; //Waiting for serial
  }
  pinMode(MODBUS_DIR_PIN, OUTPUT);
  digitalWrite(MODBUS_DIR_PIN, LOW);
  node.begin(1, Serial); //Addres Modbus Slave

  node.preTransmission(modbusPreTransmission);
  node.postTransmission(modbusPostTransmission);

  pinMode(segmentClock_Speed, OUTPUT);
  pinMode(segmentData_Speed, OUTPUT);
  pinMode(segmentLatch_Speed, OUTPUT);

  digitalWrite(segmentClock_Speed, LOW);
  digitalWrite(segmentData_Speed, LOW);
  digitalWrite(segmentLatch_Speed, LOW);
}

void loop() {
  uint8_t result;
  uint16_t data[2];
  result = node.readInputRegisters(0x03E9, 1);
  if (result == node.ku8MBSuccess){
    data[0] = node.getResponseBuffer(0x00);
    data[1] = node.getResponseBuffer(0x01);

    Serial.println(data[0]);
    speed = data[0];
    
    showNumber_LineSpeed(speed);
  } 
  else {
    Serial.print("Failed, Response Code: ");
    Serial.println("");
    delay(1000);
  }
  delay(50);
}
