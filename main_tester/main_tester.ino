#include <DigitalIO.h>
//IC Tester
//MCbx 2017
//GNU GPL

#define patsNo 16
const PROGMEM byte patterns[]={0,255,85,170,204,51,153,102, 128, 64, 32, 16,8,4,2,1}; //patterns tested on chip
//85 - 1010, 170 - 0101, 204 - 0011, 51 - 1100, 153 - 1001, 102 - 0110

//chip definitions, sample values not used.
int rows=512;
int cols=512;
unsigned int wait=3000;
long words=262144;
byte addressBits=9;
byte bitsPerWord=1; 

//int - terrible hack
const PROGMEM unsigned int chips[]={4116, 4164, 41256, 4416, 4464, 44256, 21010, 74};
byte menuptr=0;
byte testNo=0;

byte (*read_word)(int, int);   //FILL THESE IN CHIP DEF!
void (*write_word)(byte, int, int);
void (*initialize_memory)();
void (*putAddress)(int);

DigitalPin<2> pin2;
DigitalPin<3> pin3;
DigitalPin<4> pin4;
DigitalPin<5> pin5;
DigitalPin<6> pin6;
DigitalPin<7> pin7;
DigitalPin<8> pin8;
DigitalPin<9> pin9;
DigitalPin<10> pin10;
DigitalPin<11> pin11;
DigitalPin<12> pin12;
DigitalPin<13> pin13;
DigitalPin<14> pin14;
DigitalPin<15> pin15;
DigitalPin<16> pin16;
DigitalPin<17> pin17;
DigitalPin<18> pin18;
DigitalPin<19> pin19;
    

inline void powerOn()
{
  Serial.print("~");
}

inline void powerOff()
{
  Serial.print("`");
}


//prepares program for chosen chip
void prepare(int chipNo)
{
  if (chipNo==0) //4116
  {
    rows=128;
    cols=128;
    words=16384;
    addressBits=7;
    bitsPerWord=1;
    read_word=&read_word1;
    write_word=&write_word1;
    initialize_memory=&initialize_memory1;
    putAddress=&putAddress1;
  }
  if (chipNo==1) //4164 OK
  {
    rows=256;
    cols=256;
    words=65536;
    addressBits=8;
    bitsPerWord=1;
    read_word=&read_word1;
    write_word=&write_word1;
    initialize_memory=&initialize_memory1;
    putAddress=&putAddress1;
  }
  if (chipNo==2)  //41256 OK
  {
    rows=512;
    cols=512;
    words=262144;
    addressBits=9;
    bitsPerWord=1;
    read_word=&read_word1;
    write_word=&write_word1;
    initialize_memory=&initialize_memory1;
    putAddress=&putAddress1;
  }
  if (chipNo==3)
  {    //4416 OK?
    rows=256;
    cols=64;
    words=16384;
    addressBits=7;
    bitsPerWord=4;
    read_word=&read_word4;
    write_word=&write_word4;
    initialize_memory=&initialize_memory1;
    putAddress=&putAddress4;
  }
  if (chipNo==4)
  {    //4464 OK
    rows=256;
    cols=256;
    words=65536;
    addressBits=8;
    bitsPerWord=4;
    read_word=&read_word4;
    write_word=&write_word4;
    initialize_memory=&initialize_memory1;
    putAddress=&putAddress4;
  }
  if (chipNo==5)
  {     //44256
    rows=512;
    cols=512;
    words=262144;
    addressBits=9;
    bitsPerWord=4;
    read_word=&read_word5;
    write_word=&write_word5;
    initialize_memory=&initialize_memory2;
    putAddress=&putAddress5;
  }
  if (chipNo==6)
  {    //21010 - ok
    rows=1024;
    cols=1024;
    words=1048576;
    addressBits=9;
    bitsPerWord=1;
    read_word=&read_word2;
    write_word=&write_word2;
    initialize_memory=&initialize_memory2;
    putAddress=&putAddress2;
  }
}

