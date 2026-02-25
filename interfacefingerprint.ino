#include <Adafruit_Fingerprint.h> // Fingerprint sensor library
#include <HardwareSerial.h>       // Extra serial port use karne ke liye

// Serial2 ko use kar rahe hain fingerprint sensor se baat karne ke liye
HardwareSerial fingerSerial(2);  // RX = GPIO17, TX = GPIO16

// Fingerprint sensor ka object banaya
Adafruit_Fingerprint fingerprintSensor = Adafruit_Fingerprint(&fingerSerial);

void setup() {
  Serial.begin(115200);  // Serial monitor ke liye start
  delay(2000);           // Sensor startup ke liye zyada wait
  
  Serial.println("=== Fingerprint System Ready ===");
  // User ko instructions do
  Serial.println("Type 'enroll' to save fingerprint"); // Naya finger save karne ke liye
  Serial.println("Type 'match' to identify fingerprint"); // Finger match karne ke liye
  Serial.println("Type 'delete' to delete a fingerprint"); // Finger delete karne ke liye
  Serial.println("Type 'count' to see stored fingerprints"); // Total fingerprints dekhne ke liye

  // Fingerprint sensor ke liye Serial2 start karo
  fingerSerial.begin(57600, SERIAL_8N1, 17, 16);  // RX=17, TX=16
  
  // Thoda wait karo sensor ke ready hone ke liye
  delay(500);
  
  fingerprintSensor.begin(57600);  // Sensor ko 57600 baud rate se start karo

  // Check karo ki fingerprint sensor sahi se connect hai ya nahi
  if (fingerprintSensor.verifyPassword()) {
    Serial.println(" Fingerprint sensor connected successfully!");
    
    // Sensor ki details print karo
    fingerprintSensor.getParameters();
    Serial.print("Sensor contains "); Serial.print(fingerprintSensor.templateCount); 
    Serial.println(" templates");
  } else {
    Serial.println(" Sensor not found. Wires check karo!");
    Serial.println("Check connections: RX->GPIO17, TX->GPIO16, VCC->3.3V, GND->GND");e
    while (1) {
      delay(1000); // Yahin ruk jao agar sensor nahi mila
    }
  }
  Serial.println("Ready for commands...");
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n'); // User ka command padho
    command.trim(); // Extra space / new line hatao
    command.toLowerCase(); // Case-insensitive banao

    if (command == "enroll") {
      Serial.println("Enter Finger ID (1 to 127):");
      while (!Serial.available()) {
        delay(100); // CPU ko busy wait se bachao
      }
      int id = Serial.parseInt();
      Serial.read(); // Extra newline character clear karo
      
      if (id > 0 && id < 128) {
        Serial.println("Starting fingerprint enrollment...");
        bool success = saveFingerprint(id); // Finger save karo
        if (success) {
          Serial.println("Fingerprint saved successfully!");
        } else {
          Serial.println("Failed to save fingerprint.");
        }
      } else {
        Serial.println(" ID invalid hai. 1 se 127 ke beech do.");
      }
    }

    else if (command == "match") {
      Serial.println("Place finger on scanner...");
      matchFingerprint();
      delay(1000); // Finger match karo
    }

    else if (command == "delete") {
      Serial.println("Enter Finger ID to delete (1 to 127):");
      while (!Serial.available()) {
        delay(100);
      }
      int id = Serial.parseInt();
      Serial.read(); // Extra newline clear karo
      
      if (id > 0 && id < 128) {
        deleteFingerprint(id);
      } else {
        Serial.println(" ID invalid hai. 1 se 127 ke beech do.");
      }
    }

    else if (command == "count") {
      fingerprintSensor.getTemplateCount();
      Serial.print("Total stored fingerprints: ");
      Serial.println(fingerprintSensor.templateCount);
    }

    else {
      Serial.println(" Galat command. Available commands:");
      Serial.println("  - enroll (save fingerprint)");
      Serial.println("  - match (identify fingerprint)");
      Serial.println("  - delete (remove fingerprint)");
      Serial.println("  - count (show total stored)");
    }
  }
}

// Fingerprint save karne ka function
bool saveFingerprint(int id) {
  int p = -1;
  
  // Pehle check karo ki ye ID already used to nahi
  Serial.println("Checking if ID is available...");
  p = fingerprintSensor.loadModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println(" This ID already has a fingerprint! Use 'delete' first.");
    return false;
  }

  Serial.println("Place finger on sensor...");
  while (p != FINGERPRINT_OK) {
    p = fingerprintSensor.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        // Wait karte raho
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return false;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        return false;
      default:
        Serial.println("Unknown error");
        return false;
    }
    delay(50);
  }

  // Convert image to template
  p = fingerprintSensor.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println(" Could not convert image");
    return false;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = fingerprintSensor.getImage();
    delay(50);
  }

  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = fingerprintSensor.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        // Wait karte raho
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return false;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        return false;
      default:
        Serial.println("Unknown error");
        return false;
    }
    delay(50);
  }

  // Convert second image
  p = fingerprintSensor.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println(" Could not convert second image");
    return false;
  }

  // Create model
  p = fingerprintSensor.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return false;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println(" Fingerprints did not match");
    return false;
  } else {
    Serial.println("Unknown error");
    return false;
  }

  // Store model
  p = fingerprintSensor.storeModel(id);
  if (p == FINGERPRINT_OK) {
    return true;
  } else {
    Serial.println(" Could not store model");
    return false;
  }
}

// Fingerprint match karne ka function
void matchFingerprint() {
  int p = fingerprintSensor.getImage();
  
  if (p != FINGERPRINT_OK) {
    if (p == FINGERPRINT_NOFINGER) {
      Serial.println("No finger detected");
    } else {
      Serial.println("Error capturing image");
    }
    return;
  }

  // Convert image to template
  p = fingerprintSensor.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Could not convert image");
    return;
  }

  // Search for match
  p = fingerprintSensor.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Match found!");
    Serial.print("Found ID #"); Serial.print(fingerprintSensor.fingerID);
    Serial.print(" with confidence of "); Serial.println(fingerprintSensor.confidence);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("No match found");
  } else {
    Serial.println("Unknown error");
  }
}

// Fingerprint delete karne ka function
void deleteFingerprint(int id) {
  Serial.print("Deleting ID #");
  Serial.println(id);
  
  int p = fingerprintSensor.deleteModel(id);
  
  if (p == FINGERPRINT_OK) {
    Serial.println(" Deleted successfully!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete from that location");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
  } else {
    Serial.println("Unknown error");
  }
}