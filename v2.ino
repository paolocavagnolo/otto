#include <SoftwareSerial.h>

#define NUM_MOTORS 6
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

uint8_t adds[6] = {m1_add,m2_add,m3_add,m4_add,m5_add,m6_add};
uint8_t leds[6] = {m1_led,m2_led,m3_led,m4_led,m5_led,m6_led};

uint8_t pJ[5];

#define tch_btn 7
#define rec_btn 6
#define gtp_btn 5

#define de_rs 4
#define tx_rs 3
#define rx_rs 2

void alive();
bool ping(uint8_t id);
void relax();
void record();
void tx(uint8_t id, uint8_t len, uint8_t cmd);
void tx(uint8_t id, uint8_t len, uint8_t cmd, uint8_t *prms);
uint8_t * rx();
void gotopoint(uint8_t );

void setup() {

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

}

void loop() {
	
	//alive motor / update led
	alive();

	//touch mode
	if (!digitalRead(tch_btn)) {
		relax();
	}
	
	//record point and torque on
	if (!digitalRead(rec_btn)) {
		record();
	}


	//gotopoint
	if (!digitalRead(gtp_btn)) {
		gotopoint();
	}
}

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
}

void relax() {

	//TORQUE SWITCH - OFF
	tx(0xFE,0x04,0x04,{0x28,0x00});

	//ACTION
	tx(0xFE,0x02,0x05);

}

void alive() {

	for (uint8_t i=0; i<NUM_MOTORS; i++) {
		if ping(add[i]) {
			digitalWrite(led[i],HIGH);
		}
		else {
			digitalWrite(led[i],LOW);
		}
	}
	
}

bool ping(uint8_t id) {
	static uint8_t r[MAX_LEN];

	tx(id,0x01,0x00);
	delay(1);
	r = rx();

	if ((r[1] == 0xF5) && (r[2] == id) && (r[4] == 0x00)) {
		return true;
	}
	else {
		return false;
	}

}

void tx(uint8_t id, uint8_t len, uint8_t cmd) {
	
	uint8_t csum; //calc chk sum

	digitalWrite(de_rs, HIGH);

	rs.write(id);
	rs.write(len);
	rs.write(cmd);
	rs.write(csum);

	digitalWrite(de_rs,LOW);
}

void tx(uint8_t id, uint8_t len, uint8_t cmd, uint8_t *prms) {
	
	uint8_t csum; //calc chk sum

	digitalWrite(de_rs, HIGH);

	rs.write(id);
	rs.write(len);
	rs.write(cmd);
	for (uint8_t i=0; i<len-2; i++) {
		rs.write(prms[i]);
	}
	uint8_t csum; //calc chk sum

	digitalWrite(de_rs,LOW);
}

uint8_t * rx() {

	static uint8_t r[MAX_LEN];

	// 

	return * r;
}