//displays menu to prepare tester for chips
void mainMenu()
{ 
  byte k='o';
  while (k!='s')
  {
    Serial.println("   DRAM  TEST");
    prepare(menuptr);
    Serial.print("   < ");
    Serial.print(pgm_read_word_near(chips+menuptr));
    Serial.println(" >");
    while (Serial.available()==0);
    k=Serial.read();
    if (k=='a')
    {
        menuptr--;
        if (menuptr==255)
            menuptr=7;
    }
    if (k=='d') //forward
    {
        menuptr++;
        if (menuptr==8)
            menuptr=0;
    }
  }
  
  if (menuptr==7) //TTL test bootstrap
  {
    BootTTLTester();
  }

  //DRAM STEP 2
  k='o'; //one more time ask user to configure switches and put chip
  while(k!='s')
  {
    Serial.println("CONFIG, PUT CHIP");
    if (menuptr==0)
    {
      Serial.println("SW 1, 2, 3, 4 UP");
    }
    if ((menuptr==1)||(menuptr==2))
    {
      Serial.println("    SW 4 UP");
    }
    if ((menuptr==3)||(menuptr==4)||(menuptr==6))
    {
      Serial.println("    SW 5 UP");
    }
    if (menuptr==5)
    {
      Serial.println("   ALL DOWN");
    }
   // Serial.println(" PUT CHIP IN");
    while (Serial.available()==0);
    k=Serial.read();
  }
}


void setup() {
  Serial.begin(38400);
  delay(100); //wait for terminal initialization
  
  //POWER TEST!
  byte k='2';
  while (k!='1')
  {
    powerOn();
    delay(150);
    Serial.println("PWR TEST");
    Serial.print("^");
    while (Serial.available()==0);
    k=Serial.read();
    powerOff();
    if (k!='1')
    {
      Serial.print((char)k);
      Serial.println(" -5V POWER BAD!");
      delay(750);
    }
  }
    Serial.println();
  mainMenu();

  //SET PIN MODES
  if (menuptr<3) //4116, 6164, 61256
  {
    pin6.high();
    pin8.high();
    pin7.high();
    pin12.high();
    pin11.high();
    pin10.high();
    pin13.high();
    pin9.high();
    pin2.high();
    pin3.high();
    pin5.high();
    pin15.high();
    pin4.high();
    pin14.mode(INPUT);
  }

  if ((menuptr==3)||(menuptr==4)) //4416, 4464
  {
    pin6.high();
    pin8.high();
    pin7.high();
    pin12.high();
    pin11.high();
    pin10.high();
    pin13.high();
    pin9.high();
    pin5.high();
    pin15.high();
    pin4.high();
    pin16.high();
    pin2.mode(INPUT);
    pin3.mode(INPUT);
    pin14.mode(INPUT);
    pin17.mode(INPUT);
  }
  
  if (menuptr==6) //21010
  {
    pin2.high();
    pin3.high();
    pin4.low();
    pin5.high();
    pin6.high();
    pin7.high();
    pin8.high();
    pin9.high();
    pin10.high();
    pin11.high();
    pin12.high();
    pin13.high();
    pin14.high();
    pin15.high();
    pin16.high();    
    pin17.mode(INPUT);
  }

  if (menuptr==5) //44256
  {
    pin2.high();
    pin3.high();
    pin4.low();
    pin5.high();
    pin6.high();
    pin7.high();
    pin8.high();
    pin9.high();
    pin10.high();
    pin11.high();
    pin12.high();
    pin13.high();
    pin14.high();
    pin15.high();
    pin16.mode(INPUT);
    pin17.mode(INPUT);
    pin18.mode(INPUT);
    pin19.mode(INPUT);
    
  }
  powerOn();

  //set refreshing interrupt!
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 1024;  //bad value but seems to work. Fails without.
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= (1 << CS12); // 256 prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  interrupts(); // enable all interrupts
}

//for 4116, 4164, 41256
void putAddress1(int addr) //this doesn't require NoInterrupts as it's called 
{                       //from inside of noInterrupts function.
    pin6.write(bitRead(addr,0));
    pin8.write(bitRead(addr,1));
    pin7.write(bitRead(addr,2));
    pin12.write(bitRead(addr,3));
    pin11.write(bitRead(addr,4));
    pin10.write(bitRead(addr,5));
    pin13.write(bitRead(addr,6));
    pin9.write(bitRead(addr,7));
    pin2.write(bitRead(addr,8));
}

//for 4416, 4464
void putAddress4(int addr) //this doesn't require NoInterrupts as it's called 
{                       //from inside of noInterrupts function.
    pin13.write(bitRead(addr,0));
    pin12.write(bitRead(addr,1));
    pin11.write(bitRead(addr,2));
    pin10.write(bitRead(addr,3));
    pin8.write(bitRead(addr,4));
    pin7.write(bitRead(addr,5));
    pin6.write(bitRead(addr,6));
    pin9.write(bitRead(addr,7));
}

