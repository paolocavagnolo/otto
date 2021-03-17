#include <SoftwareSerial.h>

SoftwareSerial rs(2,3); //rx, tx

#define debug true

#define NUM_MOTORS 5
#define MAX_LEN 34

#define m1_led 13
#define m2_led 12
#define m3_led 11
#define m4_led 10
#define m5_led 9
#define m6_led 8

#define m1_add 0x01
#define m2_add 0x02
#define m3_add 0x03
#define m4_add 0x04
#define m5_add 0x05
#define m6_add 0x06

uint8_t adds[NUM_MOTORS] = {m1_add,m2_add,m3_add,m4_add,m5_add};
uint8_t leds[NUM_MOTORS] = {m1_led,m2_led,m3_led,m4_led,m5_led};

uint16_t pJ[NUM_MOTORS];

#define tch_btn 7
#define rec_btn 6
#define gtp_btn 5

#define de_rs 4
#define tx_rs 3
#define rx_rs 2

uint8_t r[MAX_LEN];
uint8_t t[MAX_LEN];

void alive();
void alive(uint8_t id);
bool ping(uint8_t id);
void relax();
void hold();
void zero_r();
void zero_p();
void gotop(uint16_t a, uint16_t b, uint16_t c, uint16_t d, uint16_t e);
//void record();
void tx(uint8_t id, uint8_t len, uint8_t cmd);
void tx(uint8_t l, uint8_t id, uint8_t len, uint8_t cmd, uint8_t *prms);
void rx();
//void gotopoint(uint8_t *p);

void setup() {

  rs.begin(115200);

  pinMode(m1_led,OUTPUT);
  pinMode(m2_led,OUTPUT);
  pinMode(m3_led,OUTPUT);
  pinMode(m4_led,OUTPUT);
  pinMode(m5_led,OUTPUT);
  pinMode(m6_led,OUTPUT);

  pinMode(de_rs,OUTPUT);

  pinMode(tch_btn,INPUT_PULLUP);
  pinMode(rec_btn,INPUT_PULLUP);
  pinMode(gtp_btn,INPUT_PULLUP);

  rs.flush();
  
  Serial.begin(9600);
  delay(1000);

}

char in;
byte com[2];
uint8_t i=0;
uint8_t j=0;
uint8_t num = 0;
uint8_t ll=1;

void loop() {
  /*
  //alive motor / update led
  alive();
  relax();
  delay(5000);
  pos();
  for (uint8_t i=0;i<NUM_MOTORS;i++) {
    Serial.print(pJ[i]);
    Serial.print(" ");
  }
  Serial.println();
  gotop(10,3383,1465,3310,10);
  delay(5000);
  */

  //setPin(0x04);
  //delay(1000);
  //alive();
  if (Serial.available()) {
    in = Serial.read();

    if (in == 'x') {
      ll = 1;
      Serial.read();
    }
    else if (in == 'y') {
      ll = 2;
      Serial.read();
    }
    else if (in == 0x0A) {
      com[j] = convert(num);
      Serial.println(com[j],HEX);
      i = 0;
      j++;
      num = 0;
    }
    else {
      //Serial.print(in-48,DEC);
      if (i==0) {
        num = num + (int)(in-48)*10;
      }
      else {
        num = num + (int)(in-48);
      }
      i++;
    }

    if (j > ll) {
      tx(2,0x04,ll+3,0x03,com);
      j = 0;
    }
    
  }
  
  
}
/*
void record() {

  static uint8_t r[MAX_LEN];

  for (uint8_t i=0; i<NUM_MOTORS; i++) {
    tx(adds[i],0x04,0x02,{0x38,0x02});
    r = rx();
    pJ[i] = word(r[5],r[6]);
  }
  
  //TORQUE SWITCH - ON
  tx(0xFE,0x04,0x04,{0x28,0x01});

  //ACTION
  tx(0xFE,0x02,0x05);
}*/



void relax() {
  uint8_t pp[2] = {0x28,0x00};

  //TORQUE SWITCH - OFF
  tx(2,0xFE,0x04,0x03,pp);

  //ACTION
  //tx(0xFE,0x02,0x05);

}

void hold() {
  uint8_t pp[2] = {0x28,0x01};
  
  //TORQUE SWITCH - OFF
  tx(2,0xFE,0x04,0x03,pp);

  //ACTION
  //tx(0xFE,0x02,0x05);

}

void setPin(byte b) {
  uint8_t pp[2] = {0x05,b};
  
  //TORQUE SWITCH - OFF
  tx(2,0xFE,0x04,0x03,pp);
}

void alive() {

  for (uint8_t i=0; i<NUM_MOTORS; i++) {
    if (ping(adds[i])) {
      digitalWrite(leds[i],HIGH);
    }
    else {
      digitalWrite(leds[i],LOW);
    }
  }
  
}

void alive(uint8_t id) {

  if (ping(id)) {
    digitalWrite(leds[id-1],HIGH);
  }
  else {
    digitalWrite(leds[id-1],LOW);
  }


}

bool ping(uint8_t id) {
  
  zero_r();
  tx(id,0x02,0x01);
  delayMicroseconds(500);
  rx();
  
  if ((r[1] == 0xF5) && (r[2] == id) && (r[4] == 0x00)) {
    return true;
  }
  else {
    return false;
  }

}

