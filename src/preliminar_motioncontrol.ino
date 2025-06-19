#include <Arduino.h>

// --- STEPPER MOTOR PINS ---
#define STEPPER_IN1 12
#define STEPPER_IN2 11
#define STEPPER_IN3 10
#define STEPPER_IN4 9

// --- DC MOTOR PINS (L298N Driver) ---
#define DC_MOTOR_ENA 5   // PWM pin for speed control (Enable pin on L298N)
#define DC_MOTOR_IN1 6   // Digital pin for direction (IN1 on L298N)
#define DC_MOTOR_IN2 7   // Digital pin for direction (IN2 on L298N)

// --- STEPPER MOTOR CONFIGURATION ---
const int PASOS_POR_VUELTA_COMPLETA = 4096; // 8 steps/sequence * 512 sequences/revolution for 28BYJ-48
const float PASOS_POR_GRADO = (float)PASOS_POR_VUELTA_COMPLETA / 360.0;
const int TIEMPO_ENTRE_PASOS_MS = 1; // Milliseconds delay between each step (adjust for speed)

// --- STEPPER MOTOR STATE VARIABLES ---
int currentStepInSequence = 0; // Defines the current step within the 8-step sequence
boolean stepperMotorDirection = true; // true = clockwise, false = anti-clockwise

// --- STEPPER ABSOLUTE POSITION TRACKING ---
long currentAbsoluteMotorPosition = 0; // Tracks the stepper motor's position in steps from the last reset (0-4095)

