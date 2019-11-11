#define I2C_SLAVE_ADDRESS 0x10

#define LED 3
#define CHA 4
#define CHB 1

#include "TinyWireS.h"
#include <avr/interrupt.h>

#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 32 )
#endif

volatile byte i2c_regs[] = {0,0,0,0};
volatile int32_t encoder_count = 0;
volatile bool cha_ = 0, chb_ = 0;
volatile byte reg_position = 0;
const byte reg_size = sizeof(i2c_regs);
volatile byte cmd = 0;

void setup() {
  TinyWireS.begin(I2C_SLAVE_ADDRESS);
  TinyWireS.onRequest(requestEvent);
  TinyWireS.onReceive(commandEvent);

  pinMode(LED, OUTPUT);
  pinMode(CHA, INPUT);
  pinMode(CHB, INPUT);

  GIMSK |= 0x20;
  PCMSK |= 0x12;
  sei();
}

void loop() {
  digitalWrite(LED, LOW);
}

ISR(PCINT0_vect)  
{
  volatile bool cha = 0, chb = 0;
  cha = (PINB >> CHA) & 0x01;
  chb = (PINB >> CHB) & 0x01;

  if (cha_ != cha)
  {
    if (cha == 0 and chb == 0) encoder_count--;
    else if (cha == 0 and chb == 1) encoder_count++;
    else if (cha == 1 and chb == 0) encoder_count++;
    else if (cha == 1 and chb == 1) encoder_count--;
  }
  
  else if (chb_ != chb)
  {
    if (cha == 0 and chb == 0) encoder_count++;
    else if (cha == 0 and chb == 1) encoder_count--;
    else if (cha == 1 and chb == 0) encoder_count--;
    else if (cha == 1 and chb == 1) encoder_count++;
  }

  cha_ = cha;
  chb_ = chb;
}

void requestEvent()
{
  if (cmd = 0x22)
  {

    TinyWireS.write(i2c_regs[reg_position]);

    reg_position++;
    if (reg_position >= reg_size)
    {
      reg_position = 0;
    }
    digitalWrite(LED, HIGH);
    cmd = 0;
  }
}

void commandEvent()
{
  cmd = TinyWireS.read();
  if(cmd == 0x22)
  {
    uint8_t i=0;
    for(i=0; i<=reg_size; i++)
    {
      i2c_regs[i] = (encoder_count>>(8*i))&0xFF;
    }
  }
  else if(cmd == 0x33) encoder_count = 0;
}
