#include <Arduino.h>
#include <Ethernet.h>  // Include Ethernet library for Ethernet functionality

// Define pins for components
const int morseLedPin = 13;   // Pin for the LED that blinks Morse code
const int errorLedPin = 12;   // Pin for the LED that indicates errors
const int buzzerPin = 11;     // Pin for the buzzer that emits sound for Morse code
const int buttonPin = 7;      // Pin for the button to switch between Ethernet and Serial input

// Ethernet configuration
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address (modify as needed)
IPAddress ip(192, 168, 1, 177); // IP address (modify according to your network)
EthernetServer server(80);      // Create a server on port 80

// Input mode control to switch between Serial and Ethernet modes
enum InputMode { SERIAL_MODE, ETHERNET_MODE }; // Enum to define input modes
InputMode currentMode = SERIAL_MODE; // Start in Serial mode

// Morse code timing variables (adjust these to change Morse timing)
const int dotDuration = 100;     // Duration of a dot in milliseconds; short blip for Morse dot
const int dashDuration = 300;    // Duration of a dash in milliseconds; longer blip for Morse dash
const int symbolPause = 100;     // Pause between symbols within a letter (dot or dash)
const int letterPause = 1000;    // Pause between letters; set to 1 second
const int wordPause = 1500;      // Pause between words; set to 1.5 seconds

// Morse code dictionary for A-Z and 0-9; each string represents the Morse code sequence for that character
String morseCodeTable[36] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", // A-I
    ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", // J-R
    "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",       // S-Z
    "-----", ".----", "..---", "...--", "....-", ".....", "-....", // 0-5
    "--...", "---..", "----."                                     // 6-9
};

// Function to get Morse code for a given character
// Takes a character and returns the corresponding Morse code string
String getMorseCode(char c) {
    // Check if the character is an uppercase letter
    if (c >= 'A' && c <= 'Z') {
        return morseCodeTable[c - 'A']; // Map 'A'-'Z' to Morse code
    }
    // Check if the character is a lowercase letter
    else if (c >= 'a' && c <= 'z') {
        return morseCodeTable[c - 'a']; // Map 'a'-'z' to Morse code
    }
    // Check if the character is a digit
    else if (c >= '0' && c <= '9') {
        return morseCodeTable[c - '0' + 26]; // Map '0'-'9' to Morse code
    }
    else {
        return ""; // Return empty string if character is invalid
    }
}

// Function to blink Morse code on the LED and play sound on the buzzer
// Takes a string representing the Morse code (dots and dashes)
void blinkMorseCode(const String &morseCode) {
    // Loop through each character in the Morse code string
    for (size_t i = 0; i < morseCode.length(); i++) {
        char symbol = morseCode[i]; // Get the current symbol (dot or dash)
        switch (symbol) {
            case '.': // Handle dot
                digitalWrite(morseLedPin, HIGH); // Turn on LED
                tone(buzzerPin, 1000); // Play tone at 1000 Hz for dot
                delay(dotDuration); // Wait for the duration of a dot
                digitalWrite(morseLedPin, LOW); // Turn off LED
                noTone(buzzerPin); // Stop the tone
                break;
            case '-': // Handle dash
                digitalWrite(morseLedPin, HIGH); // Turn on LED
                tone(buzzerPin, 1000); // Play tone at 1000 Hz for dash
                delay(dashDuration); // Wait for the duration of a dash
                digitalWrite(morseLedPin, LOW); // Turn off LED
                noTone(buzzerPin); // Stop the tone
                break;
        }
        delay(symbolPause); // Short pause between symbols within a letter
    }
}

