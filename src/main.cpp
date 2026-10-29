#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>
#include <Wire.h>

Servo servo270;
Servo servo360;

const uint8_t SCREEN_WIDTH = 128;
const uint8_t SCREEN_HEIGHT = 64;
const int8_t OLED_RESET = -1;
const uint8_t OLED_ADDRESS = 0x3C;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const uint8_t SERVO_270_PIN = 12;
const uint8_t SERVO_360_PIN = 13;
const uint8_t JOYSTICK_I2C_ADDRESS = 0x39;

const uint8_t REG_BUTTON = 0x31;
const uint8_t REG_Y_HIGH = 0x34;
const uint8_t REG_Y_LOW = 0x35;

const int SERVO_270_MIN = 30;
const int SERVO_270_MAX = 150;
const int JOYSTICK_DEFAULT_CENTER = 512;
const int JOYSTICK_DEADZONE = 100;
const int SERVO_360_START_THRESHOLD = 160;
const int SERVO_360_STOP_THRESHOLD = 110;
const unsigned long DEBOUNCE_MS = 60;
const unsigned long ERROR_BLINK_MS = 150;
const unsigned long REPORT_INTERVAL_MS = 500;
const unsigned long DISPLAY_INTERVAL_MS = 250;
const uint8_t CALIBRATION_SAMPLES = 32;
const uint8_t CENTER_CONFIRM_SAMPLES = 5;
const uint8_t MOTION_CONFIRM_SAMPLES = 3;

bool selected360 = false;
bool stablePressed = false;
bool lastPressedReading = false;
bool buttonArmed = false;
bool joystickAvailable = false;
bool availabilityReported = false;
bool previousAvailability = false;
bool servo360Attached = false;
bool servo360Ready = false;
bool servo360Moving = false;
bool displayReady = false;
int joystickCenterY = JOYSTICK_DEFAULT_CENTER;
int servo270Angle = 90;
uint8_t centeredSamples = 0;
uint8_t motionSamples = 0;
int8_t pending360Direction = 0;
unsigned long buttonChangedAt = 0;
unsigned long servo270StepAt = 0;
unsigned long errorLedChangedAt = 0;
unsigned long lastReportAt = 0;
unsigned long lastDisplayAt = 0;
bool errorLedState = false;

bool readRegister(uint8_t reg, uint8_t &value) {
  Wire.beginTransmission(JOYSTICK_I2C_ADDRESS);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  if (Wire.requestFrom(JOYSTICK_I2C_ADDRESS, (uint8_t)1) != 1) {
    return false;
  }

  value = Wire.read();
  return true;
}

bool readAxis(uint8_t highRegister, uint8_t lowRegister, uint16_t &value) {
  uint8_t highByte;
  uint8_t lowByte;
  if (!readRegister(highRegister, highByte) ||
      !readRegister(lowRegister, lowByte)) {
    return false;
  }

  value = (((uint16_t)highByte << 2) | (lowByte >> 6)) & 0x03FF;
  return true;
}

void updateDisplay(bool joystickOk, uint16_t yValue,
                   const __FlashStringHelper *movementStatus) {
  if (!displayReady) {
    return;
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("KidsUno - Joystick"));
  display.println();
  display.print(F("Joystick: "));
  display.println(joystickOk ? F("OK") : F("ERROR I2C"));
  display.print(F("Y: "));
  if (joystickOk) {
    display.println(yValue);
  } else {
    display.println(F("---"));
  }
  display.print(F("Control: Servo "));
  display.println(selected360 ? F("360") : F("270"));
  display.println(movementStatus);
  display.print(F("Angulo 270: "));
  display.println(servo270Angle);
  display.display();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(9600);
  Wire.begin();

  displayReady = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  if (!displayReady) {
    Serial.println(F("ERROR: no se pudo iniciar OLED SSD1306 en 0x3C."));
  } else {
    display.setRotation(2);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("KidsUno iniciando..."));
    display.display();
  }

  servo270.attach(SERVO_270_PIN);
  servo270.write(servo270Angle);

  Serial.println(F("Joystick Kidsbits I2C (direccion 0x39)"));
  Serial.println(F("Deja el joystick centrado durante el arranque para calibrarlo."));
  Serial.println(F("Mueve arriba/abajo; pulsa el joystick para cambiar de servo."));
  Serial.println(F("LED apagado: servo 270 en D12; LED encendido: servo 360 en D13."));

  uint32_t centerTotal = 0;
  uint8_t validSamples = 0;
  for (uint8_t sample = 0; sample < CALIBRATION_SAMPLES; ++sample) {
    uint16_t yValue;
    if (readAxis(REG_Y_HIGH, REG_Y_LOW, yValue)) {
      centerTotal += yValue;
      ++validSamples;
    }
    delay(10);
  }
  if (validSamples > 0) {
    joystickCenterY = centerTotal / validSamples;
    Serial.print(F("Centro Y calibrado: "));
    Serial.println(joystickCenterY);
  } else {
    Serial.println(F("No se pudo calibrar; revisa que el joystick este en el puerto I2C."));
  }
}

