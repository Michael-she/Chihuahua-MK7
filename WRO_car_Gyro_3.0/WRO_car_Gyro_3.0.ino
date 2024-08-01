// Basic demo for readings from Adafruit BNO08x
#include <Adafruit_BNO08x.h>


#include <Servo.h>

#include <Wire.h>

Servo myservo;

int targetAngle = 0;


// For SPI mode, we need a CS pin
#define BNO08X_CS 10
#define BNO08X_INT 9

// For SPI mode, we also need a RESET 
//#define BNO08X_RESET 5
// but not for I2C or UART
#define BNO08X_RESET -1



 int  trueAngleLookup [] =  {0,	1,	2,	2,	3,	4,	4,	5,	5,	6,	6,	7,	7,	8,	9,	9,	10,	10,	11,	11,	12,	13,	13,	14,	15,	15,	16,	16,	17,	18,	18,	19,	20,	20,	21,	21,	22,	23,	23,	24,	25,	25,	26,	26,	27,	28,	28,	29,	30,	30,	31,	31,	32,	33,	33,	34,	35,	35,	36,	36,	37,	38,	38,	39,	40,	40,	41,	41,	42,	43,	43,	44,	45,	45,	46,	46,	47,	48,	48,	49,	50,	50,	51,	52,	53,	53,	54,	55,	56,	57,	58,	58,	59,	60,	61,	62,	63,	63,	64,	65,	66,	67,	68,	68,	69,	70,	71,	72,	73,	73,	74,	75,	76,	77,	78,	78,	79,	80,	81,	82,	83,	83,	84,	85,	86,	87,	88,	88,	89,	90,	91,	92,	94,	95,	96,	98,	99,	100,	101,	102,	104,	105,	106,	107,	109,	110,	111,	112,	114,	115,	116,	117,	119,	120,	121,	122,	124,	125,	126,	127,	129,	130,	131,	133,	134,	135,	137,	138,	140,	142,	145,	147,	150,	153,	157,	161,	165,	170,	175,	180, 180};
 
 const int servoPin = 2;



Adafruit_BNO08x  bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

int seg1[] = { 9, 3, 6, 7, 8, 12, 11, 10 };

int seg2[] = {17, 13, 14, 15, 16, 22, 21, 20};
int trueAngle = 0;
int trueAngleOffset = 0;
int displayAngle = 0;
int angle;
int angleOld = 9999;
int lookUpAngle;
void setup(void) {

   
  Serial.begin(115200);
  delay(1000);
  initSegs();
Serial.println("FLASH");

for (int i = 0; i < 10; i++) {

    updateSeg1(i);
    updateSeg2(i);
    delay(20);
  }

 Wire1.setSDA(18);
  Wire1.setSCL(19);
  Serial.println("Wire1 SET");
  Wire1.begin(12);
  Serial.println("wire1 BEGIN");

  updateSeg1(0);
  updateSeg2(0);
//bootAimation();

 Wire1.onReceive(receiveEvent);  // register event
  Wire1.onRequest(requestEvent);

  Serial.println("Adafruit BNO08x test!");
delay(500);
bootAimation();
  // Try to initialize!
  if (!bno08x.begin_I2C()) {
  //if (!bno08x.begin_UART(&Serial1)) {  // Requires a device with > 300 byte UART buffer!
  //if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
    Serial.println("Failed to find BNO08x chip");
    delay(5);
     watchdog_enable(100, 1);
    while (1) {
       delay(10); 
   }
  }
  Serial.println("BNO08x Found!");

  for (int n = 0; n < bno08x.prodIds.numEntries; n++) {
    Serial.print("Part ");
    Serial.print(bno08x.prodIds.entry[n].swPartNumber);
    Serial.print(": Version :");
    Serial.print(bno08x.prodIds.entry[n].swVersionMajor);
    Serial.print(".");
    Serial.print(bno08x.prodIds.entry[n].swVersionMinor);
    Serial.print(".");
    Serial.print(bno08x.prodIds.entry[n].swVersionPatch);
    Serial.print(" Build ");
    Serial.println(bno08x.prodIds.entry[n].swBuildNumber);
  }

  setReports();

  Serial.println("Reading events");
  delay(100);
  for (int i = 0; i < 10; i++) {

    updateSeg1(i);
    updateSeg2(i);
    delay(50);
  }

  updateSeg1(8);
  updateSeg2(8);
delay(500);
if(watchdog_caused_reboot()){

      Serial.println("Watchdog caused last reboot, proceeding.");
    }else{
      Serial.println("Watchdog did not cause last reboot, Reset.");
      watchdog_enable(100, 1);
    while (1) {
       delay(10); 
   }
    }


 updateSeg1(abs(((int)round(sensorValue.un.rotationVector.k*180))%100)/10);
    updateSeg2(abs(((int)round(sensorValue.un.rotationVector.k*180))%10));

    myservo.attach(2); 
}

