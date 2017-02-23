/**************************************************************************/
/*!
    @file     GSL1680.cpp
    @author   ESTBLC
    @section  HISTORY
    
    v1.0 - First release

    @thanks to wolfmanjm for the source code this lib is bassed on : https://github.com/wolfmanjm/GSL1680
*/

#include <GSL1680.h>

// Registres
#define I2CADDR 0x40
#define DATA_REG 0x80
#define STATUS_REG 0xE0
#define PAGE_REG 0xF0


//CONST
#define BUFSIZE 32

struct Sts_event ts_event;

GSL1680::GSL1680 () {
    
}

void GSL1680::begin(uint8_t WAKE, uint8_t INTRPT) {
    Serial.println("Starting TS boot up"); 
    pinMode(WAKE, OUTPUT);          //
    digitalWrite(WAKE, LOW);        //
    pinMode(INTRPT, INPUT_PULLUP);  // Startup sequence PIN part
    delay(100);
    Serial.println("Toggle Wake");
	digitalWrite(WAKE, HIGH);
	delay(50);
	digitalWrite(WAKE, LOW);
	delay(50);
	digitalWrite(WAKE, HIGH);
	delay(30);
    Wire.begin();

    // CTP startup sequence
	Serial.println("Clear reg");
	clear_reg();
	Serial.println("Reset chip");
	reset();
	Serial.println("Load FW");
	loadfw();
	//startup_chip();
	Serial.println("Reset chip 2");
	reset();
	Serial.println("Startup Chip");
    startchip();
	Serial.println("Init Done");
}

void GSL1680::clear_reg() {
    uint8_t REG[4] = {0xE0, 0x80, 0xE4, 0xE0};
    uint8_t DATA[4] = {0x88, 0x01, 0x04, 0x00};
    uint8_t TIMER[4] = {20, 5, 5, 20};

    for (int i = 0; i<=4; i++) {
	    Wire.beginTransmission(I2CADDR);
        Wire.send(REG[i]);
        Wire.send(DATA[i]);
        delay(TIMER[i]);
        int r = Wire.endTransmission();
        if (r != 0){
            Serial.print("i2c write error: "); Serial.print(r); Serial.print(" "); Serial.println(REG[i], HEX);
        }
    }
}

void GSL1680::reset() {
    uint8_t REG[4] = {STATUS_REG, 0xE4};
    uint8_t DATA[4] = {0x88, 0x04};
    uint8_t TIMER[4] = {20, 10};

    for (int i = 0; i<=2; i++) {
	    Wire.beginTransmission(I2CADDR);
        Wire.send(REG[i]);
        Wire.send(DATA[i]);
        delay(TIMER[i]);
        int r = Wire.endTransmission();
        if (r != 0){
            Serial.print("i2c write error: "); Serial.print(r); Serial.print(" "); Serial.println(REG[i], HEX);
        }
    }

    DATA[0] = 0x00;
    DATA[1] = 0x00;
    DATA[2] = 0x00;
    DATA[3] = 0x00;
    datasend (0xBC, DATA, 4);
    delay(10);

}

void GSL1680::loadfw(){
    uint8_t addr;
    uint8_t Wrbuf[4];
    uint source_line = 0;
    uint source_len = sizeof(GSLX680_FW) / sizeof(struct fw_data);


    for (source_line = 0; source_line < source_len; source_line++) {
        addr = GSLX680_FW[source_line].offset;
        Wrbuf[0] = (char)(GSLX680_FW[source_line].val & 0x000000ff);
        Wrbuf[1] = (char)((GSLX680_FW[source_line].val & 0x0000ff00) >> 8);
        Wrbuf[2] = (char)((GSLX680_FW[source_line].val & 0x00ff0000) >> 16);
        Wrbuf[3] = (char)((GSLX680_FW[source_line].val & 0xff000000) >> 24);

        datasend(addr, Wrbuf, 4);
    }
}

void GSL1680::startchip() {
    Wire.beginTransmission(I2CADDR);
    Wire.write(0xE0);   //Registre
    Wire.write(0x00);   //DATA
    int r = Wire.endTransmission();
    if (r != 0){
        Serial.print("i2c write error: "); Serial.print(r); Serial.print(" "); Serial.println(0xE0, HEX);
    }
}

void GSL1680::sleep() {

}


void GSL1680::datasend(uint8_t REG, uint8_t DATA[], uint16_t NB) {
    Wire.beginTransmission(I2CADDR);
    Wire.write(REG);
    
    for (uint16_t i=0; i<NB; i++) {
        Wire.write(DATA[i]);
    }
    int r = Wire.endTransmission();
    if (r != 0){
        Serial.print("i2c write error: "); Serial.print(r); Serial.print(" "); Serial.println(REG, HEX);
    }
    return;
}

uint8_t GSL1680::dataread() {
    uint8_t TOUCHRECDATA[24] = {0};
    Wire.beginTransmission(I2CADDR);
    Wire.write(DATA_REG);

    int n = Wire.endTransmission();
    if (n != 0) {
        Serial.print("i2c write error: "); Serial.print(n); Serial.print(" "); Serial.println(DATA_REG, HEX);
    }

    n = Wire.requestFrom(I2CADDR, 24);
    if (n != 24) {
        Serial.print("i2c read error: did not get expected count "); Serial.print(n); Serial.print("/"); Serial.println("24");
    }

    for(int i = 0; i<n; i++) {
        TOUCHRECDATA[i] = Wire.read();
    }

    ts_event.NBfingers = TOUCHRECDATA[0];
    for(int i = 0; i<ts_event.NBfingers; i++) {
        ts_event.coords[i].X = ( (((uint32_t)TOUCHRECDATA[(i*4)+5])<<8) | (uint32_t)TOUCHRECDATA[(i*4)+4] ) & 0x00000FFF; // 12 bits of X coord
        ts_event.coords[i].Y = ( (((uint32_t)TOUCHRECDATA[(i*4)+7])<<8) | (uint32_t)TOUCHRECDATA[(i*4)+6] ) & 0x00000FFF;
        ts_event.coords[i].fingerID = (uint32_t)TOUCHRECDATA[(i*4)+7] >> 4; // finger that did the touch
    }
    return ts_event.NBfingers;

}

uint8_t GSL1680::readFingerID(int NB) {
    return ts_event.coords[NB].fingerID;
}

uint32_t GSL1680::readFingerX(int NB) {
    return ts_event.coords[NB].X;
}

uint32_t GSL1680::readFingerY(int NB) {
    return ts_event.coords[NB].Y;
}