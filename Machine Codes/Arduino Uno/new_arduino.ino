#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

// Set up I2C LCD (change address if necessary)
LiquidCrystal_I2C lcd(0x27, 16, 2); // Address 0x27, 16 columns and 2 rows
#define ESP32_ADDRESS 0x08  // Define I2C address for ESP32

// Keypad configuration
const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'C'},
    {'7', '8', '9', 'B'}, // 'B' for new user
    {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; // Connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Define pins for ultrasonic sensor
const int trigPin = 11;  // Trigger pin connected to digital pin 11
const int echoPin = 12; // Echo pin connected to digital pin 12

// Define pins for the servo motors
const int servoPin1 = 13; // First servo motor connected to pin 13
const int servoPin2 = A1; // Second servo motor connected to pin A1 (analog pin used as digital)

// Define pins for the IR sensor and button
const int irSensorPin = 10; // Connect IR sensor OUT to digital pin 10
const int buttonPin = A0; // Connect button to analog pin A0

Servo servo1; // Create the first servo object
Servo servo2; // Create the second servo object

// Timing variables
unsigned long lastMeasurementTime = 0;
const unsigned long measurementInterval = 500; // Interval for distance measurement
bool servoActivated = false; // Flag to indicate if servo is activated
unsigned long servoActivationTime = 0;

// Display control variables
bool showUserID = false;
bool userIDEntered = false; // Flag to check if user ID has been entered once
unsigned long displayStartTime = 0;
const unsigned long userDisplayDuration = 5000; // Show user ID for 5 seconds

// Variables for bottle counting
int bottleCount = 0;
bool bottleDetected = false;
String currentUserID = ""; // Variable to hold current user ID

// Function prototypes
void restartProcess();
void sendDataToESP32();
void initializeDisplay();
void handleKeypadInput();
void rotateServos();
void displayUserID(String userID);
int measureDistance();
String generateUniqueID(); // Added prototype for clarity

// Function to initialize the LCD display
void initializeDisplay() {
    lcd.begin(16, 2); // Set up the LCD's number of columns and rows
    lcd.print("Welcome To"); // Print a welcome message
    lcd.setCursor(0, 1);
    lcd.print("PlasteCure");
    delay(3000); // Show the message for 2 seconds
    lcd.clear(); // Clear the display after the welcome message
}

void setup() {
    Serial.begin(9600); // Start serial communication
    Wire.begin();// Set Arduino as I2C master
    lcd.begin(16, 2); // Initialize the LCD with 16 columns and 2 rows
    lcd.backlight(); // Turn on the backlight

    initializeDisplay(); // Display the welcome message

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(irSensorPin, INPUT); // Set IR sensor pin as INPUT
    pinMode(buttonPin, INPUT_PULLUP); // Set button pin as INPUT_PULLUP

    servo1.attach(servoPin1); // Attach the first servo to the specified pin
    servo2.attach(servoPin2); // Attach the second servo to the specified pin
    servo1.write(90); // Start with the servos closed
    servo2.write(90);
    delay(3000); // Delay to stabilize

    randomSeed(analogRead(A2)); // Seed for random unique ID generation
}

void loop() {
    unsigned long currentMillis = millis();

    // Fetch data and reset when button is pressed
    if (digitalRead(buttonPin) == LOW) {
        sendDataToESP32();
        restartProcess();
    }

    // Check if it's time to stop displaying the user ID and switch to distance display
    if (showUserID && (currentMillis - displayStartTime >= userDisplayDuration)) {
        showUserID = false;
        lcd.clear();
    }

    if (!showUserID) {
        // Continuously measure distance and update display if not showing user ID
        if (currentMillis - lastMeasurementTime >= measurementInterval) {
            lastMeasurementTime = currentMillis;
            int BinLevel = measureDistance(); // Measure fill level in percentage

            // Display fill level and bottle count on the LCD
            lcd.clear();
            lcd.print("Bin Level:");
            lcd.print(BinLevel);
            lcd.print("%");
            lcd.setCursor(0, 1);
            lcd.print("Bottles:");
            lcd.print(bottleCount);

            // If fill level is 95% or higher, stop counting bottles and display "Bin Full!"
            if (BinLevel >= 95) {
                lcd.clear();
                lcd.print("Bin Full!");
                delay(2000); // Show message for 2 seconds
                return; // Exit loop early, no further processing
            }
        }

        // Check the IR sensor for bottle detection
        int irValue = digitalRead(irSensorPin);
        
        if (irValue != HIGH && !bottleDetected) {
            delay(300); // Reduced delay for bottle detection
            bottleCount++; // Increase the bottle count
            bottleDetected = true; // Set flag to avoid double counting

            lcd.clear();
            lcd.print("Bottle Count:");
            lcd.setCursor(0, 1);
            lcd.print(bottleCount);
            delay(1500); // Show the count for 1.5 seconds
            
            rotateServos(); // Rotate servos if bin is not full
        }
        if (irValue == HIGH) {
            bottleDetected = false; // Reset the flag when bottle is removed
        }
    }

    // Handle keypad input only if user ID has not been entered before
    if (!userIDEntered) {
        handleKeypadInput();
    }
}

// Function to send bottle count and user ID to ESP32
// Function to send bottle count, user ID, and reward points to ESP32
void sendDataToESP32() {
    int rewardPoints = bottleCount * 5; // Each bottle gives 5 points
    String dataToSend = currentUserID + "," + String(bottleCount) + "," + String(rewardPoints);

    Wire.beginTransmission(ESP32_ADDRESS); // Start transmission to ESP32
    Wire.write(dataToSend.c_str());        // Send string as bytes
    Wire.endTransmission();                // Stop transmission

    Serial.println("Data sent to ESP32 over I2C: " + dataToSend);
}



// Function to rotate both servo motors
void rotateServos() {
    servo1.write(180); // Rotate first servo to open position
    servo2.write(180); // Rotate second servo to open position
    delay(2500); // Wait for 2.5 seconds (adjust time as needed)
    servo1.write(90); // Rotate first servo back to closed position
    servo2.write(90); // Rotate second servo back to closed position
}

// Function to handle keypad input
void handleKeypadInput() {
    lcd.clear();
    lcd.print("Enter 5 digit ID:");
    String userInput = "";
    
    while (true) {
        char key = keypad.getKey(); // Check for key press
        if (key) { // If a key is pressed
            if (key == 'A') {
                // User presses 'A' to submit the input
                break; // Exit the input loop
            } else if (key == 'B') {
                // Clear the input if 'B' is pressed
                userInput = "";
                lcd.clear();
                lcd.print("Enter 5 digit ID:");
            } else if (key == 'C') {
                // Generate and display the unique ID for new user
                String uniqueID = generateUniqueID();
                displayUserID(uniqueID); // Show new user ID
                currentUserID = uniqueID; // Store the unique ID
                userIDEntered = true; // Set flag to avoid re-entering
                bottleCount = 0; // Reset bottle count for new user
                return;
            } else {
                // Add the pressed key to user input
                userInput += key; 
                lcd.setCursor(0, 1); // Move to the second line
                lcd.print(userInput); // Display current input
            }
        }
    }

    // After the input is submitted, show entered ID
    currentUserID = userInput; // Store the entered user ID
    displayUserID(currentUserID);
    userIDEntered = true; // Set flag to avoid re-entering

    delay(1000); // Small delay before the next loop iteration  
}

// Function to display user ID and set timer for distance display
void displayUserID(String userID) {
    lcd.clear();
    lcd.print("Your ID is:");
    lcd.setCursor(0, 1);
    lcd.print(userID); // Display the entered ID or generated ID

    showUserID = true;
    displayStartTime = millis(); // Start the 5-second timer
}

// Function to generate random unique 5-digit ID
String generateUniqueID() {
    String id = "";
    for (int i = 0; i < 5; i++) {
        id += String(random(0, 10)); // Generate a random digit and append
    }
    return id;
}

// Function to measure distance (in cm) and return fill level as percentage
int measureDistance() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    int distance = duration * 0.034 / 2; // Convert to centimeters

    // Convert distance to fill level percentage
    int BinLevel = map(distance, 0, 100, 100, 0); // Adjust for your bin depth
    return constrain(BinLevel, 0, 100); // Ensure fill level is between 0 and 100
}

// Function to reset process for the next user
void restartProcess() {
    bottleCount = 0; // Reset bottle count
    currentUserID = ""; // Clear user ID
    userIDEntered = false; // Allow new user ID entry
    showUserID = false; // Stop showing ID
    lcd.clear(); // Clear the display
    lcd.print("Welcome To");
    lcd.setCursor(0, 1);
    lcd.print("PlasteCure");
    delay(3000); // Show message for 2 seconds
}
