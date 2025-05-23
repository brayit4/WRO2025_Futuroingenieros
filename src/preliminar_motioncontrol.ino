##include <NewPing.h>

// Definición de pines
#define TRIGGER_PIN_FRONT 2
#define ECHO_PIN_FRONT 3
#define TRIGGER_PIN_LEFT 4
#define ECHO_PIN_LEFT 5
#define TRIGGER_PIN_RIGHT 6
#define ECHO_PIN_RIGHT 7
#define MAX_DISTANCE_FRONT 200  // 200 cm máximo
#define MAX_DISTANCE_SIDES 100  // 100 cm máximo
#define SONAR_NUM 3

// Pines de control del motor
#define IN1 8
#define IN2 9
#define ENA 3
#define SENSOR_IR A0

NewPing sonar[SONAR_NUM] = {
  NewPing(TRIGGER_PIN_FRONT, ECHO_PIN_FRONT, MAX_DISTANCE_FRONT),  // Front
  NewPing(TRIGGER_PIN_LEFT, ECHO_PIN_LEFT, MAX_DISTANCE_SIDES),    // Left
  NewPing(TRIGGER_PIN_RIGHT, ECHO_PIN_RIGHT, MAX_DISTANCE_SIDES)   // Right
};

int velocidad_actual = 0;

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // Ejemplo de uso
  avanzar(150, 2000, 90, velocidad_actual);
  detener(1000);
}

// Función para movimiento hacia adelante mejorada
void avanzar(int velocidad_deseada, int tiempo, int angulo_deseado, int &velocidad_actual) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  acelerar_desacelerar(velocidad_deseada, tiempo, velocidad_actual);
  // Implementar lógica de giro real con encoder o IMU
}

// Función de aceleración/desaceleración suave
void acelerar_desacelerar(int velocidad_deseada, int tiempo, int &velocidad_actual) {
  int paso = 1;
  int delay_time = tiempo / abs(velocidad_deseada - velocidad_actual);
  
  if(delay_time <= 0) delay_time = 10;  // Prevención de división por cero
  
  if(velocidad_deseada > velocidad_actual) {
    for(int v = velocidad_actual; v <= velocidad_deseada; v += paso) {
      analogWrite(ENA, v);
      delay(delay_time);
    }
  } else {
    for(int v = velocidad_actual; v >= velocidad_deseada; v -= paso) {
      analogWrite(ENA, v);
      delay(delay_time);
    }
  }
  velocidad_actual = velocidad_deseada;
}

// Función mejorada para medición de color
int get_color() {
  const int lecturas = 5;
  int total = 0;
  
  for(int i = 0; i < lecturas; i++) {
    total += analogRead(SENSOR_IR);
    delay(10);
  }
  return total / lecturas;
}

// Función mejorada para medir distancia
int medir_distancia(int sensor_index) {
  if(sensor_index < 0 || sensor_index >= SONAR_NUM) return -1;
  return sonar[sensor_index].ping_cm();
}

// Función de detención mejorada
void detener(int tiempo) {
  analogWrite(ENA, 0);
  velocidad_actual = 0;
  delay(tiempo);
}