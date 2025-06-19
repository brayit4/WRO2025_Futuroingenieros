# Introducción.

El equipo DB Engineers se enorgullece en presentar a "Chola", un robot que representa nuestro esfuerzo de diseño y desarrollo de un sistema autónomo capaz de actuar de manera inteligente con su entorno, que permite demostrar integrar diversas tecnologías de control, para buscar la eficiencia y precisión en cada una de las tareas que debe ejecutar.
Durante el desarrollo de este proyecto, afrontamos desafíos que requirieron idear e implementar soluciones para dar vida a nuestro robot, desde su arquitectura de software y hardware hasta su capacidad para navegar y ejecutar acciones específicas. Chola es más que un robot; es una prueba de nuestro compromiso con la ingeniería y la innovación, creado para superar obstáculos y aprender de su interacción con el mundo.

## El robot Chola
El sistema de control adoptado para nuestro robot, ha sido desarrollado en el entorno de programación Arduino IDE, utilizando para ello la placa Arduino GIGA como controlador principal, permitiendo una interacción eficiente con el hardware del robot, sustentando su operación autónoma
El núcleo de este sistema se ubica en el archivo src/main_robot_control.ino. Este código está diseñado para gestionar los subsistemas críticos del robot, que incluyen:
    - Medición de distancia: Esencial para la navegación y la prevención de que el robot este muy cerca de la pared y pueda alterar su funcionamiento chocándose o alterando su velocidad, para ello usaremos un sensor ultrasonido HC-SR04.
    - Control preciso del movimiento: Usaremos PID para mantener el robot estable que no se desvié manteniéndose siempre en el medio de la pista, esto hara que no colisione contra la pared, para ello usaremos un giroscopio GY-9960.
    - Detección de líneas de color azul y naranja: Clave para reconocer si el robot está girando en sentido horario o antihorario, e identificar el color de los pilares y la zona de estacionamiento, para lograr esto usaremos una cámara PIXY 2.
La estructura del código se organiza en una serie de funciones clave. Cada una de estas funciones ha sido creada para interactuar de manera directa y específica con los diversos componentes electromecánicos del robot, asegurando una comunicación fluida y un control óptimo sobre cada aspecto de su desempeño.

### Controlador de Movimiento: Motor Paso a Paso 5V
Para controlar el movimiento angular preciso del robot, se ha decidido utilizar un motor paso a paso de 5V en lugar de un servo tradicional. Aunque los servos son excelentes para posicionamientos rápidos, se ha descubierto que un motor paso a paso ofrece una precisión de posicionamiento superior y la capacidad de mantener una posición sin requerir una señal PWM constante. Esto es ideal para su sistema de dirección, que necesita un control muy exacto, como el de un sistema Ackermann. Este tipo de motor es perfecto cuando la estabilidad y la precisión del ángulo son más importantes que la velocidad extrema.
### Especificaciones Clave del 28BYJ-48
 ![Eje trasero](./schemes/Motor_stepper)
    - Voltaje de Operación: Necesita alimentarlo con 5V DC. Es fundamental asegurarse de que la fuente de alimentación para el controlador del motor paso a paso sea estable a 5V para que funcione perfectamente.
    - Ángulo de Paso: Típicamente, el ángulo de paso es de 1.8 a 7.5 grados por paso. Esto es lo que le da la resolución del movimiento; cuanto más pequeño sea el ángulo, mayor será la precisión que puede lograr.
    - Corriente por Fase: Esta varía según el modelo, pero es vital que su controlador de motor paso a paso pueda suministrar la corriente adecuada para el torque que necesita.
    - Torque (Par): Aunque generalmente tiene menos torque que un servo MG995, el torque de el motor paso a paso de 5V es suficiente para la dirección en su robot.
    - Número de Fases: Usualmente son bipolares o unipolares, lo cual influye en cómo lo cablea y qué tipo de controlador necesita.
El motor paso a paso de 5V es ideal para el sistema de dirección Ackermann porque su capacidad de posicionamiento exacto y repetible es clave para manipular los ángulos de las ruedas con mucha efectividad. A diferencia de un servo, puede detenerlo en cualquier paso intermedio, lo que le da un control incremental y muy preciso.

### Posibles Mejoras:
    - Motores Paso a Paso con Mayor Torque: Si el robot necesita mover mecanismos más pesados o resistir fuerzas externas mayores en el futuro, se consideraría usar un motor paso a paso con un torque nominal más alto. Esto probablemente implicaría un mayor voltaje de operación o corriente, lo que a su vez requeriría un controlador de motor más robusto.
    - Protección contra el Agua y el Polvo: Dependiendo de dónde opere el robot, se beneficiaría mucho usar motores paso a paso sellados o construir carcasas protectoras alrededor de ellos. Esto aumentaría su fiabilidad y vida útil al protegerlos de la humedad y la suciedad.
