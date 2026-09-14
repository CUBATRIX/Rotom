// Motor esquerdo
#define IN1 19
#define IN2 18

// Motor direito
#define IN3 17
#define IN4 16

// Tempos
const int TEMPO_FRENTE = 3000;  // 3 segundos
const int TEMPO_PARADO = 2000;  // 2 segundos
const int TEMPO_TRAS   = 3000;  // 3 segundos


// =====================================================
// MOTORES PARA FRENTE
// =====================================================

void motoresFrente() {

  Serial.println(">>> OS DOIS MOTORES: FRENTE");

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}


// =====================================================
// MOTORES PARA TRÁS
// =====================================================

void motoresTras() {

  Serial.println("<<< OS DOIS MOTORES: TRÁS");

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}


// =====================================================
// PARAR MOTORES
// =====================================================

void motoresParados() {

  Serial.println("--- MOTORES PARADOS");

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Começa parado
  motoresParados();

  Serial.println();
  Serial.println("==============================");
  Serial.println(" TESTE DOS DOIS MOTORES");
  Serial.println("==============================");
}


// =====================================================
// LOOP
// =====================================================

void loop() {

  // -----------------------------------------
  // FRENTE
  // -----------------------------------------

  motoresFrente();

  delay(TEMPO_FRENTE);


  // -----------------------------------------
  // PARADO
  // -----------------------------------------

  motoresParados();

  delay(TEMPO_PARADO);


  // -----------------------------------------
  // TRÁS
  // -----------------------------------------

  motoresTras();

  delay(TEMPO_TRAS);


  // -----------------------------------------
  // PARADO
  // -----------------------------------------

  motoresParados();

  delay(TEMPO_PARADO);
}