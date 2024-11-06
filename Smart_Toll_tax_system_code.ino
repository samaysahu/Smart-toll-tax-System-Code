#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER 2 // Buzzer pin

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
Servo myServo; // Define servo name

// Initialize the LCD (I2C address 0x27, 16 chars, 2 lines)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  myServo.attach(6);    // Servo pin
  myServo.write(0);     // Servo start position
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);

  // Initialize the LCD
  lcd.init();
  lcd.clear();
  lcd.backlight();    // Turn on the LCD backlight

  // Print initial message on the LCD
  lcd.setCursor(0, 0);
  lcd.print("  Put card on   ");
  lcd.setCursor(0, 1);
  lcd.print("     reader     ");
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Clear the LCD before displaying new data
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UID tag:");

  // Show UID on the LCD
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    lcd.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  // Convert content to uppercase for comparison
  content.toUpperCase();

  lcd.setCursor(0, 1);  // Move to the second line of the LCD
  // Check if the card UID matches the authorized UID
  if (content.substring(1) == "53 37 5A E4") { // Replace with the UID of your authorized card
    lcd.print("Access Granted");
    tone(BUZZER, 500);          // Sound the buzzer
    delay(300);
    noTone(BUZZER);
    myServo.write(90);          // Move the servo to open
    delay(5000);                // Wait for 5 seconds
    myServo.write(0);           // Move the servo back to the closed position
  } else {
    lcd.print("Access Denied");
    tone(BUZZER, 300);          // Sound the buzzer
    delay(2000);
    noTone(BUZZER);
  }
  delay(2000);  // Wait 2 seconds before the next scan
}