Lo puedes conseguir aquí: 
Amazon.com: WWZMDiB 28BYJ-48 ULN2003 Kit de motor paso a paso y placa de conductor de 5 V compatible con Arduino Raspberry Pi (3 piezas) : Electrónica

## Microcontrolador: Arduino GIGA R1 WiFi
En la valida anterior, nos apoyábamos en un ARDUINO UNO para gestionar los diversos componentes del robot. Para esta competencia, buscamos una solución más potente y consolidada, por lo que confiamos en el Arduino GIGA R1 WiFi, que es microcontrolador el cual es un avance significativo, al combinar una gran capacidad de procesamiento con conectividad integrada, que lo hace ideal para manejar todos los sensores y actuadores del robot de manera más optimizada y eficiente.
## Especificaciones Clave del Arduino GIGA R1 WiFi:
![Eje trasero](./schemes/Arduino_GIGA)
    - Microcontrolador: STM32H747XI (Dual-core, Cortex-M7 a 480 MHz y Cortex-M4 a 240 MHz)
    - Memoria Flash: 2 MB
    - SRAM: 1 MB
    - Frecuencia del reloj: Hasta 480 MHz (Cortex-M7)
    - Pines: 76 pines digitales I/O (incluyendo 12 analógicos, 20 PWM, 4 UART, 3 SPI, 3 I2C, entre otros).
    - Voltaje de entrada (VIN): 6-24V
    - Capacidades Adicionales: Wi-Fi y Bluetooth integrados, USB-C, conector JTAG.
## Posibles Mejoras:
    - Diseño de un PCB personalizado: Crear una placa de circuito impreso (PCB) a medida que integre directamente el Arduino GIGA R1 WiFi con otros componentes esenciales. Esto reduciría el cableado, optimizaría el espacio y ofrecería una configuración más limpia y profesional.