//for 44256
void putAddress5(int addr) //this doesn't require NoInterrupts as it's called 
{                       //from inside of noInterrupts function.
    pin5.write(bitRead(addr,0));
    pin6.write(bitRead(addr,1));
    pin7.write(bitRead(addr,2));
    pin8.write(bitRead(addr,3));
    pin9.write(bitRead(addr,4));
    pin10.write(bitRead(addr,5));
    pin11.write(bitRead(addr,6));
    pin12.write(bitRead(addr,7));
    pin13.write(bitRead(addr,8));
}


//for 21010
void putAddress2(int addr) //this doesn't require NoInterrupts as it's called 
{                       //from inside of noInterrupts function.
    pin5.write(bitRead(addr,0));
    pin6.write(bitRead(addr,1));
    pin7.write(bitRead(addr,2));
    pin8.write(bitRead(addr,3));
    pin9.write(bitRead(addr,4));
    pin10.write(bitRead(addr,5));
    pin11.write(bitRead(addr,6));
    pin12.write(bitRead(addr,7));
    pin13.write(bitRead(addr,8));
    pin14.write(bitRead(addr,9));
}

//Refreshing. this has to be fired regurarly 
ISR(TIMER1_COMPA_vect)        // interrupt service routine 
{  
    if(menuptr==6)
    {
      for (int i=0;i<rows;i++)
      {
        (*putAddress)(i);
        pin3.low();
        pin3.high(); 
      }
      return;
    }
    for (int i=0;i<rows;i++)
    {
      (*putAddress)(i);
      pin5.low();
      pin5.high(); 
    }
} 

//write bit to memory, 4116, 4164, 41256
void write_word1(byte data, int row, int col)
{
  pin3.write(bitRead(data,0)); //data
  //row addr
  putAddress1(row);
  pin5.low();
  //we
  pin4.low();
  //col addr
  putAddress1(col);
  pin15.low();

  pin5.high();
  pin4.high();
  pin15.high();
}

//read bit from memory, 4116, 4164, 41256
byte read_word1(int row, int col)
{
  byte a=0;
  //row addr
  putAddress1(row);
   pin5.low();
  //col addr
  putAddress1(col);
  pin15.low();
  bitWrite(a,0,pin14.read());
  
  pin5.high(); 
  pin15.high();
  return a;
}

//initialize, 4116, 4164, 41256
void initialize_memory1() //shoot 8 RAS cycles before using.
{   //this is important.
  noInterrupts();
  for (byte i=0;i<8;i++)
  {
    pin5.low(); 
    pin5.high(); 
  }
  interrupts();
}

//for 4416, 4464
void write_word4(byte data, int row, int col)
{
  pin2.write(bitRead(data,0)); //data
  pin3.write(bitRead(data,1)); //data
  pin14.write(bitRead(data,2)); //data
  pin17.write(bitRead(data,3)); //data
  //row addr
  putAddress4(row);
  pin5.low();
  //we
  pin4.low();
  //col addr
  if (menuptr==4)
  {
    putAddress4(col);
  }
  else //4416 is strange.
  {
    pin12.write(bitRead(col,0));
    pin11.write(bitRead(col,1));
    pin10.write(bitRead(col,2));
    pin8.write(bitRead(col,3));
    pin7.write(bitRead(col,4));
    pin6.write(bitRead(col,5));
  }
    
  pin15.low();
  pin4.high();
  pin5.high();
  pin15.high();

  pin2.mode(INPUT);
  pin3.mode(INPUT);
  pin14.mode(INPUT);
  pin17.mode(INPUT);
}

//for 4416, 4464
byte read_word4(int row, int col)
{
  byte a=0;
  //row addr
  putAddress4(row);
  pin5.low();
  //col addr
    if (menuptr==4)
  {
    putAddress4(col);
  }
  else //4416 is strange. Columns have to be entered different way.
  {
    pin12.write(bitRead(col,0));
    pin11.write(bitRead(col,1));
    pin10.write(bitRead(col,2));
    pin8.write(bitRead(col,3));
    pin7.write(bitRead(col,4));
    pin6.write(bitRead(col,5));
  }
  pin15.low();

  //inputs
  pin2.mode(INPUT);
  pin3.mode(INPUT);
  pin14.mode(INPUT);
  pin17.mode(INPUT);

  // /g low
  pin16.low();

  //read things
  bitWrite(a,0,pin2.read());
  bitWrite(a,1,pin3.read());
  bitWrite(a,2,pin14.read());
  bitWrite(a,3,pin17.read());

  pin16.high();

  pin5.high(); 
  pin15.high();

  return a;
}



