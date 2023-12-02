# SEYDTR
INDICE

## Introducción
El objetivo de la práctica es diseñar y programar una máquina expendedora con varias funcionalidades. Para implementar la práctica se usará Arduino UNO y todos los conocimientos dados en la asignatura Sistemas Empotrados y de Tiempo Real. 

Componentes:
- [Placa Arduino UNO](https://docs.arduino.cc/resources/datasheets/A000066-datasheet.pdf)
- [Protoboard](https://agelectronica.lat/pdfs/textos/P/PROTO-BOARD.PDF)
- 2 LEDs
- LCD 16x2
- Joystick
- Sensor DHT11
- Sensor de ultrasonidos
- Botón
- 3 resistencias de 330Ω y una de 2kΩ

El esquema del circuito es el siguiente:
AÑADIR IMAGEN FRIZTING

A la hora de conectar los componentes es necesario establecer algunas cosas de antemano. Uno de los LEDs debe estar puesto en un pin PWM para poder así encenderlo con distintas intensidades. Los dos botones, tanto el joystick como el botón normal tiene que estar en un pin digital y poder así declaralo como pull-up, gracias a esto nos ahorramos poner más componentes en la protoboard.
```c
pinMode(BUTTON_PIN, INPUT_PULLUP);
pinMode(JOY_BUTTON, INPUT_PULLUP);
```
## Librerías y Arranque
Se han usado las siguientes librerías correspondientes al LCD, sensor DHT11, Threads, controladores y finalmente el wachdog. La librerías del wachdog no ha sido necesaria intalar, el resto se encuentran en el enunciado.

```c
#include <LiquidCrystal.h>
#include <DHT.h>
#include <Thread.h>
#include <StaticThreadController.h>
#include <ThreadController.h>
#include <avr/wdt.h>
```

Para la función de arranque no había problema en usar [delay()](https://www.arduino.cc/reference/en/language/functions/time/delay/) porque luego no afectará a la reactividad del programa. Comentar también que se ha establecido el puesto seria a 9600.

## Servicio
### Thread Ultrasonidos
Para poder saber si se encuentra un cliente a menos de un metro he usado un Thread con un tiempo de dos segundos. Con este thread lo que hago es saber la distancia a la que se encuentra el cliente y depende de eso se queda en un estado de suspensión o muestra el menú principal. Implementación del Thread:
```c
void setup(){
  ult_Thread.enabled = true;
  ult_Thread.setInterval(2000);
  ult_Thread.onRun(callback_ultrasonic);
}

void loop(){
  // Check ultrasound
  if (ult_Thread.shouldRun()) {
    ult_Thread.run();
  }
}
```

cosas que poner que no se me olviden:
delay(500) evitar rebots
se queda pillado -> wachdog
controlador mismo tiempo -> hilar y se ejecutan las dos 
no cambiar timer0 pq sino milis hace pum
no poner dos interrupciones en un mismo pin pq sino hace pum
si tengo dos interrupciones en la cola y las desactivo se ejecutan?















