#include <Bluepad32.h>

// =====================================================
// PINAGEM ESP32 -> PONTE H
// =====================================================

// MOTOR ESQUERDO
#define IN1 19
#define IN2 18

// MOTOR DIREITO
#define IN3 17
#define IN4 16

// =====================================================
// CONFIGURAÇÕES
// =====================================================

// Coloque true se o motor estiver girando ao contrário
// da direção desejada.
const bool INVERTER_MOTOR_ESQUERDO = false;
const bool INVERTER_MOTOR_DIREITO  = false;

// Zona morta do analógico
const int DEADZONE = 80;

// =====================================================
// CONTROLE BLUETOOTH
// =====================================================

ControllerPtr controle;

// =====================================================
// FUNÇÕES DOS MOTORES
// =====================================================

// -----------------------------------------------------
// MOTOR ESQUERDO
// -----------------------------------------------------

void motorEsquerdoFrente() {

  if (INVERTER_MOTOR_ESQUERDO) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }
}

void motorEsquerdoTras() {

  if (INVERTER_MOTOR_ESQUERDO) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
}

void motorEsquerdoParar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

// -----------------------------------------------------
// MOTOR DIREITO
// -----------------------------------------------------

void motorDireitoFrente() {

  if (INVERTER_MOTOR_DIREITO) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
}

void motorDireitoTras() {

  if (INVERTER_MOTOR_DIREITO) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
}

void motorDireitoParar() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// =====================================================
// PARAR OS DOIS MOTORES
// =====================================================

void pararMotores() {
  motorEsquerdoParar();
  motorDireitoParar();
}

// =====================================================
// MOVIMENTOS
// =====================================================

// -----------------------------------------------------
// FRENTE
// -----------------------------------------------------

void frente() {
  motorEsquerdoFrente();
  motorDireitoFrente();
}

// -----------------------------------------------------
// TRÁS
// -----------------------------------------------------

void tras() {
  motorEsquerdoTras();
  motorDireitoTras();
}

// -----------------------------------------------------
// CURVA PARA A ESQUERDA
// Motor esquerdo parado
// Motor direito girando
// -----------------------------------------------------

void curvaEsquerda() {
  motorEsquerdoParar();
  motorDireitoFrente();
}

// -----------------------------------------------------
// CURVA PARA A DIREITA
// Motor direito parado
// Motor esquerdo girando
// -----------------------------------------------------

void curvaDireita() {
  motorEsquerdoFrente();
  motorDireitoParar();
}

// =====================================================
// CALLBACK: CONTROLE CONECTADO
// =====================================================

void onConnectedController(ControllerPtr ctl) {

  if (controle == nullptr) {

    controle = ctl;

    Serial.println("=================================");
    Serial.println("CONTROLE PS5 CONECTADO!");
    Serial.println("=================================");
  }
}

// =====================================================
// CALLBACK: CONTROLE DESCONECTADO
// =====================================================

void onDisconnectedController(ControllerPtr ctl) {

  if (controle == ctl) {

    controle = nullptr;

    pararMotores();

    Serial.println("=================================");
    Serial.println("CONTROLE DESCONECTADO!");
    Serial.println("MOTORES PARADOS!");
    Serial.println("=================================");
  }
}

// =====================================================
// PROCESSAMENTO DO CONTROLE
// =====================================================

void processarControle(ControllerPtr ctl) {

  // ---------------------------------------------------
  // LEITURA DOS GATILHOS
  // ---------------------------------------------------

  int R2 = ctl->throttle();
  int L2 = ctl->brake();

  // ---------------------------------------------------
  // LEITURA DO ANALÓGICO ESQUERDO
  // ---------------------------------------------------

  int eixoX = ctl->axisX();

  // ---------------------------------------------------
  // DEBUG NO SERIAL
  // ---------------------------------------------------

  Serial.print("R2: ");
  Serial.print(R2);

  Serial.print(" | L2: ");
  Serial.print(L2);

  Serial.print(" | X: ");
  Serial.println(eixoX);

  // ===================================================
  // PRIORIDADE 1
  // R2 + L2 = PARAR
  // ===================================================

  if (R2 > 100 && L2 > 100) {

    pararMotores();

    return;
  }

  // ===================================================
  // PRIORIDADE 2
  // R2 = FRENTE
  // ===================================================

  if (R2 > 100) {

    // Analógico para a esquerda
    if (eixoX < -DEADZONE) {

      curvaEsquerda();

    }

    // Analógico para a direita
    else if (eixoX > DEADZONE) {

      curvaDireita();

    }

    // Analógico centralizado
    else {

      frente();

    }

    return;
  }

  // ===================================================
  // PRIORIDADE 3
  // L2 = RÉ
  // ===================================================

  if (L2 > 100) {

    // Analógico para a esquerda
    if (eixoX < -DEADZONE) {

      // Motor esquerdo parado
      // Motor direito vai para trás

      motorEsquerdoParar();
      motorDireitoTras();

    }

    // Analógico para a direita
    else if (eixoX > DEADZONE) {

      // Motor direito parado
      // Motor esquerdo vai para trás

      motorEsquerdoTras();
      motorDireitoParar();

    }

    // Analógico centralizado
    else {

      tras();

    }

    return;
  }

  // ===================================================
  // NENHUM GATILHO PRESSIONADO
  // ===================================================

  pararMotores();
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  // Configura os pinos da ponte H
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Garante que os motores começam parados
  pararMotores();

  // ---------------------------------------------------
  // BLUEPAD32
  // ---------------------------------------------------

  BP32.setup(
    &onConnectedController,
    &onDisconnectedController
  );

  Serial.println("=================================");
  Serial.println("ROTOM INICIADO");
  Serial.println("Aguardando controle PS5...");
  Serial.println("=================================");
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  // Atualiza o Bluepad32
  BP32.update();

  // ---------------------------------------------------
  // Verifica se existe controle conectado
  // ---------------------------------------------------

  if (controle != nullptr && controle->isConnected()) {

    processarControle(controle);

  } else {

    // Segurança:
    // se não houver controle, motores parados

    pararMotores();
  }

  delay(20);
}