// Here is where you define the sensor outputs you want to receive
void setReports(void) {
  Serial.println("Setting desired reports");
  if (! bno08x.enableReport(SH2_GAME_ROTATION_VECTOR)) {
    Serial.println("Could not enable game vector");
  }
  if (! bno08x.enableReport(SH2_ROTATION_VECTOR)) {
    Serial.println("Could not enable game vector");
  }
}


void loop() {
  
  steer();

  if (bno08x.wasReset()) {
    Serial.print("sensor was reset ");
    setReports();
  }
  
  if (! bno08x.getSensorEvent(&sensorValue)) {
    return;
  }
  
  switch (sensorValue.sensorId) {
    
    case SH2_GAME_ROTATION_VECTOR:
      // Serial.print("Game Rotation Vector - r: ");
      // Serial.print((sensorValue.un.gameRotationVector.real));
      // Serial.print(" k: ");
      // Serial.print(sensorValue.un.gameRotationVector.k);
      // Serial.print("  ");
  
      // Serial.print(" - r: ");
      // Serial.print((sensorValue.un.gameRotationVector.real)*180);
      // Serial.print(" k: ");
      // Serial.print(sensorValue.un.gameRotationVector.k*180);
      // Serial.print(" Lookup ");
  
      



       lookUpAngle = round(sensorValue.un.rotationVector.k*180);


 // Serial.print(lookUpAngle);
      

if(lookUpAngle >= 0 && sensorValue.un.rotationVector.k<=0.71){
     trueAngle = trueAngleLookup[abs(lookUpAngle%180)];

     int measuredAngle = trueAngleLookup[abs(lookUpAngle%180)];
// Serial.print("  measured ");
// Serial.print(measuredAngle);

    //  Serial.print(" NORM POS: ");
    }else if(lookUpAngle < 0 && sensorValue.un.rotationVector.k>=-0.71){
      int measuredAngle = (-trueAngleLookup[abs(lookUpAngle%180)]);
// Serial.print("  measured ");
// Serial.print(measuredAngle);


       trueAngle = (-trueAngleLookup[abs(lookUpAngle%180)]);
     //  Serial.print(" NORM NEG: ");
    }

       if(sensorValue.un.rotationVector.k>0.71 ){


lookUpAngle = round((1-sensorValue.un.rotationVector.real)*180);

int measuredAngle = trueAngleLookup[abs(180-lookUpAngle%180)];
// Serial.print("  measured ");
// Serial.print(measuredAngle);

if(measuredAngle != 180){
  trueAngle = (180-trueAngleLookup[abs(180-lookUpAngle%180)]);
}else{
  trueAngle = 180;
}
// Serial.print(" REV POS: ");
  
       }


        if(sensorValue.un.rotationVector.k<-0.71 ){
//Serial.print(" REV NEG: ");
lookUpAngle = round((1-sensorValue.un.rotationVector.real)*180);

int measuredAngle = (int)abs(180-lookUpAngle%180);
// Serial.print("  measured ");
// Serial.print(measuredAngle);
if(measuredAngle!=180){
  trueAngle = (-(180-trueAngleLookup[abs(180-lookUpAngle%180)]));
}else{
  trueAngle = -180;
}

  
       }

       displayAngle = trueAngle+trueAngleOffset;
//        Serial.print(" TRUE: ");
//       Serial.print(displayAngle);
//         Serial.print(" : ");
// Serial.print(angleOld+trueAngleOffset);

//       Serial.print(" Offset: ");
//       Serial.print(trueAngleOffset);
     
     


       if(angleOld != trueAngle){

     if(angleOld >= 175 && trueAngle <= -175){

       trueAngleOffset += 360;
       //Serial.println("Applying offset POS");
     }
     if(angleOld <= -175 && trueAngle  >= 175){

      // Serial.println("Applying offset NEG");
       trueAngleOffset -= 360;
     }

    updateSeg1(abs((trueAngle%100)/10));
    updateSeg2(abs(trueAngle%10));
    angleOld = trueAngle;
    
  }

  //Serial.println("");
      break;
  }

}



