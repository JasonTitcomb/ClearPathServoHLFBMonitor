#include <Arduino.h>
// This program monitors the HLFB (High Level Feedback) signals from up to four
// ClearPaths motors connected to digital pins 9, 10, 11, and 12.
// It reads the pulse durations to determine the duty cycle of the HLFB signals,
// which indicates whether the motors are active or idle and the % of load.
// Jason Titcomb 2026

float adjFactor[4] = {86.5, 86.5, 86.5, 86.5}; // Per-motor adjustment factors
unsigned int mctValue = 3;                     // Motor count
unsigned long pulseInTimeout = 10000;          // 10 ms
bool running = true;
bool motorFailure = false;

void reportHelp()
{
  Serial.println("Available commands:");
  Serial.println("MCT:<value>      - Set motor count (1-4)");
  Serial.println("ADJ:<v1>[,<v2>,<v3>,<v4>] - Set adjustment factor(s) for each motor (comma-separated floats)");
  Serial.println("TIMEOUT:<value>  - Set pulseIn timeout in us (unsigned long)");
  Serial.println("START            - Start HLFB monitoring");
  Serial.println("STOP             - Stop HLFB monitoring");
  Serial.println("HELP             - Show this help message");
  // Inform pins being monitored
  Serial.println("Monitoring HLFB on pins 9 to " + String(8 + mctValue) + ".");
  Serial.println("Pin 8 HIGH indicates motor failure.");
}

void setup()
{
  Serial.begin(115200);
  pinMode(9, INPUT_PULLUP);
  delay(10);
  pinMode(10, INPUT_PULLUP);
  delay(10);
  pinMode(11, INPUT_PULLUP);
  delay(10);
  pinMode(12, INPUT_PULLUP);
  delay(10);
  pinMode(13, OUTPUT); // On-board LED for status indication
  delay(10);
  pinMode(8, OUTPUT); // Control pin for an external component
  delay(10);
  digitalWrite(8, LOW); // Set pin 8 LOW to enable the component
  reportHelp();
}

void checkMotors(int pin)
{
  // Read the duration of the LOW pulse (since HLFB pulls to GND when active)

  unsigned long activeTime = pulseInLong(pin, LOW, pulseInTimeout);
  unsigned long inactiveTime = pulseInLong(pin, HIGH, pulseInTimeout);

  if (activeTime + inactiveTime > 0)
  {
    unsigned long period = activeTime + inactiveTime;
    int motorIdx = pin - 9;
    float dutyCycle = (float)activeTime / period * adjFactor[motorIdx];
    // should report 0% load at 50% dutyCycle.
    long load = round(2.0 * abs(dutyCycle - 50.0));
    Serial.print(load);
  }
  else
  {
    // If pulseIn returns 0, the motor might be disconnected or static
    bool isStaticActive = digitalRead(pin) == LOW;
    Serial.print(isStaticActive ? "0" : "-1"); // -1 indicates no signal
    if (!motorFailure)
      motorFailure = !isStaticActive;
  }
}

void readCommands()
{
  static char inputBuffer[32];
  static uint8_t idx = 0;
  while (Serial.available() > 0)
  {
    char c = Serial.read();
    if (c == '\n' || c == '\r')
    {
      if (idx > 0)
      {
        inputBuffer[idx] = '\0';
        Serial.println(inputBuffer);
        // Trim leading spaces
        char *cmd = inputBuffer;
        while (*cmd == ' ')
          cmd++;

        if (strncmp(cmd, "MCT:", 4) == 0)
        {
          int value = atoi(cmd + 4);
          Serial.print("Parsed MCT: ");
          Serial.println(value);
          mctValue = value;
        }
        else if (strncmp(cmd, "ADJ:", 4) == 0)
        {
          // Parse comma-separated floats for each motor
          char *vals = cmd + 4;
          int idxA = 0;
          char *token = strtok(vals, ",");
          while (token && idxA < 4)
          {
            float value = atof(token);
            adjFactor[idxA] = value;
            Serial.print("Parsed ADJ for motor ");
            Serial.print(idxA + 1);
            Serial.print(": ");
            Serial.println(value, 4);
            token = strtok(NULL, ",");
            idxA++;
          }
        }
         else if (strncmp(cmd, "TIMEOUT:", 8) == 0)
        {
          unsigned long value = strtoul(cmd + 8, NULL, 10);
          Serial.print("Parsed TIMEOUT: ");
          Serial.println(value);
          pulseInTimeout = value;
        }
        else if (strncmp(cmd, "START", 5) == 0)
        {
          running = true;
          Serial.println("Monitoring started.");
        }
        else if (strncmp(cmd, "STOP", 4) == 0)
        {
          running = false;
          Serial.println("Monitoring stopped.");
        }
        else if (strncmp(cmd, "HELP", 4) == 0)
        {
          reportHelp();
        }
        else
        {
          Serial.println("Unknown command. Type HELP for a list of commands.");
        }
        idx = 0;
        inputBuffer[0] = '\0';
      }
    }
    else
    {
      if (idx < sizeof(inputBuffer) - 1)
      {
        inputBuffer[idx++] = c;
      }
    }
  }
}

void loop()
{
  motorFailure = false;
  if (running)
  {
    digitalWrite(13, HIGH); // Turn on LED to indicate active monitoring
    Serial.print("HLFB:");
    for (int i = 0; i < mctValue; i++)
    {
      int pin = 9 + i;
      checkMotors(pin);
      if (i < mctValue - 1)
        Serial.print(",");
    }
    Serial.println();
    digitalWrite(13, LOW); // Turn off LED to indicate end of monitoring cycle
  }
  else
  {
    digitalWrite(13, LOW); // Ensure LED is off when not monitoring
  }

   readCommands();

  if (motorFailure)
  {
    digitalWrite(8, LOW); // Set pin 8 LOW to indicate motor failure
    motorFailure = false;  // Reset flag after reporting
  }
  else
  {
    digitalWrite(8, HIGH); // Set pin 8 HIGH to indicate no motor failure
  }
}