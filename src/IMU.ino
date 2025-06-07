#include <SparkFunMPU9250.h> // Incluye la librería para el MPU-9250
#include <Wire.h>            // Librería para comunicación I2C

// Crea un objeto MPU-9250
MPU9250 mpu;

// Variable para almacenar el ángulo absoluto de inclinación en Z (Yaw)
float anguloInclinacionZ_Yaw = 0.0;

void setup() {
  // Opcional: Iniciar comunicación serial para depuración
  // Serial.begin(115200);
  // while (!Serial) { delay(10); } // Espera a que el Monitor Serial se abra (para placas tipo Leonardo)
  // Serial.println("Iniciando MPU-9250 para ángulo Z (Yaw) absoluto...");

  // Inicializa el sensor MPU-9250
  // La función .begin() configurará el acelerómetro, giroscopio y magnetómetro.
  // También buscará la dirección I2C (0x68 o 0x69) automáticamente.
  if (mpu.begin() != INV_SUCCESS) {
    // Serial.println("Fallo al inicializar el MPU-9250. Verifica las conexiones.");
    while (1) { delay(10); } // Bloquea el programa si el sensor no se encuentra
  }

  // Opcional: Calibrar el giroscopio para mejorar la precisión.
  // Es bueno hacerlo al inicio si el sensor no está en movimiento.
  // Serial.println("Calibrando giroscopio. ¡Mantén el sensor quieto!");
  mpu.calibrateGyro();
  // Serial.println("Giroscopio calibrado.");

  // Opcional: Configurar rangos del acelerómetro y giroscopio
  // mpu.setAccelRange(MPU9250::ACCEL_RANGE_8G);
  // mpu.setGyroRange(MPU9250::GYRO_RANGE_500DPS);

  // NOTA IMPORTANTE sobre la calibración del magnetómetro:
  // La calibración del magnetómetro (brújula) es crucial para un Yaw preciso
  // y sin deriva. Sin embargo, no hay una función de una sola línea en la librería
  // para una calibración en tiempo real. Generalmente, se hace un "calibración de
  // 8" moviendo el sensor en el espacio y luego se usan esos offsets.
  // Para una aplicación simple que requiere "solo el ángulo en Z",
  // la librería SparkFun AHRS internamente hace su mejor esfuerzo con los datos brutos.
}

int IMU() {
  // Esta función lee los datos del acelerómetro, giroscopio y magnetómetro
  // y actualiza los cálculos de orientación (incluyendo el Yaw).
  // Es importante llamar a .read() tan a menudo como sea posible.
  if (mpu.readByte(mpu.AK8963_ADDRESS, AK8963_ST1) & 0x01) // Si hay datos nuevos del magnetómetro listos
  {
      mpu.readMagData(); // Lee los datos del magnetómetro
  }
  mpu.readGyroData();    // Lee los datos del giroscopio
  mpu.readAccelData();   // Lee los datos del acelerómetro

  // Actualiza los ángulos de orientación (Yaw, Pitch, Roll) usando el filtro AHRS.
  // Los datos deben estar en g's para el acelerómetro, grados/segundo para el giroscopio,
  // y microTesla para el magnetómetro. La librería se encarga de las unidades.
  mpu.updateAttitude(mpu.ax, mpu.ay, mpu.az, mpu.gx, mpu.gy, mpu.gz, mpu.mx, mpu.my, mpu.mz);

  // El ángulo de inclinación absoluto en Z (Yaw) se obtiene de la librería.
  // Está en grados y va de -180 a +180.
  anguloInclinacionZ_Yaw = mpu.yaw;

 

  // Pequeño retardo para dar tiempo a la librería a procesar y evitar lecturas demasiado rápidas.
  delay(10);
}