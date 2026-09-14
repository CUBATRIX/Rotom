#include <Bluepad32.h>

// ==========================
// PINAGEM
// ==========================
#define AIN1 19
#define AIN2 18
#define PWMA 32

#define BIN1 17
#define BIN2 16
#define PWMB 33

// ==========================
// PWM
// ==========================
const int freq = 1000;
const int canalA = 0;
const int canalB = 1;
const int resolucao = 8;

// ==========================
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// ==========================
// MOTOR
// ==========================
void setMotor(int motorA, int motorB) {

  // Motor A
  if (motorA > 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else if (motorA < 0) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
  }

  // Motor B
  if (motorB > 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } else if (motorB < 0) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
  }

  ledcWrite(canalA, abs(motorA));
  ledcWrite(canalB, abs(motorB));

  // Debug
  Serial.print("MotorA: "); Serial.print(motorA);
  Serial.print(" | MotorB: "); Serial.println(motorB);
}

// ==========================
// CONTROLE
// ==========================
void processGamepad(ControllerPtr ctl) {

  int baseSpeed = 0;
  int x = ctl->axisX();

  // ==========================
  // DEADZONE
  // ==========================
  int deadzone = 80;

  if (abs(x) < deadzone) {
    x = 0;
  } else {
    if (x > 0) {
      x = map(x, deadzone, 512, 0, 512);
    } else {
      x = map(x, -deadzone, -512, 0, -512);
    }
  }

  // ==========================
  // ACELERAÇÃO
  // ==========================
  if (ctl->throttle() > 100) {  // R2
    baseSpeed = 255;
    Serial.println("FRENTE (R2)");
  } 
  else if (ctl->brake() > 100) { // L2
    baseSpeed = -255;
    Serial.println("TRAS (L2)");
  } 
  else {
    setMotor(0, 0);
    return;
  }

  // ==========================
  // CURVA
  // ==========================
  int curva = map(x, -512, 512, -150, 150);

  int motorA, motorB;

  if (baseSpeed > 0) {
    // FRENTE
    motorA = baseSpeed - curva;
    motorB = baseSpeed + curva;
  } else {
    // TRÁS (corrigido)
    motorA = baseSpeed + curva;
    motorB = baseSpeed - curva;
  }

  // Limite
  motorA = constrain(motorA, -255, 255);
  motorB = constrain(motorB, -255, 255);

  // Evita motor "morto" sem querer
  if (motorA == 0) motorA = baseSpeed > 0 ? 80 : -80;
  if (motorB == 0) motorB = baseSpeed > 0 ? 80 : -80;

  // Debug
  Serial.print("X: "); Serial.print(x);
  Serial.print(" | Curva: "); Serial.print(curva);
  Serial.print(" | Base: "); Serial.print(baseSpeed);
  Serial.print(" | A: "); Serial.print(motorA);
  Serial.print(" | B: "); Serial.println(motorB);

  setMotor(motorA, motorB);
}

// ==========================
// CALLBACKS
// ==========================
void onConnectedController(ControllerPtr ctl) {
  Serial.println("Controle conectado!");
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      myControllers[i] = ctl;
      break;
    }
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  Serial.println("Controle desconectado!");
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      myControllers[i] = nullptr;
      break;
    }
  }
}

// ==========================
// SETUP
// ==========================
void setup() {
  Serial.begin(115200);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  ledcSetup(canalA, freq, resolucao);
  ledcAttachPin(PWMA, canalA);

  ledcSetup(canalB, freq, resolucao);
  ledcAttachPin(PWMB, canalB);

  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();

  Serial.println("Sistema pronto!");
}

// ==========================
// LOOP
// ==========================
void loop() {
  if (BP32.update()) {
    for (auto ctl : myControllers) {
      if (ctl && ctl->isConnected()) {
        processGamepad(ctl);
      }
    }
  }

  delay(30);
}