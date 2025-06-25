//Define Modbus Library
#include<ModbusRtu.h>      
Modbus bus; 
uint16_t modbus_array[] = {0,0,0,0};  

//-=-=-=Define Connection Line Speed-=-=-=-=-=-=-=
byte segmentClock_Speed = 24;
byte segmentLatch_Speed = 28;
byte segmentData_Speed = 26;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//-=-=-=Define Connection Length Counter-=-=-=-=-=
byte segmentClock_Counter = 30;
byte segmentLatch_Counter = 29;
byte segmentData_Counter = 31;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Define 7-segments
#define a  1<<0
#define b  1<<6
#define c  1<<5
#define d  1<<4
#define e  1<<3
#define f  1<<1
#define g  1<<2
#define dp 1<<7

unsigned long speed = 0;
unsigned long counter = 0;

void setup()
{
  bus = Modbus (1, Serial, 4); //Modbus Slave ID 1, RS485, and Pin Enable at DI4
  bus.begin(38400); //Modbus Slave Baudrate at 38400
  bus.start();

  pinMode(segmentClock_Speed, OUTPUT);
  pinMode(segmentData_Speed, OUTPUT);
  pinMode(segmentLatch_Speed, OUTPUT);

  digitalWrite(segmentClock_Speed, LOW);
  digitalWrite(segmentData_Speed, LOW);
  digitalWrite(segmentLatch_Speed, LOW);

  pinMode(segmentClock_Counter, OUTPUT);
  pinMode(segmentData_Counter, OUTPUT);
  pinMode(segmentLatch_Counter, OUTPUT);

  digitalWrite(segmentClock_Counter, LOW);
  digitalWrite(segmentData_Counter, LOW);
  digitalWrite(segmentLatch_Counter, LOW);
}

void loop()
{
  bus.poll(modbus_array,sizeof(modbus_array)/sizeof(modbus_array[0]));
  speed = modbus_array[2]; //Data address 2
  counter = modbus_array[0]; //Data address 0
  //speed = 123;
  //counter = 12345;
  showNumber_LineSpeed(speed); //current only test, later replace with parameter speed
  showNumber_LengthCounter(counter); //current only test, later replace with parameter speed
  delay(50);
}

//Takes a number and displays 2 numbers. Displays absolute value (no negatives)
void showNumber_LineSpeed(int value) //show line speed (3 digits) MPM
{
  byte digits[3] = {0};
  byte index_speed = 0;
  int temp_speed = value; //Remove negative signs and any decimals
  //Reverse the digits
  for (byte i = 0; i < 3; i++) {
  digits[i] = temp_speed % 10;
  temp_speed /= 10;
  }
  // Display digits MSB first
  for (int x = 2; x >= 0; x--) {
    postNumber_LineSpeed(digits[x]);
  }
  //Latch the current segment data
  digitalWrite(segmentLatch_Speed, LOW);
  digitalWrite(segmentLatch_Speed, HIGH); //Register moves storage register on the rising edge of RCK
}

void showNumber_LengthCounter(uint32_t value) //show length counter (5 digits) Meter
{
  byte digits[5] = {0};
  byte index_count = 0;
  uint32_t temp = abs(value); //Remove negative signs and any decimals

  //Reverse the digits
  //Reverse the digits
  for (byte i = 0; i < 5; i++) {
  digits[i] = temp % 10;
  temp /= 10;
  }
  // Display digits MSB first
  for (int x = 4; x >= 0; x--) {
    postNumber_LengthCounter(digits[x]);
  }
  //Latch the current segment data
  digitalWrite(segmentLatch_Counter, LOW);
  digitalWrite(segmentLatch_Counter, HIGH); //Register moves storage register on the rising edge of RCK
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

void postNumber_LengthCounter(byte counters)
{

  byte segments_counter;

  //Showing the number based on each segment in 7-segments
  switch (counters)
  {
    case 1: segments_counter = b | c; break;
    case 2: segments_counter = a | b | d | e | g; break;
    case 3: segments_counter = a | b | c | d | g; break;
    case 4: segments_counter = f | g | b | c; break;
    case 5: segments_counter = a | f | g | c | d; break;
    case 6: segments_counter = a | f | g | e | c | d; break;
    case 7: segments_counter = a | b | c; break;
    case 8: segments_counter = a | b | c | d | e | f | g; break;
    case 9: segments_counter = a | b | c | d | f | g; break;
    case 0: segments_counter = a | b | c | d | e | f; break;
  }

  //Clock these bits out to the drivers
  for (byte k = 0 ; k < 8 ; k++)
  {
    digitalWrite(segmentClock_Counter, LOW);
    digitalWrite(segmentData_Counter, segments_counter & 1 << (7 - k));
    digitalWrite(segmentClock_Counter, HIGH); //Data transfers to the register on the rising edge of SRCK
  }
}