void loop() {
  const unsigned long now = millis();
  uint8_t buttonValue;
  uint16_t yValue;

  joystickAvailable = readRegister(REG_BUTTON, buttonValue) &&
                      buttonValue <= 1 &&
                      readAxis(REG_Y_HIGH, REG_Y_LOW, yValue);

  if (!availabilityReported || joystickAvailable != previousAvailability) {
    availabilityReported = true;
    previousAvailability = joystickAvailable;
    Serial.println(joystickAvailable
                       ? F("Joystick I2C detectado.")
                       : F("ERROR: no responde el joystick en I2C 0x39; revisa el puerto SDA/SCL."));
  }

  if (!joystickAvailable) {
    if (servo360Attached) {
      servo360.detach();
      servo360Attached = false;
    }
    servo360Ready = false;
    centeredSamples = 0;
    if (now - errorLedChangedAt >= ERROR_BLINK_MS) {
      errorLedChangedAt = now;
      errorLedState = !errorLedState;
      digitalWrite(LED_BUILTIN, errorLedState ? HIGH : LOW);
    }
    if (now - lastDisplayAt >= DISPLAY_INTERVAL_MS) {
      lastDisplayAt = now;
      updateDisplay(false, 0, F("Revisa joystick"));
    }
    return;
  }

  const bool pressedReading = buttonValue != 0;
  if (pressedReading != lastPressedReading) {
    lastPressedReading = pressedReading;
    buttonChangedAt = now;
  }

  if (now - buttonChangedAt >= DEBOUNCE_MS &&
      pressedReading != stablePressed) {
    stablePressed = pressedReading;
    if (!stablePressed) {
      buttonArmed = true;
    } else if (buttonArmed) {
      buttonArmed = false;
      selected360 = !selected360;
      servo360Ready = false;
      servo360Moving = false;
      motionSamples = 0;
      pending360Direction = 0;
      centeredSamples = 0;
      if (selected360) {
        if (servo360Attached) {
          servo360.detach();
          servo360Attached = false;
        }
      } else if (servo360Attached) {
        servo360.detach();
        servo360Attached = false;
      }
      Serial.print(F("Controlando servo "));
      Serial.println(selected360 ? F("360 (D13)") : F("270 (D12)"));
    }
  }

  digitalWrite(LED_BUILTIN, selected360 ? HIGH : LOW);
  const int yPosition = (int)yValue;
  const __FlashStringHelper *movementStatus = F("Detenido");

  if (selected360) {
    const int distanceFromCenter = abs(yPosition - joystickCenterY);
    if (!servo360Ready) {
      movementStatus = F("Centra joystick");
      if (servo360Attached) {
        servo360.detach();
        servo360Attached = false;
      }
      servo360Moving = false;
      if (distanceFromCenter <= SERVO_360_STOP_THRESHOLD) {
        if (centeredSamples < CENTER_CONFIRM_SAMPLES) {
          ++centeredSamples;
        }
        if (centeredSamples >= CENTER_CONFIRM_SAMPLES) {
          servo360Ready = true;
          Serial.println(F("Joystick centrado: servo 360 listo."));
        }
      } else {
        centeredSamples = 0;
      }
    } else {
      int8_t requestedDirection = 0;
      if (yPosition < joystickCenterY - SERVO_360_START_THRESHOLD) {
        requestedDirection = -1;
      } else if (yPosition > joystickCenterY + SERVO_360_START_THRESHOLD) {
        requestedDirection = 1;
      }

      if (servo360Moving && distanceFromCenter <= SERVO_360_STOP_THRESHOLD) {
        if (servo360Attached) {
          servo360.detach();
          servo360Attached = false;
        }
        servo360Moving = false;
        motionSamples = 0;
        pending360Direction = 0;
      } else if (servo360Moving && requestedDirection != 0 &&
                 requestedDirection != pending360Direction) {
        if (servo360Attached) {
          servo360.detach();
          servo360Attached = false;
        }
        servo360Moving = false;
        pending360Direction = requestedDirection;
        motionSamples = 1;
      } else if (!servo360Moving && requestedDirection != 0) {
        if (requestedDirection != pending360Direction) {
          pending360Direction = requestedDirection;
          motionSamples = 1;
        } else if (motionSamples < MOTION_CONFIRM_SAMPLES) {
          ++motionSamples;
        }

        if (motionSamples >= MOTION_CONFIRM_SAMPLES) {
          servo360Moving = true;
          if (!servo360Attached) {
            servo360.attach(SERVO_360_PIN);
            servo360Attached = true;
          }
        }
      } else if (!servo360Moving) {
        motionSamples = 0;
        pending360Direction = 0;
      }

      if (servo360Moving) {
        if (pending360Direction < 0) {
          const int pulseUs = map(constrain(yPosition, 0,
                                            joystickCenterY - SERVO_360_STOP_THRESHOLD),
                                  0, joystickCenterY - SERVO_360_STOP_THRESHOLD,
                                  1100, 1450);
          servo360.writeMicroseconds(pulseUs);
          movementStatus = F("Subiendo");
        } else {
          const int pulseUs = map(constrain(yPosition,
                                            joystickCenterY + SERVO_360_STOP_THRESHOLD,
                                            1023),
                                  joystickCenterY + SERVO_360_STOP_THRESHOLD,
                                  1023, 1550, 1900);
          servo360.writeMicroseconds(pulseUs);
          movementStatus = F("Bajando");
        }
      } else {
        movementStatus = F("Detenido");
      }
    }
  } else {
    if (servo360Attached) {
      servo360.detach();
      servo360Attached = false;
    }
    servo360Moving = false;
    motionSamples = 0;
    pending360Direction = 0;

    const int distanceFromCenter = abs(yPosition - joystickCenterY);
    if (distanceFromCenter > JOYSTICK_DEADZONE) {
      movementStatus =
          yPosition < joystickCenterY - JOYSTICK_DEADZONE
              ? F("Moviendo izquierda")
              : F("Moviendo derecha");
      const unsigned long stepInterval =
          map(constrain(distanceFromCenter - JOYSTICK_DEADZONE, 0, 426),
              0, 426, 100, 20);
      if (now - servo270StepAt >= stepInterval) {
        servo270StepAt = now;
        if (yPosition < joystickCenterY - JOYSTICK_DEADZONE &&
            servo270Angle < SERVO_270_MAX) {
          ++servo270Angle;
          servo270.write(servo270Angle);
        } else if (yPosition > joystickCenterY + JOYSTICK_DEADZONE &&
                   servo270Angle > SERVO_270_MIN) {
          --servo270Angle;
          servo270.write(servo270Angle);
        }
      }
    }
  }

  if (now - lastReportAt >= REPORT_INTERVAL_MS) {
    lastReportAt = now;
    Serial.print(F("Y="));
    Serial.print(yValue);
    Serial.print(F(" Centro="));
    Serial.print(joystickCenterY);
    Serial.print(F("  Servo seleccionado="));
    Serial.print(selected360 ? F("360") : F("270"));
    Serial.print(F("  Movimiento="));
    Serial.print(movementStatus);
    Serial.print(F("  Angulo 270="));
    Serial.println(servo270Angle);
  }

  if (now - lastDisplayAt >= DISPLAY_INTERVAL_MS) {
    lastDisplayAt = now;
    updateDisplay(true, yValue, movementStatus);
  }
}