// Define the 8-step sequence for the stepper motor (Half-step drive)
int stepSequence[8][4] =
{
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

// --- STEPPER MOTOR FUNCTIONS ---
void setStepperMotorStep(int step) {
  digitalWrite(STEPPER_IN1, stepSequence[step][0]);
  digitalWrite(STEPPER_IN2, stepSequence[step][1]);
  digitalWrite(STEPPER_IN3, stepSequence[step][2]);
  digitalWrite(STEPPER_IN4, stepSequence[step][3]);
}

void advanceOneStepperMotorStep() {
  setStepperMotorStep(currentStepInSequence);
  
  if (stepperMotorDirection) {
    currentStepInSequence++;
    currentAbsoluteMotorPosition++; // Increment absolute position for clockwise
    if (currentStepInSequence >= 8) {
      currentStepInSequence = 0; // Wrap sequence
    }
  } else {
    currentStepInSequence--;
    currentAbsoluteMotorPosition--; // Decrement absolute position for anti-clockwise
    if (currentStepInSequence < 0) {
      currentStepInSequence = 7; // Wrap sequence
    }
  }
  
  // Keep absolute position within 0 to PASOS_POR_VUELTA_COMPLETA - 1
  // This handles wrapping around the 360-degree cycle
  currentAbsoluteMotorPosition = (currentAbsoluteMotorPosition % PASOS_POR_VUELTA_COMPLETA + PASOS_POR_VUELTA_COMPLETA) % PASOS_POR_VUELTA_COMPLETA;
  
  delay(TIEMPO_ENTRE_PASOS_MS);
}

// Function to move the stepper motor a certain number of degrees and return to zero
void moverMotorPasoAPasoGrados(float degrees, boolean direction) {
  long initialAbsolutePosition = currentAbsoluteMotorPosition; // Store current position before moving
  int numberOfSteps = round(abs(degrees) * PASOS_POR_GRADO); // Calculate steps needed
  stepperMotorDirection = direction; // Set direction for this movement

  Serial.print("Moviendo motor paso a paso ");
  Serial.print(degrees);
  Serial.print(" grados en dirección ");
  Serial.println(direction ? "Horaria" : "Anti-horaria");
  Serial.print("Total de pasos para este movimiento: ");
  Serial.println(numberOfSteps);

  for (int i = 0; i < numberOfSteps; i++) {
    advanceOneStepperMotorStep();
  }
  
  Serial.print("Posición absoluta actual del motor paso a paso (pasos): ");
  Serial.println(currentAbsoluteMotorPosition);

  // --- Return to Zero Logic ---
  Serial.println("Motor paso a paso regresando a cero absoluto (0 grados)...");
  
  long stepsToReturn = currentAbsoluteMotorPosition; // This is the distance from 0 in steps
  boolean returnDirection;

  // Determine the shortest path back to zero
  if (stepsToReturn <= PASOS_POR_VUELTA_COMPLETA / 2) { // If less than half a turn, go anti-clockwise
    returnDirection = false; // Anti-clockwise
  } else { // If more than half a turn, go clockwise (shorter path)
    stepsToReturn = PASOS_POR_VUELTA_COMPLETA - stepsToReturn; // Calculate steps for clockwise return
    returnDirection = true; // Clockwise
  }

  // Handle the special case where stepsToReturn is 0 (already at zero)
  if (stepsToReturn == 0 && currentAbsoluteMotorPosition != 0) { // If it landed exactly at 0 after the modulo
      // This means it took a full 360-degree turn and landed back on 0
      // No extra return steps needed as it's already there
      Serial.println("Ya en cero absoluto. No se necesita movimiento de retorno.");
  } else if (stepsToReturn > 0) {
      stepperMotorDirection = returnDirection; // Set the return direction
      Serial.print("    Pasos para regresar: ");
      Serial.print(stepsToReturn);
      Serial.println(returnDirection ? " (Horario)" : " (Anti-horario)");
      for (int i = 0; i < stepsToReturn; i++) {
          advanceOneStepperMotorStep();
      }
  }

  Serial.print("Posición absoluta final del motor paso a paso (debería ser 0 pasos): ");
  Serial.println(currentAbsoluteMotorPosition);
  Serial.println("--- Movimiento del motor paso a paso completado ---");

  // Optional: Turn off coils after returning to zero
  // digitalWrite(STEPPER_IN1, LOW); 
  // digitalWrite(STEPPER_IN2, LOW);
  // digitalWrite(STEPPER_IN3, LOW);
  // digitalWrite(STEPPER_IN4, LOW);
}

// --- DC MOTOR FUNCTIONS ---
// Function to move DC motor forward at a given speed (0-255)
void moverDCMotorAdelante(int speed) {
  digitalWrite(DC_MOTOR_IN1, HIGH); // Set direction
  digitalWrite(DC_MOTOR_IN2, LOW);
  analogWrite(DC_MOTOR_ENA, speed); // Set speed using PWM
  Serial.print("Motor DC: Adelante a velocidad ");
  Serial.println(speed);
}

// Function to move DC motor backward at a given speed (0-255)
void moverDCMotorAtras(int speed) {
  digitalWrite(DC_MOTOR_IN1, LOW);  // Set direction
  digitalWrite(DC_MOTOR_IN2, HIGH);
  analogWrite(DC_MOTOR_ENA, speed); // Set speed using PWM
  Serial.print("Motor DC: Atrás a velocidad ");
  Serial.println(speed);
}

// Function to stop DC motor
void detenerDCMotor() {
  digitalWrite(DC_MOTOR_IN1, LOW); // Stop motor by setting both direction pins LOW
  digitalWrite(DC_MOTOR_IN2, LOW);
  analogWrite(DC_MOTOR_ENA, 0);    // Set speed to 0
  Serial.println("Motor DC: Detenido");
}


void setup() {
  Serial.begin(9600); // Serial communication speed
  Serial.println("Iniciando control de motores (paso a paso y DC).");

  // Setup Stepper Motor Pins
  pinMode(STEPPER_IN1, OUTPUT);
  pinMode(STEPPER_IN2, OUTPUT);
  pinMode(STEPPER_IN3, OUTPUT);
  pinMode(STEPPER_IN4, OUTPUT);

  // Setup DC Motor Pins
  pinMode(DC_MOTOR_ENA, OUTPUT);
  pinMode(DC_MOTOR_IN1, OUTPUT);
  pinMode(DC_MOTOR_IN2, OUTPUT);

  // Ensure stepper motor coils are off at the start
  setStepperMotorStep(0); 
  digitalWrite(STEPPER_IN1, LOW); 
  digitalWrite(STEPPER_IN2, LOW);
  digitalWrite(STEPPER_IN3, LOW);
  digitalWrite(STEPPER_IN4, LOW);

  // Ensure DC motor is stopped at the start
  detenerDCMotor();
  
  delay(1000); // Short initial pause
}

void loop() {
  // --- Usage examples ---

  // Ejemplo 1: Mover motor DC hacia adelante, luego detener
  moverDCMotorAdelante(150); // Mueve hacia adelante a velocidad media
  delay(3000); // Por 3 segundos
  detenerDCMotor(); // Detener el motor DC
  delay(1000); // Pausar 1 segundo

  // Ejemplo 2: Mover motor paso a paso, luego regresar a cero
  moverMotorPasoAPasoGrados(60, false); // Mueve el paso a paso 60 grados anti-horario
  delay(2000); // Pausar 2 segundos después de regresar a cero

  // Ejemplo 3: Mover motor DC hacia atrás, luego detener
  moverDCMotorAtras(100); // Mueve hacia atrás a velocidad lenta
  delay(2000); // Por 2 segundos
  detenerDCMotor(); // Detener el motor DC
  delay(1000); // Pausar 1 segundo
  
  // Ejemplo 4: Mover motor paso a paso, luego regresar a cero
  moverMotorPasoAPasoGrados(60, true); // Mueve el paso a paso 60 grados horario
  delay(2000); // Pausar 2 segundos después de regresar a cero

  Serial.println("\n--- Reiniciando ciclo de movimientos ---");
  delay(1000);
}
