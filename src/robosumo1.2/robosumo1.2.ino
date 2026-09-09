void processGamepad(ControllerPtr ctl) {

  Serial.println("---- CONTROLE ----");

  // Leitura dos gatilhos (0 a 1023)
  int acelerador = ctl->throttle(); // R2
  int re = ctl->brake();            // L2

  // Eixo X (curva)
  int x = ctl->axisX(); // -512 a 512

  // Deadzone
  if (abs(x) < 50) x = 0;

  // ==========================
  // DEFINE VELOCIDADE BASE
  // ==========================
  int base = 0;

  if (acelerador > 50) {
    base = map(acelerador, 0, 1023, 0, 255);
    Serial.print("FRENTE | Vel: ");
    Serial.println(base);
  } 
  else if (re > 50) {
    base = -map(re, 0, 1023, 0, 255);
    Serial.print("RE | Vel: ");
    Serial.println(base);
  } 
  else {
    base = 0;
    Serial.println("PARADO");
  }

  // ==========================
  // CURVA (analógico)
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

  Serial.print("Motor A: ");
  Serial.print(motorA);
  Serial.print(" | Motor B: ");
  Serial.println(motorB);

  // ==========================
  // APLICA NOS MOTORES
  // ==========================
  setMotorA(motorA);
  setMotorB(motorB);
}