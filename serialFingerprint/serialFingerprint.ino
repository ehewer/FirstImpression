/*
#include <I2CTransfer.h>
#include <Packet.h>
#include <PacketCRC.h>
#include <SPITransfer.h>
*/
#include <SerialTransfer.h>
#include <Adafruit_Fingerprint.h>

// SerialTransfer
SerialTransfer myTransfer;

// UNO does not have hardware serial, so use software serial
SoftwareSerial mySerial(2, 3);

// LEDs
const int ledG = 12;
const int ledR = 11;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

void setup() {
  // start serial with Python script
  Serial.begin(115200);
  myTransfer.begin(Serial);

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  // initialize LEDs and signal completion
  pinMode(ledG, OUTPUT);
  pinMode(ledR, OUTPUT);

  if (finger.verifyPassword()) {
    // fingerprint sensor found
    digitalWrite(ledG, HIGH);
    delay(2000);
    digitalWrite(ledG, LOW);
  } else {
    // could not find fingerprint sensor
    digitalWrite(ledR, HIGH);
    delay(2000);
    digitalWrite(ledR, LOW);
    // stall
    while (1) { delay(1); }
  }

  // get sensor parameters
  finger.getParameters();
}

void loop() {
  if(myTransfer.available()) {
    uint16_t recSize = 0;
    uint16_t sendSize = 0;
    uint32_t command;
    
    recSize = myTransfer.rxObj(command, recSize);

    // echo back to Python
    sendSize = myTransfer.txObj(command, sendSize);
    myTransfer.sendData(sendSize);

    id = command;
    while (!getFingerprintEnroll());
  }
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  // Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    /*
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
    */
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      // Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      // Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      // Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      // Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      // Serial.println("Could not find fingerprint features");
      return p;
    default:
      // Serial.println("Unknown error");
      return p;
  }

  // Serial.println("Remove finger");
  digitalWrite(ledG, HIGH);
  delay(2000);
  digitalWrite(ledG, LOW);
  delay(1000);
  
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  // Serial.print("ID "); Serial.println(id);
  p = -1;
  // Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    /*
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
    */
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      // Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      // Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      // Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      // Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      // Serial.println("Could not find fingerprint features");
      return p;
    default:
      // Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  // Serial.print("Creating model for #");  Serial.println(id);
  // Serial.println("Remove finger");
  digitalWrite(ledG, HIGH);
  delay(300);
  digitalWrite(ledG, LOW);
  delay(100);
  digitalWrite(ledG, HIGH);
  delay(300);
  digitalWrite(ledG, LOW);
  delay(1000);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    // Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    // Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    // Serial.println("Fingerprints did not match");
    return p;
  } else {
    // Serial.println("Unknown error");
    return p;
  }

  digitalWrite(ledG, HIGH);
  delay(200);
  digitalWrite(ledG, LOW);
  delay(50);
  digitalWrite(ledG, HIGH);
  delay(200);
  digitalWrite(ledG, LOW);
  delay(50);
  digitalWrite(ledG, HIGH);
  delay(200);
  digitalWrite(ledG, LOW);
  delay(1000);

  // Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    // Serial.println("Stored!");
    digitalWrite(ledG, HIGH);
    digitalWrite(ledR, HIGH);
    delay(2000);
    digitalWrite(ledG, LOW);
    digitalWrite(ledR, LOW);
    delay(1000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    // Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    // Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    // Serial.println("Error writing to flash");
    return p;
  } else {
    // Serial.println("Unknown error");
    return p;
  }

  return true;
}
