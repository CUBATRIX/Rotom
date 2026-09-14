#include <Bluepad32.h>

// ==========================
// PINAGEM (igual à sua imagem)
// ==========================
#define AIN1 19
#define AIN2 18
#define PWMA 32

#define BIN1 17   // TX2
#define BIN2 16   // RX2
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
// FUNÇÕES DOS MOTORES
// ==========================
void setMotorA(int velocidade) {
  Serial.print("Motor A: ");

  if (velocidade > 0) {
    Serial.print("FRENTE ");
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } 
  else if (velocidade < 0) {
    Serial.print("TRAS ");
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
  } 
  else {
    Serial.print("PARADO ");
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
  }

  Serial.println(abs(velocidade));
  ledcWrite(canalA, abs(velocidade));
}

void setMotorB(int velocidade) {
  Serial.print("Motor B: ");

  if (velocidade > 0) {
    Serial.print("FRENTE ");
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } 
  else if (velocidade < 0) {
    Serial.print("TRAS ");
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  } 
  else {
    Serial.print("PARADO ");
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
  }

  Serial.println(abs(velocidade));
  ledcWrite(canalB, abs(velocidade));
}

// ==========================
// CONTROLE (R2 + L2 + ANALÓGICO)
// ==========================
void processGamepad(ControllerPtr ctl) {

  Serial.println("\n==== CONTROLE ====");

  // Gatilhos
  int acelerador = ctl->throttle(); // R2
  int re = ctl->brake();            // L2

  // Analógico (curva)
  int x = ctl->axisX(); // -512 a 512

  // Deadzone
  if (abs(x) < 50) x = 0;

  // ==========================
  // VELOCIDADE BASE
  // ==========================
  int base = 0;

  if (acelerador > 50) {
    base = map(acelerador, 0, 1023, 0, 255);
    Serial.print("FRENTE | Velocidade: ");
    Serial.println(base);
  } 
  else if (re > 50) {
    base = -map(re, 0, 1023, 0, 255);
    Serial.print("RE | Velocidade: ");
    Serial.println(base);
  } 
  else {
    Serial.println("PARADO");
    base = 0;
  }

  // ==========================
  // CURVA
  // ==========================
  int curva = map(x, -512, 512, -150, 150);

  Serial.print("Curva: ");
  Serial.println(curva);

  // ==========================
  // DIFERENCIAL
  // ==========================
  int motorA = base + curva;
  int motorB = base - curva;

  motorA = constrain(motorA, -255, 255);
  motorB = constrain(motorB, -255, 255);

  Serial.print("Motor A final: ");
  Serial.print(motorA);
  Serial.print(" | Motor B final: ");
  Serial.println(motorB);

  // Aplica nos motores
  setMotorA(motorA);
  setMotorB(motorB);
}

// ==========================
// CALLBACKS
// ==========================
void onConnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.println("Controle conectado!");
      myControllers[i] = ctl;
      break;
    }
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.println("Controle desconectado!");
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

  // PWM A
  ledcSetup(canalA, freq, resolucao);
  ledcAttachPin(PWMA, canalA);

  // PWM B
  ledcSetup(canalB, freq, resolucao);
  ledcAttachPin(PWMB, canalB);

  // Bluetooth
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();

  Serial.println("Sistema iniciado!");
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

  delay(100);
}