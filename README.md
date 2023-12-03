# Sistemas Empotrados y de Tiempo Real

## Índice
* [Introducción][intro]
* [Librerías y arranque][lib]
* [Servicio][serv]
* [Administrador][admin]
* [Vídeos][vid]

  [intro]: https://github.com/rsanchez2021/SEYDTR/blob/main/README.md#introducción
  [lib]: https://github.com/rsanchez2021/SEYDTR/blob/main/README.md#librerías-y-arranque
  [serv]: https://github.com/rsanchez2021/SEYDTR/blob/main/README.md#servicio
  [admin]: https://github.com/rsanchez2021/SEYDTR/blob/main/README.md#administrador
  [vid]: https://github.com/rsanchez2021/SEYDTR/blob/main/README.md#vídeo


## Introducción
El objetivo de la práctica es diseñar y programar una máquina expendedora con diversas funcionalidades. Para implementar la práctica, se utilizará Arduino UNO y se aplicarán todos los conocimientos adquiridos en la asignatura de Sistemas Empotrados y de Tiempo Real.

Componentes:
- [Placa Arduino UNO](https://docs.arduino.cc/resources/datasheets/A000066-datasheet.pdf)
- Protoboard
- 2 LEDs
- LCD 16x2
- Joystick
- Sensor DHT11
- Sensor de ultrasonidos
- Botón
- 3 resistencias de 330Ω y una de 2kΩ

### Esquema

AÑADIR IMAGEN FRIZTING

Al conectar los componentes, es necesario establecer algunas configuraciones de antemano. Uno de los LEDs debe estar conectado a un pin PWM para permitir el control de su intensidad luminosa. Ambos botones, tanto el joystick como el botón normal, deben estar en un pin digital para poder declararlos como pull-up, lo que ahorra la necesidad de poner más componentes en la protoboard.
```c
pinMode(BUTTON_PIN, INPUT_PULLUP);
pinMode(JOY_BUTTON, INPUT_PULLUP);
```
## Librerías y Arranque
Se han usado las siguientes librerías correspondientes al [LCD](https://github.com/rsanchez2021/SEYDTR/commit/da49937ad703255b1f0c11c830a60694a07a741e), sensor [DHT11](https://github.com/adafruit/DHT-sensor-library), [Threads](https://www.arduino.cc/reference/en/libraries/arduinothread/), controladores y finalmente el Wachdog. La librerías del wachdog no ha sido necesaria intalar.

```c
#include <LiquidCrystal.h>
#include <DHT.h>
#include <Thread.h>
#include <StaticThreadController.h>
#include <ThreadController.h>
#include <avr/wdt.h>
```

Para la función de arranque no había problema en usar [delay()](https://www.arduino.cc/reference/en/language/functions/time/delay/)  ya que no afectará a la reactividad del programa. Cabe destacar que se ha establecido la velocidad de comunicación serial a 9600.

## Servicio
### Thread Ultrasonidos
Se ha implementado un Thread con un tiempo de dos segundos para determinar si hay un cliente a menos de un metro. Este hilo permite suspender el sistema o mostrar el menú principal según la distancia del cliente. Implementación del Thread:
```c
Thread ult_Thread = Thread();

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
### Delay
Se ha intentado reducir al mínimo la utilización de delay() en todo el código, dejando únicamente los delays de los joysticks para evitar rebotes y en la función de servir café. Una mejora potencial del código sería utilizar Timer0/Timer1 para eliminar por completo los delays. En caso de hacer esto, se debe tener precaución al modificar Timer0, ya que la función millis() dejará de funcionar, como se observa en este [código](https://github.com/rsanchez2021/SEYDTR/commit/da49937ad703255b1f0c11c830a60694a07a741e). 

### Interrupciones
En caso de querer acceder al menú de administrador se tiene que pulsar el botón 5 segundos, o si se desea reiniciar la placa, se debe pulsar entre 2 y 3 segundos. Se ha utilizado una interrupción de hardware para este propósito. Cuando se pulsa el botón, se guarda el primer tiempo y se desactiva un booleano para que solo guarde el tiempo inicial. Cuando se suelta el botón, la interrupción salta y guarda el segundo tiempo, evitando rebotes en el botón.  Finalmente, se calcula la diferencia y, según el tiempo transcurrido y dónde se encuentre, se accede a la función admin, se reinicia o se vuelve al loop principal. Se tuvo la intención original de establecer dos interrupciones en el pin del botón, una en RISING y otra en FALLING, pero esto no es posible; se debería usar CHANGE y leer el botón dentro de la interrupción.  Es importante señalar que tanto el segundo tiempo como el booleano se han establecido como volátiles, ya que se modifican en la interrupción. Implementación final de la interrupción:

```c
volatile bool one_time = false;
volatile long time2_pressed;

void setup(){
  ...
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_h_isr, RISING);
}

void button_h_isr() {
  time2_pressed = millis();
  one_time = true;
}

```

## Administrador
### Controlador Threads
Dentro de la función admin, se pide mostrar la temperatura y la humedad de forma dinámica. Para lograr esto, se ha utilizado un controlador de threads que mide estos valores cada medio segundo. Dado que el tiempo de ejecución es el mismo y no pueden ejecutarse simultáneamente, se ejecutan de forma encadenada, garantizando que uno se ejecute inmediatamente después del otro. Implementación del controlador:
```c
ThreadController controller = ThreadController();
Thread tem_Thread = Thread();
Thread hum_Thread = Thread();

void setup(){
  tem_Thread.enabled = true;
  tem_Thread.setInterval(500);
  tem_Thread.onRun(callback_temperature);

  hum_Thread.enabled = true;
  hum_Thread.setInterval(500);
  hum_Thread.onRun(callback_humidity);

  controller.add(&tem_Thread);
  controller.add(&hum_Thread);
}

void tem_hum_loop(){
  controller.run();
}
```
### Wachdog
Se ha utilizado el Watchdog para evitar bloqueos en callbacks, interrupciones o algunas funciones. En cada loop principal, se reinicia el watchdog de 8 segundos para que, si alguna función se queda bloqueada, la placa se reinicie. Implementación del Watchdog:

```c
void setup(){
  wdt_disable();
  ...
  wdt_enable(WDTO_8S);
}

void loop(){
  ...
  wdt_reset();
}

void loop_admin(){
  wdt_reset();
}

void loop_price(){
  wdt_reset();
}
```

## Vídeo 