[Aqui lo encontraras](https://www.amazon.com/Arduino-Giga-R1-WiFi-ABX00063/dp/B0BTTRZ9TB)  [text](https://www.amazon.com/Arduino-Giga-R1-WiFi-ABX00063/dp/B0BTTRZ9TB)

## Unidad de Medición Inercial (IMU): MPU-9250
Un aspecto c que ayuda a la navegación del robot es el sensor MPU-9250, que es un IMU de 9 ejes altamente versátil, que integra un acelerómetro de 3 ejes, un giroscopio de 3 ejes y un magnetómetro de 3 ejes en un único chip. 
Aunque el MPU-9250 es capaz de proporcionar datos de aceleración y orientación magnética, actualmente nos enfocamos en utilizar la velocidad angular proporcionada por el giroscopio para calcular el ángulo de orientación del robot. Esta información es esencial para rastrear los giros y obtener en función de eso el ángulo de desviación YAW (ángulo en Z) que representa las deviaciones de lado a lado de forma horizontal, permitiendo al robot comprender su orientación espacial y ajustar su trayectoria.
## Especificaciones Clave del IMU MPU-9250:
![Eje trasero](./schemes/IMU_sensor)
    - Rango del Giroscopio: Seleccionable entre ±250, ±500, ±1000, ±2000 °/s
    - Rango del Acelerómetro: Seleccionable entre ±2g, ±4g, ±8g, ±16g
    - Rango del Magnetómetro: ±4800 µT (microtesla)
    - Interfaz de Comunicación: I2C (también soporta SPI)
    - Voltaje de Alimentación: 2.4V a 3.6V (comúnmente se usa con módulos breakout de 3.3V o 5V con regulador)
## Posibles Mejoras:
    - Implementación de Fusión de Sensores (Acelerómetro + Giroscopio + Magnetómetro): Actualmente, solo utilizamos el giroscopio. Una mejora fundamental sería aplicar algoritmos de fusión de sensores (como el filtro de Kalman o el filtro complementario). Al combinar los datos de los tres componentes (acelerómetro, giroscopio y magnetómetro), obtendríamos una estimación mucho más precisa y robusta de la orientación absoluta (cabeceo, alabeo y rumbo) del robot, compensando la deriva del giroscopio y siendo más resistente a las interferencias.
    - Mitigación de Ruido y Vibraciones: Aunque el MPU-9250 es un buen sensor, en entornos con vibraciones significativas del robot, se pueden implementar filtros digitales adicionales en el software (ej., promedios móviles, filtros de paso bajo) para suavizar las lecturas y obtener datos más estables para los cálculos de orientación.

[aqui lo encontraras](https://www.amazon.com/-/es/MPU-9250-MPU9250-Giroscopio-aceleración-llegada/dp/B0DZVC5CG4/ref=sr_1_1?__mk_es_US=ÅMÅŽÕÑ&dib=eyJ2IjoiMSJ9.v2-K_uJllzT5KHT5s_lTDVdParcJ4usfJxIvj8Q8RONAsiDiNizib86p_vkQzXw_S258xHNWHLELeIBfJ2_hlrw7r26-kr8ZsZxv76lkbyQ.g5RCJe4iWob8q1qgMWSCl-Nlf-ys2agpGqLTEbVSZ1o&dib_tag=se&keywords=MPU-9250&qid=1750297365&sr=8-1)


## Construcción Mecánica y Eléctrica
El proyecto Chola basa la estructura de su chasis en un Kit de carros autónomos adquirido a través de Alibaba: Encuéntralo aquí 


[Aquí encontrará un tutorial del armado original del chasis](https://youtu.be/ulTM1uV1Bvg?si=uPVaVt-tIMoJ6x8m)

A pesar de ser un chasis adquirido, este fue sometido a algunas modificaciones con el fin de adaptarlo mejor a las necesidades y los lineamientos de la competencia. Es por ello que decidimos realizar un doblez a la platina principal del chasis en 90 grados hacia arriba:

![Platina despues](./others/piezas/Platinadespues.png)

----

![Insytrucciones](./others/piezas/Platinadobaldo.png)

El kit incluye otras piezas de plástico que fueron usadas de manera particular para este proyecto, específicamente las siguientes piezas:
![Piezas direccion](./others/piezas/Resinaepoxi.jpg)

Estas deberán unirse de la siguiente manera: Haciendo uso de resina epoxica mantenemos las piezas unidas de manera firme y resistente a impactos durante las pruebas de funcionamiento. También aprovechamos los agujeros para tornillos que tienen estas piezas para fijar nuestro microcontrolador principal:
[Insertar imagen del microcontrolador fijado aquí]

## Sistema de Dirección
El kit original venía con un sistema de dirección basado en un servomotor y algunas piezas para crear un sistema de dirección Ackerman. Este diseño fue una elección conveniente, no solo porque ya estaba incluido en el kit, sino también porque la configuración Ackerman es ideal para el propósito de la competencia. Permite que el vehículo tenga un sobreviraje controlado, lo que facilita giros más rápidos y eficientes, optimizando así el rendimiento en la competencia.



[Aquí encontrará un tutorial del armado original del chasis](https://youtu.be/ulTM1uV1Bvg?si=uPVaVt-tIMoJ6x8m)



Imágenes de las piezas: 
![Piezas direccion](./others/piezas/piezassistema.jpg)


### Sin embargo, a pesar de las ventajas del sistema ya incluido con el kit, por motivos de precisión y durabilidad, decidimos realizar cambios a este sistema:
- Crear una placa de Baquelita de 2.5cm por 8.5cm.
- Utilizar un motor tipo stepper 28BYJ-48 de 5v.
- Utilizar una cobertura de motor circular.
- Perforar la cobertura para el motor.

Si se hicieron todos los pasos de manera correcta este sera el resultado:
![Piezas direccion](./others/piezas/Sistema_direccion.jpg)

## Medidas generales

Las ruedas traseras vienen incluidas en el kit , tienen unas medidas de 6cm de diámetro y 3cm de grosor 
![Rueda trasera](./others/piezas/ruedasmedidas.png)

Las ruedas delanteras son ruedas de para lego genéricas con medidas de 2.5cm de grosor y 4 cm de diámetro.
![rueda delante](./others/piezas/ruedasdelante1.jpg)

El eje trasero mide 8.5cm de longitud y el eje delantero mide 6.4cm con un volado delantero de 5cm.
![Eje trasero](./others/piezas/ejetrasero.jpg)

La distancia entre ejes de 15.5 cm 


 - Las medidas totales de Chola con 21cm de largo y 8cm de ancho sin tomar en cuenta las ruedas , con estas el groso se eleva a 15 cm 
## Sistema de motor
 - El motor usado es un motor DC de 3v a 12v genérico , con un piñón de 12 dientes. 
Para transmitir el movimiento a las ruedas traseras , se engrana el motor a un piñón de 45 dientes , este último está ubicado en el eje de las ruedas y transmite el movimiento de los motores en una relación de 3,75

- Lo cual todo esto nos deja una transmisión de 1500rpm a 375 rpm de el motor a el eje.

## Circuito electrico




