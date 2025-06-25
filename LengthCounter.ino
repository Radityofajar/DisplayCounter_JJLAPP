#include<ModbusRtu.h>      
Modbus bus; 
uint16_t modbus_array[] = {0,0,0,0};  
#define LATCH 30
#define CLK 31
#define DATA 29
#define LATCH_1 24
#define CLK_1 26
#define DATA_1 28
byte digit[10]= {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};
unsigned long mpm=0;
unsigned long meter=0;
long a, b, c, d, e;
// a = digit 1 7-segment, b = digit 2 7-segment, ... e = digit 5 7-segment
long i, j, k, l, m;
// i = digit 1 baris 2 7-segment

void setup() 
{
  pinMode(LATCH, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(LATCH_1, OUTPUT);
  pinMode(CLK_1, OUTPUT);
  pinMode(DATA_1, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  bus = Modbus (1,Serial,4);            //Modbus slave ID as 1 and 1 connected via RS-485 and 4 connected to DE & RE pin of RS-485 Module 
  bus.begin(38400);                //Modbus slave baudrate at 9600
  bus.start();
}

void loop()
{
  bus.poll(modbus_array,sizeof(modbus_array)/sizeof(modbus_array[0]));       //Used to receive or write value from Master
  mpm = modbus_array[2];
  meter = modbus_array[0];
  delay(250);

  //MPM
  e = mpm%10; // units
  d = mpm%100; // tens
  c = mpm%1000; // hundreds
  b = mpm%10000; // thousands

  // Isolate to only one digit
  b -= c;
  c -= d;
  d -= e;
  d /= 10;
  c /= 100; 
  b /= 1000;
  a = mpm/10000;

  // Display to 7-Segments
        digitalWrite(LATCH_1, LOW);
        shiftOut(DATA_1, CLK_1, MSBFIRST, ~digit[a]); //Ten thousands
        shiftOut(DATA_1, CLK_1, MSBFIRST, ~digit[b]); //Thousands
        shiftOut(DATA_1, CLK_1, MSBFIRST, ~digit[c]); //Hundreds
        shiftOut(DATA_1, CLK_1, MSBFIRST, ~digit[d]); //Tens
        shiftOut(DATA_1, CLK_1, MSBFIRST, ~digit[e]); //Units
        digitalWrite(LATCH_1, HIGH);

  // Meter
  m = meter%10; //Units
  l = meter%100; //Tens
  k = meter%1000; //Hundreds
  j = meter%10000; //Thousands

  // Isolate to only one digit
  j -= k; k -= l; l -= m;
  l /= 10; k /= 100;  j /= 1000; i = meter/10000;

  // Display to 7-Segments
        digitalWrite(LATCH, LOW);
        shiftOut(DATA, CLK, MSBFIRST, ~digit[i]); //Ten thousands
        shiftOut(DATA, CLK, MSBFIRST, ~digit[j]); //Thousands
        shiftOut(DATA, CLK, MSBFIRST, ~digit[k]); //Hundreds
        shiftOut(DATA, CLK, MSBFIRST, ~digit[l]); //Tens
        shiftOut(DATA, CLK, MSBFIRST, ~digit[m]); //Units
        digitalWrite(LATCH, HIGH);
  delay(250);
}