#include <LiquidCrystal.h>
#include <DHT.h>
#include <Thread.h>
#include <StaticThreadController.h>
#include <ThreadController.h>
#include <TimerOne.h>

ThreadController controller = ThreadController();
Thread tem_Thread = Thread();
Thread hum_Thread = Thread();
Thread ult_Thread = Thread(); //Thread del ultrasonidos

// DHT11
#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LED2 -> PWM
const int LED1_PIN = A2;
const int LED2_PIN = 10;

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
byte customChar[] = {
  B00000,
  B01110,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000
};

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
int position_admin = 1;
int time = 0;
int distance = 0;
float temperature = 0;
float humidity = 0;

String name1 = "Cafe solo";
float coffe1 = 1;
String name2 = "Cafe cortado";
float coffe2 = 1.10;
String name3 = "Cafe doble";
float coffe3 = 1.25;
String name4 = "Cafe premium";
float coffe4 = 1.50;
String name5 = "Chocolate";
float coffe5 = 2.00;

long randNumber;

bool show_t_h = true;
bool admin = true;
bool loop_tem_hum = false;
bool loop_sen = false;
bool loop_count = false;

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
  lcd.createChar(0, customChar);

  // Joystick
  pinMode(JOY_BUTTON, INPUT_PULLUP);

  // Thread ultrasonic (no en el controlador)
  ult_Thread.enabled = true;
  ult_Thread.setInterval(2000);
  ult_Thread.onRun(callback_ultrasonic);

  tem_Thread.enabled = true;
  tem_Thread.setInterval(500); //Cada medio segundo
  tem_Thread.onRun(callback_temperature);

  hum_Thread.enabled = true;
  hum_Thread.setInterval(550); //Cada medio segundo
  hum_Thread.onRun(callback_humidity);

  controller.add(&tem_Thread);
  controller.add(&hum_Thread);

  lcd.clear();

  // Boot function
  // Boot();
  
}

void callback_ultrasonic(){

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);

  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia en cm
  if(d >= Distance){
    sleep_mode = true;
    show_t_h = true; //cuando vuelva el cliente se mostrará la temeperatura
  }
  else{
    sleep_mode = false;
  }
  return sleep_mode;
  // Comprobar la distancia, si la distancia es mayor de 1m -> modo suspnsión sino vuelvo al loop y muestro menú
}

void callback_temperature(){
  Serial.println("Temperatura");
  lcd.setCursor(0,0);
  temperature = dht.readTemperature();
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C");
}

void callback_humidity(){
  humidity = dht.readHumidity();
  lcd.setCursor(0,1);
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.print("%");
}

void loop(){
  loop_admin(); // Meter una variable, si es verdad ir a admin con interrupciones

  // Compruebo el ultrasonido
  if(ult_Thread.shouldRun()){
    ult_Thread.run();
  }

  lcd.clear();

  if(sleep_mode == true){
    lcd.setCursor(0,0);
    lcd.print("ESPERANDO");
    lcd.setCursor(0, 1);
    lcd.print("CLIENTE");
  }

  else if (sleep_mode == false){
    // Muestro la temperatura si vuelvo al menú
    if (show_t_h == true){
      show_tem_hum();
    }

    // Compruebo si el joystick se ha movido hacia un lado o hacia otro
    // Se ha movido hacia arriba
    if (analogRead(Y_PIN) < 300){
      position += 1;
      delay(500); // Añadimos un delay para que no explote
      if (position > 5){
        position = 1;
      }
    }
    // Se ha movido el joystick abajo
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

void show_tem_hum(){
  long endTime = millis() + 5000;
  // Lo meto dentro del while para que se actualice
  while( millis() < endTime){
    float humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    lcd.setCursor(0,0);
    lcd.print("Hum: ");
    lcd.print(humidity);
    lcd.setCursor(0,1);
    lcd.print("Temp: ");
    lcd.print(temperature);
  }
  show_t_h = false;
}

void main_menu(){
  switch (position) {
    case 1:
      show_menu(name1,coffe1);
      break;
    case 2:
      show_menu(name2,coffe2);
      break;
    case 3:
      show_menu(name3,coffe3);
      break;
    case 4:
      show_menu(name4,coffe4);
      break;
    case 5:
      show_menu(name5,coffe5);
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
  lcd.print("PREPARANDO");
  lcd.setCursor(0,1);
  lcd.print("CAFE ...");
  
  for( int t = 0; t < randNumber; t++){
    analogWrite(LED2_PIN, intense);
    intense += sum_random;
    delay(1000);
  }
  analogWrite(LED2_PIN, 0);
  lcd.clear();
  lcd.print("RETIRE BEBIDA");
  delay(3000);
}

void show_menu(String name, float price){
  lcd.print(name);
  lcd.setCursor(0,1);
  lcd.print(price);
}

void loop_admin(){
  analogWrite(LED2_PIN, 255);
  digitalWrite(LED1_PIN, HIGH);
  while (admin){

    lcd.clear();

    if (analogRead(Y_PIN) < 300){
      position_admin += 1;
      delay(500); // Añadimos un delay para que no explote
      if (position_admin > 4){
        position_admin = 1;
      }
    }
    // Se ha movido el joystick abajo
    else if (analogRead(Y_PIN) > 700){
      position_admin -= 1;
      delay(500); // Añadimos un delay para que no explote
      if (position_admin < 1){
        position_admin = 4;
      }
    } // Joysticj hacia la izquierda -> mover de aquí

    joyState = digitalRead(JOY_BUTTON);
    // PULLUP -> LOW
    if (joyState == LOW){
      switch (position_admin) {
        case 1:
          loop_tem_hum = true;
          lcd.clear();
          tem_hum_loop();
          break;
        case 2:
          loop_sen = true;
          lcd.clear();
          sen_loop();
          break;
        case 3:
          loop_count = true;
          count_loop();
          break;
        case 4:
          //price_loop();
          break;
  }
    }
    admin_menu();
  }
}


void admin_menu(){
  switch (position_admin) {
    case 1:
      lcd.print("Ver Temperatura");
      break;
    case 2:
      lcd.print("Ver distancia");
      lcd.setCursor(0,1);
      lcd.print("sensor");
      break;
    case 3:
      lcd.print("Ver contador");
      break;
    case 4:
      lcd.print("Modificar precios");
      break;
  }
}

void tem_hum_loop(){
  while (loop_tem_hum){
    controller.run();
    if (analogRead(X_PIN) < 300){
      loop_tem_hum = false;
    }
  }
}

void sen_loop(){
  while (loop_sen){
    lcd.clear();
    digitalWrite(Trigger, HIGH);
    delayMicroseconds(10);          //Enviamos un pulso de 10us
    digitalWrite(Trigger, LOW);

    t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
    d = t/59;

    lcd. print("Distancia: ");
    lcd.setCursor(0,1);
    lcd.print(d);
    lcd.print("cm");

    if (analogRead(X_PIN) < 300){
      loop_sen = false;
    }
  }
}

void count_loop(){
  while (loop_count){
    lcd.clear();

    long elapsedTime = millis()/ 1000; // elapsed time in seconds

    int hours = elapsedTime / 3600;
    int minutes = (elapsedTime % 3600) / 60;
    int seconds = elapsedTime % 60;

    lcd.print(hours);
    lcd.print(" hours ");
    lcd.print(minutes);
    lcd.print(" min ");
    lcd.setCursor(0,1);
    lcd.print(seconds);
    lcd.print(" sec");

    if (analogRead(X_PIN) < 300) {
        loop_count = false;
    }
  }
}