void steer(){

int angleDiff = (displayAngle-targetAngle);




int maxSteering = 30;
  if (angleDiff > maxSteering) {
    angleDiff = maxSteering;
  } else if (angleDiff < -maxSteering) {
    angleDiff = -maxSteering;
  }

int steeringAngle = angleDiff*2;


int maxSteeringAng = 60;
  if (steeringAngle > maxSteeringAng) {
    steeringAngle = maxSteeringAng;
  } else if (steeringAngle < -maxSteeringAng) {
    steeringAngle = -maxSteeringAng;
  }

steeringAngle +=70;
Serial.print(displayAngle);
Serial.print("    ");
Serial.println(targetAngle);
  myservo.write(steeringAngle);


}



int recieveInt() {
  byte b1 = Wire1.read();
  byte b2 = Wire1.read();

  int returnInt = b1;

  returnInt = returnInt << 8;
  returnInt += b2;

  return returnInt;
}
void sendInt(int sendInt) {
  byte b1 = sendInt >> 8;
  byte b2 = sendInt;
  Wire1.write(b1);
  Wire1.write(b2);
}

void receiveEvent(int len) {
updateSeg1('.');
updateSeg2('.');

  int16_t tatmp = recieveInt();
   targetAngle = tatmp;
  delay(5);
  Serial.print("RECIEVED\t\t\t\t Target angle  =");
  Serial.println(targetAngle);
  //int motor = Wire.read();
  //delay(1000);
}

void requestEvent() {

  sendInt(displayAngle);
}



void bootAimation() {

  for (int i = 0; i < 8; i++) {
    Serial.println(seg1[i]);
    digitalWrite(seg1[i], LOW);
    delay(50);
    digitalWrite(seg1[i], HIGH);
  }

  for (int i = 0; i < 8; i++) {
    Serial.println(seg2[i]);
    digitalWrite(seg2[i], LOW);
    delay(50);
    digitalWrite(seg2[i], HIGH);
  }
}



void updateSeg1(char displayNum) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(seg1[i], HIGH);
  }

  if (displayNum == 0) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[7], LOW);
    digitalWrite(seg1[3], LOW);
    digitalWrite(seg1[4], LOW);
    digitalWrite(seg1[6], LOW);
  } else if (displayNum == 1) {
    digitalWrite(seg1[6], LOW);
    digitalWrite(seg1[7], LOW);
  } else if (displayNum == 2) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[7], LOW);
    digitalWrite(seg1[2], LOW);
    digitalWrite(seg1[3], LOW);
    digitalWrite(seg1[4], LOW);
  } else if (displayNum == 3) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[7], LOW);
    digitalWrite(seg1[2], LOW);
    digitalWrite(seg1[6], LOW);
    digitalWrite(seg1[4], LOW);
  } else if (displayNum == 4) {
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[7], LOW);
    digitalWrite(seg1[2], LOW);
    digitalWrite(seg1[6], LOW);
  } else if (displayNum == 5) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[2], LOW);
    digitalWrite(seg1[6], LOW);
    digitalWrite(seg1[4], LOW);
  } else if (displayNum == 6) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[2], LOW);
    digitalWrite(seg1[3], LOW);
    digitalWrite(seg1[6], LOW);
    digitalWrite(seg1[4], LOW);
  } else if (displayNum == 7) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[7], LOW);
    digitalWrite(seg1[6], LOW);
  } else if (displayNum == 8) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[2], LOW);
    digitalWrite(seg1[3], LOW);
    digitalWrite(seg1[4], LOW);
    digitalWrite(seg1[6], LOW);
    digitalWrite(seg1[7], LOW);
  } else if (displayNum == 9) {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[2], LOW);
    digitalWrite(seg1[7], LOW);
    digitalWrite(seg1[6], LOW);
  } else if (displayNum == 'C') {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[3], LOW);
    digitalWrite(seg1[4], LOW);
 
 } else if (displayNum == 'F') {
    digitalWrite(seg1[0], LOW);
    digitalWrite(seg1[1], LOW);
    digitalWrite(seg1[3], LOW);
    digitalWrite(seg1[2], LOW);
 } else if (displayNum == '.') {
    digitalWrite(seg1[5], LOW);
  }  
}



