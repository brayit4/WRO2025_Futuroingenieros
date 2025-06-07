#include <Pixy2.h> // Incluye la librería oficial de Pixy2

// Crea un objeto Pixy2 para la comunicación con la cámara
Pixy2 pixy;

// --- Variables para almacenar la información del objeto detectado ---
int detectedSignature = 0;   // 0 si no se detecta nada; 1-7 para las firmas de color.
int detectedX = -1;          // Coordenada X del centro del objeto (0-319; -1 si no).
int detectedY = -1;          // Coordenada Y del centro del objeto (0-199; -1 si no).
int detectedWidth = 0;       // Ancho del objeto en píxeles (0-319; 0 si no hay objeto).
int detectedHeight = 0;      // Alto del objeto en píxeles (0-199; 0 si no hay objeto).
// --- Fin de Variables de Detección ---

// --- Variables para el estado del carrito (ejemplo, puedes añadir más) ---
enum CarritoEstado {
  BUSCANDO,
  SIGUIENDO_VERDE,
  EVITANDO_ROJO,
  DETENIDO_AZUL,
  PARADO // Estado inicial o de error
};

CarritoEstado estadoActual = PARADO; // El carrito empieza parado

// --- Constantes para la lógica de seguimiento/evasión (ajusta según tu carrito) ---
const int CENTRO_MIN_X = 140; // Rango inferior del centro de la imagen (aproximadamente 320/2 - buffer)
const int CENTRO_MAX_X = 180; // Rango superior del centro de la imagen (aproximadamente 320/2 + buffer)
const int DISTANCIA_CERCANA_Y = 150; // Si Y es mayor que esto, el objeto está muy cerca (Pixy Y va de 0 arriba a 199 abajo)
// --- Fin de Constantes ---

void setup() {
  // Configuración de pines para motores o LEDs aquí si las tienes (no incluidas en este código)
  // pinMode(LED_VERDE_PIN, OUTPUT);
  // pinMode(LED_ROJO_PIN, OUTPUT);

  // Serial.begin(115200); // Solo para depuración
  // Serial.println("Iniciando Pixy2 y lógica de colores...");

  pixy.init();
  // pixy.changeProg("color_connected_components"); // Asegúrate de que Pixy2 esté en este programa
}


