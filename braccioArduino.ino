#include "BraccioV2.h"
#include <LiquidCrystal_I2C.h>

// Definieer het Braccio object
Braccio arm;

// Definieer de LCD
LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C adres 0x27, 20 kolommen, 4 rijen

#define MAX_LINES 4
#define MAX_CHARS 20

// Timing voor zeer zachte beweging
unsigned long lastMoveTime = 0;
const unsigned long MOVE_DELAY = 50; // 50ms tussen elke graad = ZEER langzaam

// Globale variabelen
String lines[MAX_LINES];
int currentLine = 0;
String inputLine = "";

// Huidige en target posities
int current_b = 90, current_s = 90, current_e = 45, current_wv = 45, current_wr = 90, current_g = 50;
int target_b = 90, target_s = 90, target_e = 45, target_wv = 45, target_wr = 90, target_g = 50;

bool isMoving = false;

// Functie prototypes
void addLine(String line);
void updateLCD();
void clearLines();
void setTargetPosition(int b, int s, int e, int wv, int wr, int g);
void updateSmoothMovement();


void setup() {
    Serial.begin(9600);

    // 1. BRACCIOV2 INITIALISATIE
    arm.setJointCenter(WRIST_ROT, 90);
    arm.setJointCenter(WRIST, 90);
    arm.setJointCenter(ELBOW, 90);
    arm.setJointCenter(SHOULDER, 90);
    arm.setJointCenter(BASE_ROT, 90);
    arm.setJointCenter(GRIPPER, 50);

    arm.setJointMax(GRIPPER, 100);
    arm.setJointMin(GRIPPER, 15);

    // Start de arm en stel de initiële positie in
    arm.begin(false);
    arm.setAllNow(current_b, current_s, current_e, current_wv, current_wr, current_g);

    // 2. LCD INITIALISATIE
    lcd.init();
    lcd.backlight();

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Serial monitor:");

    clearLines();
    updateLCD();

    lastMoveTime = millis();
}

void loop() {
    // 1. VERWERK SERIËLE DATA
    while (Serial.available()) {
        char c = Serial.read();

        if (c == '\n') {
            String command = inputLine;
            command.trim();

            // Controleer op het MOVE commando
            if (command.startsWith("MOVE")) {
                int b, s, e, wv, wr, g;

                sscanf(command.c_str(), "MOVE %d %d %d %d %d %d", &b, &s, &e, &wv, &wr, &g);

                // Stel de target positie in (beweging gebeurt incrementeel in loop)
                setTargetPosition(b, s, e, wv, wr, g);
            }

            // Log de volledige ingevoerde lijn naar de scroll-buffer
            addLine(command);

            inputLine = "";
            updateLCD();

        } else if (c >= 32 && c <= 126) { // Printable ASCII
            if (inputLine.length() < MAX_CHARS) {
                inputLine += c;
            }
        }
    }

    // 2. UPDATE ZACHTE BEWEGING (incrementeel)
    updateSmoothMovement();

    // 3. UPDATE BRACCIO
    arm.update();
}

// Stel de target positie in
void setTargetPosition(int b, int s, int e, int wv, int wr, int g) {
    target_b = b;
    target_s = s;
    target_e = e;
    target_wv = wv;
    target_wr = wr;
    target_g = g;
    isMoving = true;
}

// Update beweging stap voor stap (1 graad per MOVE_DELAY)
void updateSmoothMovement() {
    if (!isMoving) return;

    unsigned long currentTime = millis();
    if (currentTime - lastMoveTime < MOVE_DELAY) return;

    lastMoveTime = currentTime;

    // Beweeg elk gewricht 1 graad dichter naar de target
    bool stillMoving = false;

    if (current_b < target_b) { current_b++; stillMoving = true; }
    else if (current_b > target_b) { current_b--; stillMoving = true; }

    if (current_s < target_s) { current_s++; stillMoving = true; }
    else if (current_s > target_s) { current_s--; stillMoving = true; }

    if (current_e < target_e) { current_e++; stillMoving = true; }
    else if (current_e > target_e) { current_e--; stillMoving = true; }

    if (current_wv < target_wv) { current_wv++; stillMoving = true; }
    else if (current_wv > target_wv) { current_wv--; stillMoving = true; }

    if (current_wr < target_wr) { current_wr++; stillMoving = true; }
    else if (current_wr > target_wr) { current_wr--; stillMoving = true; }

    if (current_g < target_g) { current_g++; stillMoving = true; }
    else if (current_g > target_g) { current_g--; stillMoving = true; }

    // Update de fysieke positie
    arm.setAllNow(current_b, current_s, current_e, current_wv, current_wr, current_g);

    // Stop als alle gewrichten op de target zijn
    isMoving = stillMoving;
}

// --- Scrolling Functies ---

void addLine(String line) {
    lines[currentLine] = line;
    currentLine = (currentLine + 1) % MAX_LINES;
}

void updateLCD() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Serial monitor:");

    int lineToPrint = (currentLine + 1) % MAX_LINES;
    for (int i = 0; i < MAX_LINES; i++) {
        lcd.setCursor(0, i + 1);
        lcd.print(lines[lineToPrint]);

        int spaces = MAX_CHARS - lines[lineToPrint].length();
        for (int s=0; s<spaces; s++) lcd.print(' ');

        lineToPrint = (lineToPrint + 1) % MAX_LINES;
    }
}

void clearLines() {
    for (int i=0; i<MAX_LINES; i++) {
        lines[i] = "";
    }
}