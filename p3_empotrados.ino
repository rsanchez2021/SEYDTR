#include <LiquidCrystal.h>
#include <DHT.h>
#include <Thread.h>
#include <StaticThreadController.h>
#include <ThreadController.h>

Thread myThread = Thread();

// DHT11
#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LED1 -> PWM
const int LED1_PIN = 10;
const int LED2_PIN = A2;

// BUTTON
const int BUTTON_PIN = 9;

// Ultrasonic
const int Trigger = 7;
const int Echo = 6;
long t , d; // Tiempo y distancia
bool sleep_mode = false;
const int Distance = 20; // Distancia cliente

//Crear el objeto LCD con los números correspondientes (rs, en, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Joystick
int JOY_BUTTON = 8;
int X_PIN = A1;
int Y_PIN = A0;

// VARIABLES
int buttonState = 0;
int joyState = 0;
int sum_random = 0;
int intense = 0;
int position = 1;
float coffe1 = 1;
float coffe2 = 1.10;
float coffe3 = 1.25;
float coffe4 = 1.50;
float coffe5 = 2.00;
long randNumber;

void setup() {
  Serial.begin(9600);    //iniciar puerto serie
  dht.begin();
  // LEDs
  pinMode(LED1_PIN , OUTPUT);
  pinMode(LED2_PIN , OUTPUT);
  // BUTTON
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  //Ultrasonic
  pinMode(Trigger, OUTPUT);
  pinMode(Echo, INPUT);
  digitalWrite(Trigger, LOW);
  // Inicializar el LCD con el número de  columnas y filas del LCD
  lcd.begin(16, 2);
  // Joystick
  pinMode(JOY_BUTTON, INPUT_PULLUP);

  lcd.clear();

  // Thread
  myThread.enabled = true;
  myThread.setInterval(2000);
  myThread.onRun(callback_ultrasonic);
  // Boot function
  //Boot();

}

void callback_ultrasonic(){

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);

  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia en cm
  if(d >= Distance){
    sleep_mode = true;
  }
  else{
    sleep_mode = false;
  }
  return sleep_mode;
  // Comprobar la distancia, si la distancia es mayor de 1m -> modo suspnsión sino vuelvo al loop y muestro menú
}

void loop(){
  // Compruebo el ultrasonido
  if(myThread.shouldRun()){
    myThread.run();
  }

  lcd.clear();

  if(sleep_mode == true){
    lcd.print("ESPERANDO CLIENTE");
  }

  else if (sleep_mode == false){
    // Compruebo si el joystick se ha movido hacia un lado o hacia otro
    // Se ha movido hacia la derecha
    if (analogRead(Y_PIN) < 300){
      position += 1;
      delay(500); // Añadimos un delay para que no explote
      if (position > 5){
        position = 1;
      }
    }
    // Se ha movido el joystick a la izquierda
    else if (analogRead(Y_PIN) > 700){
      position -= 1;
      delay(500); // Añadimos un delay para que no explote
      if (position < 1){
        position = 5;
      }
    }
    joyState = digitalRead(JOY_BUTTON);
    // PULLUP -> LOW
    if (joyState == LOW){
      serve_coffe();
    }

    main_menu();
  }
}

void main_menu(){
  switch (position) {
    case 1:
    //show cafe 1€
      lcd.print("Cafe solo");
      lcd.setCursor(0,1);
      lcd.print(coffe1);
      break;
    case 2:
      lcd.print("Cafe cortado");
      lcd.setCursor(0,1);
      lcd.print(coffe2);
      break;
    case 3:
      lcd.print("Cafe doble");
      lcd.setCursor(0,1);
      lcd.print(coffe3);
      break;
    case 4:
      lcd.print("Cafe premium");
      lcd.setCursor(0,1);
      lcd.print(coffe4);
      break;
    case 5:
      lcd.print("Chocolate");
      lcd.setCursor(0,1);
      lcd.print(coffe5);
      break;
  }
}

void Boot(){
  lcd.print("CARGANDO...");
  for (int i = 1; i < 3; i++){
    Serial.print(i);
    digitalWrite(LED1_PIN , HIGH);
    delay(1000);
    digitalWrite(LED1_PIN , LOW);
    delay(1000);
  }
}

void serve_coffe(){
  randNumber = random(4,9); // select random number between 4 and 8
  Serial.println(randNumber);
  sum_random = 255 / randNumber; // Lo que tiene que sumar cada segundo
  intense = 0;
  t = 0;
  while (t < randNumber){
    digitalWrite(LED1_PIN, intense);
    intense += sum_random;
    t += 1;
  }
}

