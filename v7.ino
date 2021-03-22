#include <SoftwareSerial.h>

#define PI 3.1415

#define NUM_MOTORS 5
#define MAX_LEN 34

#define m1_led 8
#define m2_led 12
#define m3_led 11
#define m4_led 9
#define m5_led 10
#define m6_led 13

#define m1_add 0x01
#define m2_add 0x02
#define m3_add 0x03
#define m4_add 0x04
#define m5_add 0x05
#define m6_add 0x06

uint8_t adds[NUM_MOTORS] = {m1_add,m2_add,m3_add,m4_add,m5_add};
uint8_t leds[NUM_MOTORS] = {m1_led,m2_led,m3_led,m4_led,m5_led};

#define de_rs 4
#define tx_rs 3
#define rx_rs 2

SoftwareSerial rs(rx_rs,tx_rs);

uint8_t r[MAX_LEN];

float x[5] = {0,0,0,0,0};
float y[5] = {0,0,0,0,0};

float l0 = 72.2;
float l1 = 125;
float l2 = 125;
float l3 = 60.7;

float a;
float b;
float c;

float C = PI/2;

void risolvi2(float pX, float pY);
void daAngoli();

void setup() {

  pinMode(m1_led,OUTPUT);
  pinMode(m2_led,OUTPUT);
  pinMode(m3_led,OUTPUT);
  pinMode(m4_led,OUTPUT);
  pinMode(m5_led,OUTPUT);
  pinMode(m6_led,OUTPUT);

  pinMode(de_rs,OUTPUT);

  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);

  rs.begin(115200);
  rs.flush();

  x[0] = 200;
  y[0] = 400;

  x[1] = x[0];
  y[1] = y[0]-l0;

  Serial.begin(9600);
  
}

float pX;
float pY;

long aliveTime = 0;
long solveTime = 0;
uint16_t j = 0;

bool relaxTrig = false;
long relaxTime = 0;

bool posTrig = false;
long posTime = 0;

bool mode = false;

void loop() {
  
  //ALIVE to LEDs
  if ((millis() - aliveTime) > 1000) {
    aliveTime = millis();
    alive();

    /*
    int a_px = floor((180*a/PI*-40+1980)/100);
    int b_px = floor(((180*a/PI*-40+1980)-a_px*100)/10);
    int c_px = round((180*a/PI*-40+1980)-a_px*100-b_px*10);
    
    Serial.write(120);
    Serial.write(a_px+48);
    Serial.write(b_px+48);
    Serial.write(c_px+48);*/
    
  }

  //BUTTON 1 relax/hold (6) closer to leds
  if (!digitalRead(6) && !relaxTrig) {
    uint8_t pp[2] = {0x28,mode};
    tx(2,0xFE,0x04,0x03,pp);
    relaxTrig = true;
    relaxTime = millis();
    mode = !mode;
    
  }
  if ((millis() - relaxTime) > 500) {
    relaxTrig = false;
  }

  //BUTTON 2 go vertical (7) 
  if (!digitalRead(7) && !posTrig) {
    uint8_t pp[2] = {0x28,mode};
    pos2mot(2000,2000,2000);
    posTrig = true;
    posTime = millis();
  }
  if ((millis() - posTime) > 500) {
    posTrig = false;
  }


  if (Serial.available()) {
    char in = Serial.read();
    if (in == 'x') {
      Serial.println("leggo pos: ");
      uint8_t pp[2] = {0x38,0x02};
      tx(2,0x02,4,0x02,pp);
      delayMicroseconds(500);
      rx();
      if ((r[1] == 0xF5) && (r[2] == 0x02)) {
        Serial.print(word(r[5],r[6]));
        Serial.print(" ");
      }
      tx(2,0x03,4,0x02,pp);
      delayMicroseconds(500);
      rx();
      if ((r[1] == 0xF5) && (r[2] == 0x03)) {
        Serial.print(word(r[5],r[6]));
        Serial.print(" ");
      }
      tx(2,0x04,4,0x02,pp);
      delayMicroseconds(500);
      rx();
      if ((r[1] == 0xF5) && (r[2] == 0x04)) {
        Serial.println(word(r[5],r[6]));
      }
    }
  }

  /*
  //POSs from processing
  if (Serial.available()) {
    char in = Serial.read();
    int num = 0;
    if (in == 'x') {
      while (Serial.available() < 3);
      in = Serial.read();
      num = num + (in-48)*100;
      in = Serial.read();
      num = num + (in-48)*10;
      in = Serial.read();
      num = num + (in-48)*1;
      pX = num;
    }
    if (in == 'y') {
      while (Serial.available() < 3);
      in = Serial.read();
      num = num + (in-48)*100;
      in = Serial.read();
      num = num + (in-48)*10;
      in = Serial.read();
      num = num + (in-48)*1;
      pY = num;
    }
   
  }
  

  
  //CALOLA angoli da coordinate
  if ((millis() - solveTime) > 200) {
    solveTime = millis();

    risolvi2(pX,pY);
    mov((180*a/PI*-40+1980));
  }
  
  

  //pos2mot(a,b,c);
  */
 
}