void updateSeg2(char displayNum) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(seg2[i], HIGH);
  }

  if (displayNum == 0) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[7], LOW);
    digitalWrite(seg2[3], LOW);
    digitalWrite(seg2[4], LOW);
    digitalWrite(seg2[6], LOW);
  } else if (displayNum == 1) {
    digitalWrite(seg2[6], LOW);
    digitalWrite(seg2[7], LOW);
  } else if (displayNum == 2) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[7], LOW);
    digitalWrite(seg2[2], LOW);
    digitalWrite(seg2[3], LOW);
    digitalWrite(seg2[4], LOW);
  } else if (displayNum == 3) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[7], LOW);
    digitalWrite(seg2[2], LOW);
    digitalWrite(seg2[6], LOW);
    digitalWrite(seg2[4], LOW);
  } else if (displayNum == 4) {
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[7], LOW);
    digitalWrite(seg2[2], LOW);
    digitalWrite(seg2[6], LOW);
  } else if (displayNum == 5) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[2], LOW);
    digitalWrite(seg2[6], LOW);
    digitalWrite(seg2[4], LOW);
  } else if (displayNum == 6) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[2], LOW);
    digitalWrite(seg2[3], LOW);
    digitalWrite(seg2[6], LOW);
    digitalWrite(seg2[4], LOW);
  } else if (displayNum == 7) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[7], LOW);
    digitalWrite(seg2[6], LOW);
  } else if (displayNum == 8) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[2], LOW);
    digitalWrite(seg2[3], LOW);
    digitalWrite(seg2[4], LOW);
    digitalWrite(seg2[6], LOW);
    digitalWrite(seg2[7], LOW);
  } else if (displayNum == 9) {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[2], LOW);
    digitalWrite(seg2[7], LOW);
    digitalWrite(seg2[6], LOW);
  } else if (displayNum == 'C') {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[3], LOW);
    digitalWrite(seg2[4], LOW);
 
 } else if (displayNum == 'F') {
    digitalWrite(seg2[0], LOW);
    digitalWrite(seg2[1], LOW);
    digitalWrite(seg2[3], LOW);
    digitalWrite(seg2[2], LOW);
 } else if (displayNum == '.') {
    digitalWrite(seg2[5], LOW);
  }  
}


void initSegs(){

    pinMode(seg1[0], OUTPUT);
  pinMode(seg1[1], OUTPUT);
  pinMode(seg1[2], OUTPUT);
  pinMode(seg1[3], OUTPUT);
  pinMode(seg1[4], OUTPUT);
  pinMode(seg1[5], OUTPUT);
  pinMode(seg1[6], OUTPUT);
  pinMode(seg1[7], OUTPUT);


    pinMode(seg2[0], OUTPUT);
  pinMode(seg2[1], OUTPUT);
  pinMode(seg2[2], OUTPUT);
  pinMode(seg2[3], OUTPUT);
  pinMode(seg2[4], OUTPUT);
  pinMode(seg2[5], OUTPUT);
  pinMode(seg2[6], OUTPUT);
  pinMode(seg2[7], OUTPUT);

   for (int i = 0; i < 8; i++) {
    Serial.println(i);

    digitalWrite(seg1[i], HIGH);
  }
   for (int i = 0; i < 8; i++) {
    Serial.println(i);

    digitalWrite(seg2[i], HIGH);
  }
}