//for 44256
void write_word5(byte data, int row, int col)
{
  pin19.write(bitRead(data,0)); //data
  pin16.write(bitRead(data,1)); //data
  pin17.write(bitRead(data,2)); //data
  pin18.write(bitRead(data,3)); //data
  //row addr
  putAddress5(row);
  pin3.low();
  //we
  pin2.low();
  //col addr
  putAddress5(col);
  pin15.low();
  
  pin3.high();
  pin2.high();
  pin15.high();

  pin19.mode(INPUT);
  pin16.mode(INPUT);
  pin17.mode(INPUT);
  pin18.mode(INPUT);
}

//for 44256
byte read_word5(int row, int col)
{
  byte a=0;
  //row addr
  putAddress5(row);
  pin3.low();
  //col addr
  putAddress5(col);
  pin15.low();

  //inputs
  pin19.mode(INPUT);
  pin16.mode(INPUT);
  pin17.mode(INPUT);
  pin18.mode(INPUT);

  // /g low
  pin14.low();

  //read things
  bitWrite(a,0,pin19.read());
  bitWrite(a,1,pin16.read());
  bitWrite(a,2,pin17.read());
  bitWrite(a,3,pin18.read());

  pin14.high();

  pin3.high(); 
  pin15.high();

  return a;
}



//write bit to memory, 21010
void write_word2(byte data, int row, int col)
{
  pin16.write(bitRead(data,0)); //data
  //row addr
  putAddress2(row);
  pin3.low();
  //we
  pin2.low();
  //col addr
  putAddress2(col);
  pin15.low();

  pin2.high();
  pin15.high();
  pin3.high();
}

//read bit from memory, 21010
byte read_word2(int row, int col)
{
  byte a=0;
  //row addr
  putAddress2(row);
   pin3.low();
  //col addr
  putAddress2(col);
  pin15.low();
  bitWrite(a,0,pin17.read());
  
  pin3.high(); 
  pin15.high();

  return a;
}

//initialize, 21010
void initialize_memory2() //shoot 8 RAS cycles before using.
{   //this is important.
  noInterrupts();
  for (byte i=0;i<8;i++)
  {
    pin3.low(); 
    pin3.high(); 
  }
  interrupts();
}

//test pattern generator.
short pt1=-1;
byte pattern(byte no,byte bits)
{
  byte k=0;
  for (int i=0;i<bits;i++)
  {
    pt1++;
    if (pt1>7)
      pt1=0;
    bitWrite(k,i,bitRead(no,pt1));
  }
  return k;
}

//error halter
void throw_err(int r, int c, int t, int p)
{
   powerOff();
   Serial.println("");
   Serial.print("PASS ");
   Serial.print(t+1);
   Serial.print(" TEST ");
   Serial.print(p);
   Serial.println("");
   Serial.print("ERROR! ");
   Serial.println((long)r*rows+c);
   while(1);
}

void loop() {
  (*initialize_memory)();
  delay(100);
  for (byte curPatt=0;curPatt<patsNo;curPatt++) //patterns loop
  {
    Serial.print("PASS ");
    Serial.print(testNo+1);
    Serial.print(" TEST ");
    Serial.print(curPatt);
    Serial.println("");
    Serial.print("WRITE ");
    Serial.flush();
    pt1=-1;
    noInterrupts();
    //Now by calling rows more frequently, we implicitly refresh memory
    for (int c=0;c<cols;c++)
    {
      for (int r=0;r<rows;r++)
      {
        (*write_word)(pattern(pgm_read_byte_near(patterns+curPatt),bitsPerWord), r,c);
      // Serial.println(pattern(patterns[curPatt],bitsPerWord),BIN); //generator test
      }
    }
    interrupts(); //WAIT: Refreshing interrupt go!
    Serial.print("WAIT ");
    delay(wait);
    pt1=-1;
    Serial.print("READ ");
    Serial.flush();
    noInterrupts();
    for (int c=0;c<cols;c++)
    {
       for (int r=0;r<rows;r++)
       {
         if ((*read_word)(r,c)!=pattern(pgm_read_byte_near(patterns+curPatt),bitsPerWord))
         {
          interrupts();
//          Serial.println((*read_word)(r,c),BIN);
          throw_err(r,c,testNo,curPatt);
         }
       }
    }
    interrupts();
    Serial.println("");
  }
  testNo++;
}