void alive() {

  for (uint8_t kk=0; kk<NUM_MOTORS; kk++) {
    if (ping(adds[kk])) {
      digitalWrite(leds[kk],HIGH);
    }
    else {
      digitalWrite(leds[kk],LOW);
    }
  }

}

void pos2mot(uint16_t a, uint16_t b, uint16_t c) {

  byte cmd[3] = {0x2a,0x08,0x00};

  tx(2,0x02,0x05,0x03,cmd);
  delayMicroseconds(2500);
  tx(2,0x03,0x05,0x03,cmd);
  delayMicroseconds(2500);
  tx(2,0x04,0x05,0x03,cmd);

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

}


bool ping(uint8_t id) {
  
  for (uint8_t i=0; i<MAX_LEN; i++) {
    r[i] = 0;
  }
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

void rx() {

  uint8_t iii = 0;
  
  while (rs.available()) {
    r[iii] = rs.read();
    iii++;
  }

}

void mov(uint16_t ang) {

  uint8_t pp1[2] = {0x1C,0x01};
  tx(2,0x01,0x04,0x03,pp1);
  delay(1);
  uint8_t pp2[2] = {0x28,0x01};
  tx(2,0x01,0x04,0x03,pp2);
  delay(1);

  uint8_t pp3[5] = {0x2A,highByte(ang),lowByte(ang),0x03,0xE8};
  tx(2,0x01,0x07,0x03,pp3);
  
}


void risolvi2(float pX, float pY) {

  float x3 = pX - l3*cos(-C);
  float y3 = pY + l3*sin(-C);
  
  float Cx;
  if (y3 < y[1]) {
    Cx = acos((x3-x[1])/dist(x[1],y[1],x3,y3));
  }
  else {
    Cx = -acos((x3-x[1])/dist(x[1],y[1],x3,y3));
  }
  
  float lim = l1 + l2 + l3*cos(Cx+C);
  
  float A;
  if (pY < y[1]) {
    A = PI/2-acos((pX-x[1])/dist(x[1],y[1],pX,pY));
  }
  else {
    A = PI/2+acos((pX-x[1])/dist(x[1],y[1],pX,pY));
  }
  float d41 = dist(pX,pY,x[1],y[1]);
  
  if (d41 > l1 + l2 + l3) {
    
    a = A;
    b = 0;
    c = 0;
    daAngoli();
    
  }
  else {
    if (d41 > lim) {
      b = 0;

      float aa = acos(((l1+l2)*(l1+l2)+d41*d41-l3*l3)/(2*(l1+l2)*d41));
      float cc = acos(((l1+l2)*(l1+l2)+l3*l3-d41*d41)/(2*(l1+l2)*l3));

      a = A - aa;
      c = PI - cc;

      daAngoli();
    }
    else {
      float xm = 0;
      float ym = 0;

      if (x3 > x[1]) {
        xm = (x3 - x[1])/2 + x[1];
      }
      else {
        xm = (x[1] - x3)/2 + x3;
      }
      if (y3 > y[1]) {
        ym = (y3 - y[1])/2 + y[1];
      }
      else {
        ym = (y[1] - y3)/2 + y3;
      }

      //circle(xm,ym,5);
      float d13 = dist(x3,y3,x[1],y[1]);

      float h = sqrt(l1*l1 - (d13/2)*(d13/2));

      float x2 = xm + h*(y3-y[1])/d13;
      float y2 = ym - h*(x3-x[1])/d13;

      float d12 = dist(x[1],y[1],x2,y2);
      float d23 = dist(x2,y2,x3,y3);

      float aa = acos((d12*d12+d13*d13-d23*d23)/(2*d12*d13));
      float bb = acos((d12*d12+d23*d23-d13*d13)/(2*d12*d23));

      float cc = acos((d13*d13+d41*d41-l3*l3)/(2*d13*d41));

      if ((A > C-PI/2) && (A < C+PI/2)) {
        a = A-aa-cc;
      }
      else {
        a = A-aa+cc;
      }

      b = PI - bb;
      c = C+PI/2 - b - a;

      daAngoli();
    }
  }

}

void daAngoli() {
  if (a > radians(75)) {
    a = radians(75);
  }
  else if (a < radians(-70)) {
    a = radians(-70);
  }

  if (b > radians(220)) {
    b = radians(220);
  }
  else if (b < radians(-220)) {
    b = radians(-220);
  }

  if (c > radians(140)) {
    c = radians(140);
  }
  else if (c < radians(-140)) {
    c = radians(-140);
  }

  x[2] = x[1] + l1*cos(PI/2-a);
  y[2] = y[1] - l1*sin(PI/2-a);

  x[3] = x[2] + l2*cos(PI/2-a-b);
  y[3] = y[2] - l2*sin(PI/2-a-b);

  x[4] = x[3] + l3*cos(PI/2-a-b-c);
  y[4] = y[3] - l3*sin(PI/2-a-b-c);
  
}

float dist(float xa, float ya, float xb, float yb) {
  return sqrt((xa-xb)*(xa-xb)+(ya-yb)*(ya-yb));
}
