# Engineering materials

This repository contains engineering materials of a self-driven vehicle's model participating in the WRO Future Engineers competition in the season 2022.

## Team Members:
* Brayan Tamiche
* Eduardo Aguilera
* Alessandro Fabriani

## Content (Preliminar)

* **`t-photos/`**: Contains 2 photos of the team (an official one and one funny photo with all team members).
* **`v-photos/`**: Contains 6 photos of the vehicle (from every side, from top and bottom).
* **`video/`**: Contains the `video.md` file with the link to a video where driving demonstration exists.
* **`schemes/`**: Contains one or several schematic diagrams in form of JPEG, PNG or PDF of the electromechanical components illustrating all the elements (electronic components and motors) used in the vehicle and how they connect to each other.
* **`src/`**: Contains code of control software for all components which were programmed to participate in the competition.
* **`models/`**: Is for the files for models used by 3D printers, laser cutting machines and CNC machines to produce the vehicle elements. If there is nothing to add to this location, the directory can be removed.
* **`other/`**: Is for other files which can be used to understand how to prepare the vehicle for the competition. It may include documentation how to connect to a SBC/SBM and upload files there, datasets, hardware specifications, communication protocols descriptions etc. If there is nothing to add to this location, the directory can be removed.

## Introduction

Nuestro software de control para el vehículo autónomo está desarrollado en el entorno de Arduino IDE, utilizando la placa **Arduino UNO** como controlador principal. El código, ubicado en `src/main_robot_control.ino`, está diseñado para gestionar los principales subsistemas del robot: sensado de distancia, control de motores y detección básica de línea/color.

El código se estructura en varias funciones clave que se relacionan directamente con los componentes electromecánicos:

* **Sensores Ultrasónicos (Distancia):**
    * Utilizamos la librería `NewPing.h` para interactuar con tres sensores ultrasónicos (frontal, izquierdo, derecho).
    * Las funciones `medir_distancia(int sensor_index)` se encargan de leer y retornar la distancia en centímetros de cada sensor.
    * **Componentes Relacionados:** Sensores Ultrasónicos HC-SR04 (o similar).

* **Control de Motores (Movimiento):**
    * Las funciones `avanzar()`, `detener()`, y `acelerar_desacelerar()` manejan el movimiento del robot.
    * El módulo **L298N** se utiliza para controlar la dirección (adelante/atrás) y la velocidad de los motores DC (mediante PWM en el pin ENA).
    * Actualmente, la lógica de giro es básica; se prevé una implementación más avanzada de "giro real" que podría involucrar la integración de un IMU o encoders para mayor precisión en la navegación.
    * **Componentes Relacionados:** Módulo Controlador de Motor L298N, Motores DC del chasis.
    ![Controlador de Motor L298N](./schemes/l298n.jpg)

* **Sensor IR (Detección de Línea/Color):**
    * La función `get_color()` lee el valor analógico de un sensor infrarrojo, lo que puede ser utilizado para detectar diferencias en el color de la superficie (como una línea) o la presencia de objetos cercanos.
    * **Componentes Relacionados:** Sensor IR analógico (fotodiodo/fototransistor con resistencia) conectado al pin `A0`.

* **Placa Controladora Principal:**
    * La placa **Arduino UNO** es el cerebro que procesa todas las lecturas de los sensores y envía las señales de control a los motores.
    * **Componentes Relacionados:** Placa Arduino UNO.
    ![Placa Arduino UNO](./schemes/arduino_uno.jpeg)

* **Sensor IMU (Pendiente de Integración Completa):**
    * Aunque el sensor MPU6050 ha sido documentado (ver imagen abajo), su integración completa para "lógica de giro real" o control de estabilidad aún está pendiente de desarrollo en este fragmento de código. Su función sería proporcionar datos de aceleración y giroscopio para mejorar la navegación.
    * **Componentes Relacionados:** Sensor MPU6050.
    ![Sensor MPU6050](./schemes/mpu6050.jpeg)

### Proceso para Construir/Compilar/Subir el Código:

Para utilizar este código en el vehículo, sigue los pasos estándar del Arduino IDE:
1.  **Instalación de Librerías:** Asegúrate de tener instalada la librería `NewPing` en tu Arduino IDE. Puedes hacerlo desde el Gestor de Librerías (`Sketch > Include Library > Manage Libraries...`).
2.  **Abrir el Sketch:** Abre el archivo `main_robot_control.ino` (o como lo hayas nombrado) en el Arduino IDE.
3.  **Configuración de Placa y Puerto:** En el Arduino IDE, selecciona la placa "Arduino UNO" en `Tools > Board` y el puerto COM/Serial correcto al que esté conectado tu Arduino en `Tools > Port`.
4.  **Compilar:** Haz clic en el botón "Verify" (verificar) para compilar el código y comprobar errores de sintaxis.
5.  **Subir:** Haz clic en el botón "Upload" (subir) para cargar el programa compilado a la placa Arduino UNO conectada.

## Galería del Vehículo

Aquí se muestran diferentes vistas de nuestro robot.

### Vista Frontal
![Vista Frontal del Robot](./v-photos/foto_frontal.jpg)

### Vista Superior
![Vista Superior del Robot](./v-photos/foto_arriba.jpg)

### Vista de Perfil
![Vista de Perfil del Robot](./v-photos/foto_perfil_robot.jpg)

### Vista Trasera
![Vista Trasera del Robot](./v-photos/foto_trasera_robot.jpg)

### Vista Inferior (Pendiente de añadir)
![Vista Inferior del Robot](./v-photos/vehicle_bottom.jpg)

### Otra Vista Lateral (Pendiente de añadir)
![Otra Vista Lateral del Robot](./v-photos/vehicle_side_2.jpg)