#include <Pixy2.h>

// Crea un objeto Pixy2 para comunicación SPI
Pixy2 pixy;

// --- Configuración para la DISTANCIA (¡MUY IMPORTANTE: NECESITA CALIBRACIÓN!) ---
const float KNOWN_OBJECT_SIZE_CM = 5.0;      // <<< ¡AJUSTA ESTE VALOR al tamaño REAL de tu objeto!
const float PIXY_FOCAL_LENGTH_PIXELS = 277.0; // <<< ¡AJUSTA ESTE VALOR POR CALIBRACIÓN!


// =========================================================================
// --- CONFIGURACIÓN DE FILTROS POR ROL DE FIRMA ---
// =========================================================================

// --- Rol 1: OBJETIVO PRINCIPAL (Filtros Estrictos) ---
const int TARGET_SIGNATURE = 1;               // Firma para el objetivo (rojo).
const float TARGET_DISTANCE_THRESHOLD = 80.0; // Distancia para considerarlo "lejano".
const int TARGET_MIN_AGE_FRAMES = 10;         // Nº de frames para no ser "fugaz".
const int TARGET_MIN_AREA_PIXELS = 50;        // Área mínima para no ser ruido.
const float TARGET_MIN_ASPECT_RATIO = 0.5;    // Ratio mínimo de forma (ancho/alto).
const float TARGET_MAX_ASPECT_RATIO = 2.0;    // Ratio máximo de forma (ancho/alto).

// --- Rol 2: OBSTÁCULO / SEGUNDA PRIORIDAD (Ahora con Filtros Estrictos) ---
const int OBSTACLE_SIGNATURE = 3;             // Firma para el obstáculo (verde).
// >>> AHORA USA LOS MISMOS PARÁMETROS DE FILTRADO QUE EL OBJETIVO PRINCIPAL <<<
const float OBSTACLE_DISTANCE_THRESHOLD = 80.0; // Misma distancia para considerarlo "lejano".
const int OBSTACLE_MIN_AGE_FRAMES = 10;         // Mismo nº de frames para no ser "fugaz".
const int OBSTACLE_MIN_AREA_PIXELS = 50;        // Misma área mínima para no ser ruido.
const float OBSTACLE_MIN_ASPECT_RATIO = 0.5;    // Mismo ratio mínimo de forma (ancho/alto).
const float OBSTACLE_MAX_ASPECT_RATIO = 2.0;    // Mismo ratio máximo de forma (ancho/alto).


// =========================================================================
// --- CONFIGURACIÓN DE CUADRANTES DE PANTALLA ---
// Asumiendo que la Pixy2 tiene una resolución de 316x208 (ancho x alto)
const int SCREEN_CENTER_X = 316 / 2;
const int SCREEN_CENTER_Y = 208 / 2;
const int CENTRAL_QUADRANT_THRESHOLD_X = 50; // Margen en X para el cuadrante central
const int CENTRAL_QUADRANT_THRESHOLD_Y = 30; // Margen en Y para el cuadrante central


void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando PixyCam con Filtros por Roles...");
  delay(100);

  if (pixy.init() != 0) {
    Serial.println("Error al inicializar PixyCam. Reintentando...");
    while(pixy.init() != 0);
  }
  Serial.println("PixyCam inicializada correctamente.");
  Serial.println("----------------------------------------------");
}

