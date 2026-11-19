
#include <Bluepad32.h>

// =====================================
// PINAGEM
// =====================================

#define AIN1 19
#define AIN2 18
#define PWMA 21

#define BIN1 17
#define BIN2 16
#define PWMB 4

// =====================================
// PWM
// =====================================

const int freq = 1000;
const int canalA = 0;
const int canalB = 1;
const int resolucao = 8;

// =====================================
// CONTROLE
// =====================================

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

const int deadzone = 80;
const int limiteCurva = 180;

// =====================================
// CONTROLE DOS MOTORES
// =====================================

void setMotor(int motorA, int motorB) {

  motorA = constrain(motorA, -255, 255);
  motorB = constrain(motorB, -255, 255);

  // Motor A
  if (motorA > 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  }
  else if (motorA < 0) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
  }
  else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
  }

  // Motor B
  if (motorB > 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  }
  else if (motorB < 0) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  }
  else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
  }

  ledcWrite(canalA, abs(motorA));
  ledcWrite(canalB, abs(motorB));
}

// =====================================
// DEADZONE DO ANALOGICO
// =====================================

int aplicarDeadzone(int valor) {

  if (abs(valor) < deadzone) {
    return 0;
  }

  if (valor > 0) {
    return map(valor, deadzone, 512, 0, 512);
  }
  else {
    return map(valor, -deadzone, -512, 0, -512);
  }
}

// =====================================
// PROCESSAMENTO DO CONTROLE
// =====================================

void processGamepad(ControllerPtr ctl) {

  // Analógico esquerdo X
  int x = ctl->axisX();

  x = aplicarDeadzone(x);

  // Normalização da direção
  int direcao = map(x, -512, 512, -255, 255);

  // Gatilhos
  int r2 = ctl->throttle();
  int l2 = ctl->brake();

  // Velocidade base
  int baseSpeed = 0;

  // R2 = frente
  if (r2 > 0) {
    baseSpeed = map(r2, 0, 1023, 0, 255);
  }

  // L2 = ré
  if (l2 > 0) {
    baseSpeed = -map(l2, 0, 1023, 0, 255);
  }

  // =================================
  // MISTURA DIFERENCIAL
  // =================================

  int curva = map(direcao, -255, 255,
                  -limiteCurva, limiteCurva);

  int motorA = baseSpeed;
  int motorB = baseSpeed;

  if (baseSpeed > 0) {

    motorA = baseSpeed - curva;
    motorB = baseSpeed + curva;

  }
  else if (baseSpeed < 0) {

    motorA = baseSpeed + curva;
    motorB = baseSpeed - curva;

  }

  // Limitação
  motorA = constrain(motorA, -255, 255);
  motorB = constrain(motorB, -255, 255);

  // =================================
  // APLICAR MOVIMENTO
  // =================================

  setMotor(motorA, motorB);

  // =================================
  // SERIAL MONITOR
  // =================================

  Serial.print("R2: ");
  Serial.print(r2);

  Serial.print(" | L2: ");
  Serial.print(l2);

  Serial.print(" | X: ");
  Serial.print(x);

  Serial.print(" | Curva: ");
  Serial.print(curva);

  Serial.print(" | Motor A: ");
  Serial.print(motorA);

  Serial.print(" | Motor B: ");
  Serial.println(motorB);
}

// =====================================
// CALLBACKS
// =====================================

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
      setMotor(0, 0);
      break;
    }
  }
}

// =====================================
// SETUP
// =====================================

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

  setMotor(0, 0);

  BP32.setup(
    &onConnectedController,
    &onDisconnectedController
  );

  BP32.forgetBluetoothKeys();

  Serial.println("Sistema iniciado!");
}

// =====================================
// LOOP
// =====================================

void loop() {

  BP32.update();

  bool controleConectado = false;

  for (auto ctl : myControllers) {

    if (ctl && ctl->isConnected()) {

      controleConectado = true;

      if (ctl->hasData()) {
        processGamepad(ctl);
      }

      break;
    }
  }

  // Fail-safe de desconexão
  if (!controleConectado) {
    setMotor(0, 0);
  }

  delay(20);
}
