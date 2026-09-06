#include <Bluepad32.h>

// ==========================
// PINAGEM DA PONTE H
// ==========================

// Motor A
#define AIN1 19
#define AIN2 18
#define PWMA 32

// Motor B
#define BIN1 17   // TX2
#define BIN2 16   // RX2
#define PWMB 33


// ==========================
// CONFIGURAÇÃO PWM
// ==========================

const int freq = 1000;
const int canalA = 0;
const int canalB = 1;
const int resolucao = 8;


// ==========================
// CONFIGURAÇÕES DO CONTROLE
// ==========================

const int DEADZONE = 100;

// Velocidade mínima e máxima
const int VELOCIDADE_MIN = 80;
const int VELOCIDADE_MAX = 255;


// ==========================
// CONTROLES BLUETOOTH
// ==========================

ControllerPtr myControllers[BP32_MAX_GAMEPADS];


// =================================================
// FUNÇÃO PARA CONTROLAR MOTOR A
// velocidade:
// valor positivo = frente
// valor negativo = trás
// zero = parado
// =================================================

void setMotorA(int velocidade) {

  velocidade = constrain(velocidade, -255, 255);

  if (velocidade > 0) {

    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);

  }
  else if (velocidade < 0) {

    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);

  }
  else {

    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);

  }

  ledcWrite(canalA, abs(velocidade));
}


// =================================================
// FUNÇÃO PARA CONTROLAR MOTOR B
// =================================================

void setMotorB(int velocidade) {

  velocidade = constrain(velocidade, -255, 255);

  if (velocidade > 0) {

    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);

  }
  else if (velocidade < 0) {

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);

  }
  else {

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);

  }

  ledcWrite(canalB, abs(velocidade));
}


// =================================================
// PARAR TODOS OS MOTORES
// =================================================

void pararMotores() {

  setMotorA(0);
  setMotorB(0);

}


// =================================================
// PROCESSAMENTO DO CONTROLE
// =================================================

void processGamepad(ControllerPtr ctl) {

  // ==========================================
  // LEITURA DOS JOYSTICKS
  // ==========================================

  // Joystick esquerdo
  int joystickEsquerdo = ctl->axisY();

  // Joystick direito
  int joystickDireito = ctl->axisRY();


  // ==========================================
  // LEITURA DOS GATILHOS
  // ==========================================

  // L2
  int L2 = ctl->brake();

  // R2
  int R2 = ctl->throttle();


  // ==========================================
  // DEFINIÇÃO DA VELOCIDADE
  // ==========================================

  /*
     Os gatilhos normalmente retornam
     valores entre 0 e 1023.

     R2 aumenta a velocidade.
     L2 diminui a velocidade.
  */

  int velocidadeBase = VELOCIDADE_MIN;

  // R2 aumenta a velocidade
  velocidadeBase += map(
    R2,
    0,
    1023,
    0,
    VELOCIDADE_MAX - VELOCIDADE_MIN
  );

  // L2 reduz a velocidade
  velocidadeBase -= map(
    L2,
    0,
    1023,
    0,
    VELOCIDADE_MIN
  );

  velocidadeBase = constrain(
    velocidadeBase,
    0,
    VELOCIDADE_MAX
  );


  // ==========================================
  // DEADZONE
  // Evita que o robô se mova sozinho
  // ==========================================

  if (abs(joystickEsquerdo) < DEADZONE) {
    joystickEsquerdo = 0;
  }

  if (abs(joystickDireito) < DEADZONE) {
    joystickDireito = 0;
  }


  // ==========================================
  // CONVERTE JOYSTICK PARA VELOCIDADE
  // ==========================================

  /*
     Bluepad32 normalmente trabalha com
     valores aproximados entre -512 e 512.
  */

  int motorA = map(
    joystickEsquerdo,
    -512,
    512,
    velocidadeBase,
    -velocidadeBase
  );

  int motorB = map(
    joystickDireito,
    -512,
    512,
    velocidadeBase,
    -velocidadeBase
  );


  // Garante os limites
  motorA = constrain(
    motorA,
    -velocidadeBase,
    velocidadeBase
  );

  motorB = constrain(
    motorB,
    -velocidadeBase,
    velocidadeBase
  );


  // ==========================================
  // CONTROLE DOS MOTORES
  // ==========================================

  setMotorA(motorA);

  setMotorB(motorB);


  // ==========================================
  // SERIAL MONITOR
  // ==========================================

  Serial.println("=================================");

  Serial.print("Joystick Esquerdo: ");
  Serial.println(joystickEsquerdo);

  Serial.print("Joystick Direito: ");
  Serial.println(joystickDireito);

  Serial.print("L2: ");
  Serial.println(L2);

  Serial.print("R2: ");
  Serial.println(R2);

  Serial.print("Velocidade Base: ");
  Serial.println(velocidadeBase);

  Serial.print("Motor A: ");
  Serial.println(motorA);

  Serial.print("Motor B: ");
  Serial.println(motorB);

}


// =================================================
// CONTROLE CONECTADO
// =================================================

void onConnectedController(ControllerPtr ctl) {

  bool foundEmptySlot = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {

    if (myControllers[i] == nullptr) {

      Serial.println("Controle conectado!");

      myControllers[i] = ctl;

      foundEmptySlot = true;

      break;
    }
  }

  if (!foundEmptySlot) {

    Serial.println(
      "Controle conectado, mas nao ha espaco!"
    );

  }
}


// =================================================
// CONTROLE DESCONECTADO
// =================================================

void onDisconnectedController(ControllerPtr ctl) {

  bool foundController = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {

    if (myControllers[i] == ctl) {

      Serial.println("Controle desconectado!");

      myControllers[i] = nullptr;

      foundController = true;

      break;
    }
  }


  // Segurança: para os motores
  pararMotores();


  if (!foundController) {

    Serial.println(
      "Controle desconectado nao encontrado!"
    );

  }
}


// =================================================
// SETUP
// =================================================

void setup() {

  Serial.begin(115200);

  Serial.println();
  Serial.println("=================================");
  Serial.println("ROBO SUMO - INICIANDO");
  Serial.println("=================================");


  // --------------------------------
  // PINOS DE DIREÇÃO
  // --------------------------------

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);


  // --------------------------------
  // CONFIGURAÇÃO PWM MOTOR A
  // --------------------------------

  ledcSetup(
    canalA,
    freq,
    resolucao
  );

  ledcAttachPin(
    PWMA,
    canalA
  );


  // --------------------------------
  // CONFIGURAÇÃO PWM MOTOR B
  // --------------------------------

  ledcSetup(
    canalB,
    freq,
    resolucao
  );

  ledcAttachPin(
    PWMB,
    canalB
  );


  // --------------------------------
  // INICIA COM MOTORES PARADOS
  // --------------------------------

  pararMotores();


  // --------------------------------
  // BLUETOOTH
  // --------------------------------

  BP32.setup(
    &onConnectedController,
    &onDisconnectedController
  );


  // Remove pareamentos antigos
  // Caso queira manter os pareamentos,
  // comente esta linha.
  BP32.forgetBluetoothKeys();


  Serial.println("Bluetooth iniciado!");
  Serial.println("Aguardando controle PS5...");
}


// =================================================
// LOOP PRINCIPAL
// =================================================

void loop() {

  if (BP32.update()) {

    for (auto ctl : myControllers) {

      if (ctl && ctl->isConnected()) {

        processGamepad(ctl);

      }

    }

  }

  delay(50);

}