void PIXY() {
  // 1. Resetear las variables de detección en cada ciclo
  detectedSignature = 0;
  detectedX         = -1;
  detectedY         = -1;
  detectedWidth     = 0;
  detectedHeight    = 0;

  // 2. Obtener el número de bloques de color detectados
  int numBlocks = pixy.ccc.getBlocks();

  // 3. Procesar los datos si se detectó al menos un bloque de color
  if (numBlocks > 0) {
    // Para esta lógica, tomaremos el primer bloque (índice 0).
    // Si necesitas el más grande, usa la lógica de "largestBlockIndex" del código anterior.
    detectedSignature = pixy.ccc.blocks[0].m_signature;
    detectedX         = pixy.ccc.blocks[0].m_x;
    detectedY         = pixy.ccc.blocks[0].m_y;
    detectedWidth     = pixy.ccc.blocks[0].m_width;
    detectedHeight    = pixy.ccc.blocks[0].m_height;
  }

  // --- LÓGICA PRINCIPAL PARA RECONOCER COLORES Y DECIDIR ACCIONES ---

  switch (detectedSignature) {
    case 1: // Firma 1: Reconocida como 'ROJO' (ej. obstáculo o meta a evitar)
      estadoActual = EVITANDO_ROJO;
      // Aquí puedes añadir más lógica para el movimiento de evasión
      if (detectedY > DISTANCIA_CERCANA_Y) { // Si el objeto rojo está muy cerca
        // Frenar o girar bruscamente para evitar la colisión
        // controlMotor(DETENER); // Función ficticia de control de motor
        // Serial.println("Objeto ROJO muy cerca! Deteniendo."); // Debug
      } else if (detectedX < CENTRO_MIN_X) { // Objeto rojo a la izquierda
        // Girar a la derecha para rodearlo
        // controlMotor(GIRAR_DERECHA);
        // Serial.println("Objeto ROJO a la izquierda! Girando a la derecha."); // Debug
      } else if (detectedX > CENTRO_MAX_X) { // Objeto rojo a la derecha
        // Girar a la izquierda para rodearlo
        // controlMotor(GIRAR_IZQUIERDA);
        // Serial.println("Objeto ROJO a la derecha! Girando a la izquierda."); // Debug
      } else { // Objeto rojo delante, pero no demasiado cerca
        // Puede seguir recto con cautela o detenerse preventivamente
        // controlMotor(AVANZAR_LENTO);
        // Serial.println("Objeto ROJO delante. Avanzando con cautela."); // Debug
      }
      break;

    case 2: // Firma 2: Reconocida como 'VERDE' (ej. objetivo o camino a seguir)
      estadoActual = SIGUIENDO_VERDE;
      // Lógica para seguir el objeto verde
      if (detectedX < CENTRO_MIN_X) { // Objeto verde a la izquierda
        // Girar a la izquierda para centrarlo
        // controlMotor(GIRAR_IZQUIERDA);
        // Serial.println("Objeto VERDE a la izquierda! Girando a la izquierda."); // Debug
      } else if (detectedX > CENTRO_MAX_X) { // Objeto verde a la derecha
        // Girar a la derecha para centrarlo
        // controlMotor(GIRAR_DERECHA);
        // Serial.println("Objeto VERDE a la derecha! Girando a la derecha."); // Debug
      } else { // Objeto verde centrado
        // Avanzar recto hacia él
        // controlMotor(AVANZAR_RECTO);
        // Serial.println("Objeto VERDE centrado! Avanzando."); // Debug
      }
      // Si está muy cerca (detectar por Y o por size), puede detenerse o agarrar
      if (detectedY > DISTANCIA_CERCANA_Y && detectedX >= CENTRO_MIN_X && detectedX <= CENTRO_MAX_X) {
        // Objeto verde cerca y centrado. Alcanzado el objetivo.
        // controlMotor(DETENER);
        // Serial.println("Objeto VERDE alcanzado! Deteniendo."); // Debug
      }
      break;

    case 3: // Firma 3: Reconocida como 'AZUL' (ej. zona de parada o descanso)
      estadoActual = DETENIDO_AZUL;
      // El carrito debe detenerse completamente al ver este color
      // controlMotor(DETENER);
      // Serial.println("Objeto AZUL detectado! Deteniendo completamente."); // Debug
      break;

    default: // detectedSignature es 0 o cualquier otra firma no definida
      estadoActual = BUSCANDO;
      // No se detectó ningún color de interés. El carrito puede:
      // - Girar lentamente en círculos para buscar el objetivo.
      // - Avanzar despacio.
      // - Quedarse quieto y esperar.
      // controlMotor(BUSCAR_OBJETIVO);
      // Serial.println("Buscando objetivo..."); // Debug
      break;
  }

  // --- Fin de la Lógica Principal ---


  // Pequeña pausa para permitir que Pixy2 procese los frames de la cámara.
  // Ajusta este delay si necesitas más o menos velocidad de detección.
  delay(20);
}


// --- Aquí irían tus funciones reales para controlar los motores del carrito ---
// Por ejemplo, usando un driver L298N, un TB6612FNG, o directamente con PWM y HIGH/LOW.
// Necesitarías definir tus pines de motor y la lógica de movimiento.
/*
// Ejemplo de definiciones (adapta a tu carrito)
#define MOTOR_IZQ_ADELANTE_PIN  2
#define MOTOR_IZQ_ATRAS_PIN    3
#define MOTOR_DER_ADELANTE_PIN  4
#define MOTOR_DER_ATRAS_PIN    5
#define MOTOR_PWM_PIN_IZQ      6  // Si controlas velocidad con PWM
#define MOTOR_PWM_PIN_DER      7  // Si controlas velocidad con PWM

void controlMotor(int accion) {
  switch (accion) {
    case AVANZAR_RECTO:
      // Lógica para motores que avancen recto
      // digitalWrite(MOTOR_IZQ_ADELANTE_PIN, HIGH);
      // digitalWrite(MOTOR_DER_ADELANTE_PIN, HIGH);
      // analogWrite(MOTOR_PWM_PIN_IZQ, 200);
      // analogWrite(MOTOR_PWM_PIN_DER, 200);
      break;
    case GIRAR_IZQUIERDA:
      // Lógica para motores que giren a la izquierda
      break;
    case GIRAR_DERECHA:
      // Lógica para motores que giren a la derecha
      break;
    case DETENER:
      // Lógica para detener los motores
      // digitalWrite(MOTOR_IZQ_ADELANTE_PIN, LOW);
      // digitalWrite(MOTOR_IZQ_ATRAS_PIN, LOW);
      // digitalWrite(MOTOR_DER_ADELANTE_PIN, LOW);
      // digitalWrite(MOTOR_DER_ATRAS_PIN, LOW);
      break;
    // ... otros estados de movimiento
  }
}

// Definiciones de acciones de movimiento (ejemplo)
#define AVANZAR_RECTO       1
#define AVANZAR_LENTO       2
#define GIRAR_IZQUIERDA     3
#define GIRAR_DERECHA       4
#define DETENER             5
#define BUSCAR_OBJETIVO     6
*/