void loop() {
  int blockCount = pixy.ccc.getBlocks();

  if (blockCount < 0) {
    Serial.print("Error de comunicacion con PixyCam: ");
    Serial.println(blockCount);
    return;
  }

  if (blockCount > 0) {
    Serial.print("Detectados ");
    Serial.print(blockCount);
    Serial.println(" objetos:");

    // --- Lógica para encontrar el objeto OBSTACLE_SIGNATURE más cercano ---
    float closestObstacleDistance = 9999.0; // Inicializamos con un valor muy alto
    int closestObstacleIndex = -1; // Para guardar el índice del obstáculo más cercano

    for (int i = 0; i < blockCount; i++) {
      if (pixy.ccc.blocks[i].m_signature == OBSTACLE_SIGNATURE) {
        int obstacleWidth = pixy.ccc.blocks[i].m_width;
        if (obstacleWidth > 0) {
          float currentObstacleDistance = (KNOWN_OBJECT_SIZE_CM * PIXY_FOCAL_LENGTH_PIXELS) / obstacleWidth;
          if (currentObstacleDistance < closestObstacleDistance) {
            closestObstacleDistance = currentObstacleDistance;
            closestObstacleIndex = i;
          }
        }
      }
    }
    // --- Fin de la lógica para encontrar el obstáculo más cercano ---


    for (int j = 0; j < blockCount; j++) {
      // Extrae los datos comunes del objeto actual
      int signature = pixy.ccc.blocks[j].m_signature;
      int x = pixy.ccc.blocks[j].m_x;
      int y = pixy.ccc.blocks[j].m_y;
      int width = pixy.ccc.blocks[j].m_width;
      int height = pixy.ccc.blocks[j].m_height;
      int age = pixy.ccc.blocks[j].m_age;
      
      String objectLabel = ""; // Etiqueta para imprimir
      float distance = 0.0;
      bool printPriorityDetails = false; // Bandera para controlar la impresión de detalles de objetos prioritarios

      // Calcula la distancia si el ancho es válido (evitar división por cero)
      if (width > 0) {
        distance = (KNOWN_OBJECT_SIZE_CM * PIXY_FOCAL_LENGTH_PIXELS) / width;
      }

      // --- SWITCH DE DECISIÓN POR FIRMA ---
      switch (signature) {
        
        case TARGET_SIGNATURE: { // Reglas para el OBJETIVO PRINCIPAL (rojo)
          objectLabel = "[OBJETIVO] ";
          
          if ((width * height) < TARGET_MIN_AREA_PIXELS) {
            Serial.println("   --> Descartado " + objectLabel + " (área muy pequeña).");
            continue;
          }
          
          float aspectRatio = (float)width / (float)height;
          if (aspectRatio < TARGET_MIN_ASPECT_RATIO || aspectRatio > TARGET_MAX_ASPECT_RATIO) {
            Serial.println("   --> Descartado " + objectLabel + " (forma incorrecta).");
            continue;
          }
          
          if (age < TARGET_MIN_AGE_FRAMES && distance > TARGET_DISTANCE_THRESHOLD) {
            Serial.println("   --> Descartado " + objectLabel + " (lejano y fugaz).");
            continue;
          }
          
          objectLabel = "[OBJETIVO VÁLIDO] ";
          printPriorityDetails = true; // Activa la impresión de detalles para el objetivo

          // Lógica para girar a la izquierda si el objetivo está en el cuadrante central
          if (x >= (SCREEN_CENTER_X - CENTRAL_QUADRANT_THRESHOLD_X) && 
              x <= (SCREEN_CENTER_X + CENTRAL_QUADRANT_THRESHOLD_X) &&
              y >= (SCREEN_CENTER_Y - CENTRAL_QUADRANT_THRESHOLD_Y) &&
              y <= (SCREEN_CENTER_Y + CENTRAL_QUADRANT_THRESHOLD_Y)) {
            Serial.println("   --> OBJETIVO EN CENTRO: Girar a la izquierda!");
            // Aquí iría el código para que tu robot gire a la izquierda
            // Ejemplo: robot.turnLeft(); 
          }
          break; // Pasa a la impresión de datos (si printPriorityDetails es true)
        }

        case OBSTACLE_SIGNATURE: { // Reglas para el OBSTÁCULO (verde) - AHORA COMO PRIORITARIO
          // Si este no es el obstáculo más cercano, lo ignoramos y pasamos al siguiente.
          if (j != closestObstacleIndex) {
            // Serial.println("   --> Descartado OBSTÁCULO (no es el más cercano)."); // Descomentar para depurar
            continue;
          }

          objectLabel = "[OBSTÁCULO] ";

          if ((width * height) < OBSTACLE_MIN_AREA_PIXELS) {
            Serial.println("   --> Descartado " + objectLabel + " (área muy pequeña).");
            continue;
          }

          float aspectRatio = (float)width / (float)height;
          if (aspectRatio < OBSTACLE_MIN_ASPECT_RATIO || aspectRatio > OBSTACLE_MAX_ASPECT_RATIO) {
            Serial.println("   --> Descartado " + objectLabel + " (forma incorrecta).");
            continue;
          }
          
          if (age < OBSTACLE_MIN_AGE_FRAMES && distance > OBSTACLE_DISTANCE_THRESHOLD) {
            Serial.println("   --> Descartado " + objectLabel + " (lejano y fugaz).");
            continue;
          }
          
          objectLabel = "[OBSTÁCULO PRIORITARIO] ";
          printPriorityDetails = true; // Activa la impresión de detalles para este obstáculo prioritario

          // Lógica para girar a la izquierda si el obstáculo está en el cuadrante central
          if (x >= (SCREEN_CENTER_X - CENTRAL_QUADRANT_THRESHOLD_X) && 
              x <= (SCREEN_CENTER_X + CENTRAL_QUADRANT_THRESHOLD_X) &&
              y >= (SCREEN_CENTER_Y - CENTRAL_QUADRANT_THRESHOLD_Y) &&
              y <= (SCREEN_CENTER_Y + CENTRAL_QUADRANT_THRESHOLD_Y)) {
            Serial.println("   --> OBSTÁCULO EN CENTRO: Girar a la izquierda!");
            // Aquí iría el código para que tu robot gire a la izquierda
            // Ejemplo: robot.turnLeft(); 
          }
          break; 
        }

        default: { // Reglas para cualquier otra firma (ej. firma 2)
          objectLabel = "[Info] ";
          // printPriorityDetails no se activa para otras firmas, por lo que no se imprimirán los detalles.
          break; 
        }
      }

      // --- IMPRESIÓN DE DATOS SOLO PARA OBJETOS PRIORITARIOS (SIGNATURA 1 Y EL OBSTÁCULO MÁS CERCANO) ---
      // Este código solo se ejecuta si el objeto no fue descartado por un 'continue'
      // Y si la bandera printPriorityDetails es verdadera
      if (printPriorityDetails) {
        Serial.print("   " + objectLabel + "Firma: ");
        Serial.print(signature);
        Serial.print("   X: ");
        Serial.print(x);
        Serial.print("   Y: ");
        Serial.print(y);
        Serial.print("   Ancho: ");
        Serial.print(width);
        Serial.print("   Alto: ");
        Serial.print(height);
        Serial.print("   Edad: ");
        Serial.print(age);
        
        if (width > 0) { // Solo imprime la distancia si el ancho es válido para evitar NaNs
          Serial.print("   Distancia: ");
          Serial.print(distance, 1);
          Serial.println(" cm");
        } else {
          Serial.println();
        }
      }
    }
    Serial.println("---");
  }

  delay(2000);
}