void tx(uint8_t id, uint8_t len, uint8_t cmd) {
  
  uint8_t csum; //calc chk sum
  csum = 0xFF - id - len - cmd;

  digitalWrite(de_rs, HIGH);
  rs.write(0xFF);
  rs.write(0xFF);
  rs.write(id);
  rs.write(len);
  rs.write(cmd);
  rs.write(csum);
  digitalWrite(de_rs,LOW);

  if (debug) {
    Serial.print(0xFF,HEX);
    Serial.print(" ");
    Serial.print(0xFF,HEX);
    Serial.print(" ");
    Serial.print(id,HEX);
    Serial.print(" ");
    Serial.print(len,HEX);
    Serial.print(" ");
    Serial.print(cmd,HEX);
    Serial.print(" ");
    Serial.println(csum,HEX);
  }
}

void tx(uint8_t l, uint8_t id, uint8_t len, uint8_t cmd, uint8_t *prms) {
  
  uint8_t csum; //calc chk sum
  csum = 0xFF - id - len - cmd;
  for (uint8_t i=0; i<len-l ; i++) {
    csum -= prms[i];
  }

  digitalWrite(de_rs, HIGH);
  rs.write(0xFF);
  rs.write(0xFF);
  rs.write(id);
  rs.write(len);
  rs.write(cmd);
  for (uint8_t i=0; i<len-l; i++) {
    rs.write(prms[i]);
  }
  rs.write(csum);
  digitalWrite(de_rs,LOW);

  if (debug) {
    Serial.print(0xFF,HEX);
    Serial.print(" ");
    Serial.print(0xFF,HEX);
    Serial.print(" ");
    Serial.print(id,HEX);
    Serial.print(" ");
    Serial.print(len,HEX);
    Serial.print(" ");
    Serial.print(cmd,HEX);
    Serial.print(" ");
    for (uint8_t i=0; i<len-l; i++) {
      Serial.print(prms[i],HEX);
      Serial.print(" ");
    }
    Serial.println(csum,HEX);
  }
}

void rx() {

  uint8_t i = 0;
  uint8_t a;
  
  while (rs.available()) {
    r[i] = rs.read();
    i++;
  }

}

void zero_r() {
  for (uint8_t i=0; i<MAX_LEN; i++) {
    r[i] = 0;
  }
}

void zero_p() {
  for (uint8_t i=0; i<NUM_MOTORS; i++) {
    pJ[i] = 0;
  }
}

void pos() {
  zero_p();
  uint8_t pp[2] = {0x38,0x02};
  for (uint8_t i=0; i<NUM_MOTORS;i++) {
    tx(2,adds[i],4,2,pp);
    delayMicroseconds(500);
    rx();
    pJ[i] = word(r[5],r[6]);
  }
}



void gotop(uint16_t a, uint16_t b, uint16_t c, uint16_t d, uint16_t e) {
  
}

byte convert(byte a) {
  
  switch(a) {
    case 0:
      return 0x00;
    case 1:
      return 0x01;
    case 2:
      return 0x02;
    case 3:
      return 0x03;
    case 4:
      return 0x04;
    case 5:
      return 0x05;
    case 6:
      return 0x06;
    case 7:
      return 0x07;
    case 8:
      return 0x08;
    case 9:
      return 0x09;
    case 10:
      return 0x0A;
    case 11:
      return 0x0B;
    case 12:
      return 0x0C;
    case 13:
      return 0x0D;
    case 14:
      return 0x0E;
    case 15:
      return 0x0F;
    case 16:
      return 0x10;
    case 17:
      return 0x11;
    case 18:
      return 0x12;
    case 19:
      return 0x13;
    case 20:
      return 0x14;
    case 21:
      return 0x15;
    case 22:
      return 0x16;
    case 23:
      return 0x17;
    case 24:
      return 0x18;
    case 25:
      return 0x19;
    case 26:
      return 0x1A;
    case 27:
      return 0x1B;
    case 28:
      return 0x1C;
    case 29:
      return 0x1D;
    case 40:
      return 0x28;
    case 41:
      return 0x29;
    case 42:
      return 0x2A;
    case 43:
      return 0x2B;
    case 44:
      return 0x2C;
    case 45:
      return 0x2D;
    case 46:
      return 0x2E;
    case 47:
      return 0x2F;
    case 48:
      return 0x30;
 
    case 56:
      return 0x38;
    case 57:
      return 0x39;
    case 58:
      return 0x3A;
    case 59:
      return 0x3B;
    case 60:
      return 0x3C;
    case 61:
      return 0x3D;
    case 62:
      return 0x3E;
    case 63:
      return 0x3F;
    case 64:
      return 0x40;
    case 65:
      return 0x41;
    
  }
}

/*
void gotopointL(uint8_t *p) {
  
  uint8_t l1 = 125;
  uint8_t l2 = 125;
  uint8_t l3 = 60.7;
  uint8_t h1 = 72.2;

}

void gotopointJ(uint8_t *p) {
  
  for (uint8_t i=0; i<NUM_MOTORS; i++) {
    //SET POSITION
    tx(adds[i],0x04,0x04,{0x2A,p[i]});
  }
  //ACTION
  tx(0xFE,0x02,0x05);
}*/
