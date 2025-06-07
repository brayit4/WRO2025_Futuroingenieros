// --- Pines de Conexión del Sensor TCS3200 ---
const int S0 = 4;
const int S1 = 5;
const int S2 = 6;
const int S3 = 7;
const int OUT = 8; // Pin de salida de frecuencia del sensor

// --- Variables para almacenar los valores de los colores ---
// Estos valores representan la 'duración del pulso' (en microsegundos)
// para cada componente de color (Rojo, Verde, Azul).
// Una DURACIÓN MÁS PEQUEÑA (mayor frecuencia) significa que hay MÁS LUZ de ese color.
int valorRojo = 0;
int valorVerde = 0;
int valorAzul = 0;

// --- Definición de los Colores a Reconocer (Enumeración) ---
// Usamos un 'enum' para dar nombres claros a los colores que queremos identificar.
enum ColorIdentificado {
  COLOR_DESCONOCIDO,
  COLOR_ROJO,
  COLOR_VERDE,
  COLOR_AZUL,
  COLOR_BLANCO,
  COLOR_NEGRO,
  COLOR_AMARILLO // Puedes añadir más colores si lo necesitas
};

// --- Variable para almacenar el Color Detectado ---
// Esta será la variable principal que contendrá el color clasificado.
ColorIdentificado colorDetectado = COLOR_DESCONOCIDO;


// --- Función: Leer la frecuencia de un componente de color ---
// Toma los pines S2 y S3 para seleccionar el filtro de color (R, G, B o Clear).
// Devuelve la duración del pulso LOW en microsegundos.
// Menor duración = mayor intensidad de ese color.
int leerComponenteColor(int filtroS2, int filtroS3) {
  digitalWrite(S2, filtroS2);
  digitalWrite(S3, filtroS3);
  // 'pulseIn(OUT, LOW)' mide el tiempo que el pin OUT permanece en LOW.
  // El TCS3200 genera una onda cuadrada, y su frecuencia cambia con la luz.
  // Al medir el tiempo de un pulso LOW, estamos midiendo el inverso de la frecuencia.
  return pulseIn(OUT, LOW);
}


void setup() {
  // Configurar pines de control (S0, S1, S2, S3) como SALIDAS
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  // Configurar el pin de salida de frecuencia (OUT) como ENTRADA
  pinMode(OUT, INPUT);

  // Configurar la escala de frecuencia de salida del TCS3200.
  // S0 = HIGH, S1 = LOW -> Selecciona el 20% de escalado de la frecuencia.
  // Esta es una configuración común para Arduino.
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
}


void loop() {
  // 1. Obtener las lecturas de los componentes de color (R, G, B)
  // Se leen secuencialmente, con pequeños delays entre cada lectura para mayor estabilidad.

  // Leer el componente ROJO
  valorRojo = leerComponenteColor(LOW, LOW); // S2=LOW, S3=LOW para Rojo
  delay(10); // Pequeña pausa

  // Leer el componente AZUL
  valorAzul = leerComponenteColor(LOW, HIGH); // S2=LOW, S3=HIGH para Azul
  delay(10); // Pequeña pausa

  // Leer el componente VERDE
  valorVerde = leerComponenteColor(HIGH, LOW); // S2=HIGH, S3=LOW para Verde
  delay(10); // Pequeña pausa


  // 2. Clasificar el color basándose en los valores RGB leídos
  // --- ¡IMPORTANTE: CALIBRACIÓN! ---
  // Los siguientes rangos (ej. 30, 80, 150) son EJEMPLOS.
  // Necesitas determinar tus propios rangos específicos experimentalmente.
  //
  // Para CALIBRAR:
  // a) Descomenta 'Serial.begin(9600);' y 'Serial.println' en el setup y loop.
  // b) Apunta el sensor a un objeto rojo y anota los valores de valorRojo, valorVerde, valorAzul.
  // c) Repite para verde, azul, blanco, negro, amarillo, etc.
  // d) Con esos valores, ajusta los umbrales para cada color a continuación.
  //    Recuerda: Un valor BAJO significa MÁS de ese color.

  // Reiniciar el color detectado al inicio de cada clasificación
  colorDetectado = COLOR_DESCONOCIDO;

  // Lógica de clasificación usando umbrales
  // El orden de las condiciones puede influir. A menudo, los colores más extremos (negro, blanco)
  // o los más críticos se evalúan primero.

  // --- Detección de NEGRO (muy poca luz de todos los colores = valores altos) ---
  // Si los valores de R, G, B son todos altos (por encima de un umbral), es negro.
  if (valorRojo > 150 && valorVerde > 150 && valorAzul > 150) {
    colorDetectado = COLOR_NEGRO;
  }
  // --- Detección de BLANCO (mucha luz de todos los colores = valores bajos) ---
  // Si los valores de R, G, B son todos bajos (por debajo de un umbral), es blanco.
  else if (valorRojo < 60 && valorVerde < 60 && valorAzul < 60) {
    colorDetectado = COLOR_BLANCO;
  }
  // --- Detección de ROJO ---
  // El valor de ROJO es bajo (mucha luz roja), y los de VERDE y AZUL son altos (poca de las otras luces).
  else if (valorRojo < 40 && valorVerde > 80 && valorAzul > 80) {
    colorDetectado = COLOR_ROJO;
  }
  // --- Detección de VERDE ---
  // El valor de VERDE es bajo, y los de ROJO y AZUL son altos.
  else if (valorRojo > 80 && valorVerde < 40 && valorAzul > 80) {
    colorDetectado = COLOR_VERDE;
  }
  // --- Detección de AZUL ---
  // El valor de AZUL es bajo, y los de ROJO y VERDE son altos.
  else if (valorRojo > 80 && valorVerde > 80 && valorAzul < 40) {
    colorDetectado = COLOR_AZUL;
  }
  // --- Detección de AMARILLO (Rojo y Verde bajos, Azul alto) ---
  // El amarillo es una mezcla de rojo y verde.
  else if (valorRojo < 70 && valorVerde < 70 && valorAzul > 100) {
    colorDetectado = COLOR_AMARILLO;
  }
  // Si no coincide con ninguno de los rangos anteriores, el color sigue siendo COLOR_DESCONOCIDO.


  // --- Variables actualizadas ---
  // En este punto del código, las variables 'valorRojo', 'valorVerde', 'valorAzul'
  // contienen las lecturas crudas del sensor.
  // Y la variable 'colorDetectado' contiene la clasificación del color (ej. COLOR_ROJO, COLOR_VERDE, etc.).
  // Estas variables están listas para ser usadas en cualquier otra parte de tu programa.

  /*
  // Ejemplo de cómo acceder a las variables (comentado):
  if (colorDetectado == COLOR_ROJO) {
    // Aquí puedes ejecutar una acción para cuando se detecta el color rojo
    // por ejemplo, mover un robot o encender un LED específico.
  } else if (colorDetectado == COLOR_VERDE) {
    // Acción para verde
  }
  // Puedes usar valorRojo, valorVerde, valorAzul para depurar o para lógicas más finas.
  */

  // Un pequeño retardo al final del loop para controlar la velocidad de las lecturas
  delay(100); // Ajusta este valor si necesitas lecturas más rápidas o más lentas.
}