// Function to translate a string into Morse code and blink it
// Takes the input string, converts each character to Morse code, and blinks it
void translateAndBlink(const String &input) {
    // Loop through each character in the input string
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i]; // Get the current character
        String morse = getMorseCode(c); // Get the Morse code for the character
        if (morse != "") { // If the Morse code is valid
            blinkMorseCode(morse); // Blink the Morse code on the LED and buzzer
            delay(letterPause); // Pause between letters
        } 
        else if (c == ' ') { // If the character is a space, indicating a word gap
            tone(buzzerPin, 2000); // Play a higher-pitched tone to indicate a word break
            delay(300); // Play the high pitch tone for 0.3 seconds
            noTone(buzzerPin); // Stop the tone
            delay(wordPause); // Pause between words
        } 
        else { // Handle invalid characters
            Serial.println("Error: Invalid character detected!"); // Print error message to Serial
            digitalWrite(errorLedPin, HIGH); // Turn on error LED
            delay(1000); // Wait for 1 second
            digitalWrite(errorLedPin, LOW); // Turn off error LED
        }
    }
}

// Function to read input from Ethernet
// Reads incoming data from Ethernet client and returns it as a string
String readEthernetInput() {
    EthernetClient client = server.available(); // Listen for incoming clients
    if (client) {
        String input = ""; // Initialize an empty string to store input
        while (client.connected()) {
            if (client.available()) { // Check if data is available from the client
                char c = client.read(); // Read a character from the client
                if (c == '\n') { // Stop reading at newline character
                    client.stop(); // Stop the client connection
                    break;
                }
                input += c; // Append the character to the input string
            }
        }
        return input; // Return the input string
    }
    return ""; // Return empty string if no client is connected
}

// Function to check button press and switch input mode
// Detects button press and toggles between Serial and Ethernet input modes
void checkButton() {
    static bool lastButtonState = HIGH; // Variable to store the last state of the button
    bool currentButtonState = digitalRead(buttonPin); // Read the current state of the button
    if (lastButtonState == HIGH && currentButtonState == LOW) { // Button press detected
        // Toggle the input mode between Serial and Ethernet
        currentMode = (currentMode == SERIAL_MODE) ? ETHERNET_MODE : SERIAL_MODE;
        Serial.print("Switched to ");
        Serial.println(currentMode == SERIAL_MODE ? "Serial Mode" : "Ethernet Mode");
    }
    lastButtonState = currentButtonState; // Update the last state of the button
}

void setup() {
    // Initialize pins as outputs for LED, error LED, and buzzer
    pinMode(morseLedPin, OUTPUT); // Set the Morse code LED pin as output
    pinMode(errorLedPin, OUTPUT); // Set the error LED pin as output
    pinMode(buzzerPin, OUTPUT); // Set the buzzer pin as output
    pinMode(buttonPin, INPUT_PULLUP); // Set the button pin as input with internal pull-up resistor

    Serial.begin(9600); // Start Serial communication at 9600 baud rate
    Ethernet.begin(mac, ip); // Initialize Ethernet with MAC and IP address
    server.begin(); // Start the Ethernet server
    Serial.println("Enter text to convert to Morse code:"); // Prompt user for input
}

void loop() {
    checkButton(); // Check if button was pressed to switch input mode

    String input = ""; // Initialize an empty string to hold user input

    // Handle input based on the current mode (Serial or Ethernet)
    if (currentMode == SERIAL_MODE) {
        if (Serial.available() > 0) { // If data is available on Serial
            input = Serial.readStringUntil('\n'); // Read the string until newline character
            input.trim(); // Remove leading/trailing whitespace from the input
        }
    } 
    else if (currentMode == ETHERNET_MODE) {
        input = readEthernetInput(); // Read input from Ethernet
        input.trim(); // Remove leading/trailing whitespace from the input
    }

    // Check if there is a valid input string
    if (input.length() > 0) {
        translateAndBlink(input); // Translate the input and blink Morse code
        Serial.println("Input processed. Enter new text:"); // Inform user that the input was processed
        digitalWrite(morseLedPin, LOW); // Ensure the Morse LED is turned off
        digitalWrite(errorLedPin, LOW); // Ensure the error LED is turned off
        noTone(buzzerPin); // Ensure the buzzer is off
    } 
    else {
        // If no valid input, ensure all outputs are off and system is idle
        digitalWrite(morseLedPin, LOW); // Ensure the Morse LED is off
        digitalWrite(errorLedPin, LOW); // Ensure the error LED is off
        noTone(buzzerPin); // Ensure the buzzer is off
        delay(100); // Small delay to prevent rapid execution of the loop
    }
}