#include <LiquidCrystal.h>
#include <DHT.h>
#include <Thread.h>
#include <StaticThreadController.h>
#include <ThreadController.h>
#include <TimerOne.h>
#include <avr/wdt.h>

ThreadController controller = ThreadController();
Thread tem_Thread = Thread();
Thread hum_Thread = Thread();
Thread ult_Thread = Thread();

// DHT11
#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LED2 -> PWM
const int LED1_PIN = A2;
const int LED2_PIN = 10;

// BUTTON -> Pin 2 Interrupciones
const int BUTTON_PIN = 2;

// Ultrasonic
const int Trigger = 7;
const int Echo = 6;
long t , d; // Tiempo y distancia
bool sleep_mode = false;
const int Distance = 20; // Distancia cliente

//Crear el objeto LCD con los números correspondientes (rs, en, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 9);

// Joystick
int JOY_BUTTON = 8;
int X_PIN = A1;
int Y_PIN = A0;

// VARIABLES
int joyState = 0;
int sum_random = 0;
int intense = 0;
int position = 1;
int position_admin = 1;
int position_price = 1;
float temperature = 0;
float humidity = 0;
int buttonState = 0;

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
long elapsedTime;
long time;
long time_pressed;

bool show_t_h = true;
bool price = false;
bool loop_tem_hum = false;
bool loop_sen = false;
bool loop_count = false;
bool price_loop = false;
bool admin = true;

// Variables en interrupciones
volatile bool one_time = false;
volatile long time2_pressed;

void setup() {
  Serial.begin(9600);    //iniciar puerto serie
  wdt_disable();
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

  Boot();
  image.png

  // Interrupciones
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_h_isr, RISING);
  
  wdt_enable(WDTO_8S);
}

void callback_ultrasonic(){

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);  // Send a 10us pulse
  digitalWrite(Trigger, LOW);

  t = pulseIn(Echo, HIGH);  // Get the pulse width
  d = t/59;  // Scale time to distance in cm
  if(d >= Distance){
    sleep_mode = true;
    show_t_h = true;  // When the customer returns, the temperature will be displayed
  } else{
    sleep_mode = false;
  }
  return sleep_mode;
}

void callback_temperature(){
  lcd.setCursor(0, 0);
  temperature = dht.readTemperature();
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.write(223);  // Show º
  lcd.print("C");
}

void callback_humidity(){
  humidity = dht.readHumidity();
  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.print("%");
}

void button_h_isr(){
  time2_pressed = millis();
  one_time = true;
}

void loop(){
  buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && one_time == true){
    time_pressed = millis();
    one_time = false;
  }

  if (time2_pressed - time_pressed > 5000){
    admin = true;
    time2_pressed = 0;
    time_pressed = 0;
    loop_admin();
  } else if (time2_pressed - time_pressed < 3000 && time2_pressed - time_pressed > 2000){
    Boot();
  }

  // Check ultrasound
  if(ult_Thread.shouldRun()){
    ult_Thread.run();
  }

  lcd.clear();

  if(sleep_mode == true){
    lcd.setCursor(0,0);
    lcd.print("ESPERANDO");
    lcd.setCursor(0, 1);
    lcd.print("CLIENTE");
    wdt_reset();
  } else if (sleep_mode == false){
    // Display temperature if returning to the menu
    if (show_t_h == true){
      show_tem_hum();
    }

    // Check if the joystick has moved left or right
    joystick_state();

    joyState = digitalRead(JOY_BUTTON);
    // PULLUP -> LOW
    if (joyState == LOW){
      serve_coffe();
    }
    wdt_reset();
    main_menu();
  }
}

void joystick_state(){
  if (analogRead(Y_PIN) < 300){
    position += 1;
    position_admin += 1;
    if (position > 5){
      position = 1;
    } else if (position_admin > 4){
      position_admin = 1;
    }
    delay(500);
  } else if (analogRead(Y_PIN) > 700){
    position -= 1;
    position_admin -= 1;
    if (position < 1){
      position = 5;
    } else if (position_admin < 1){
      position_admin = 4;
    }
  }
}

void show_tem_hum(){
  long endTime = millis() + 5000;

  while( millis() < endTime){
    float humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    lcd.setCursor(0, 0);
    lcd.print("Hum: ");
    lcd.print(humidity);
    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(temperature);
  }
  show_t_h = false;
}

void main_menu(){
  switch (position) {
    case 1:
      show_menu(name1, coffe1);
      break;
    case 2:
      show_menu(name2, coffe2);
      break;
    case 3:
      show_menu(name3, coffe3);
      break;
    case 4:
      show_menu(name4, coffe4);
      break;
    case 5:
      show_menu(name5, coffe5);
      break;
  }
}

void Boot(){
  lcd.print("CARGANDO...");
  for (int i = 1; i < 3; i++){
    digitalWrite(LED1_PIN , HIGH);
    delay(1000);
    digitalWrite(LED1_PIN , LOW);
    delay(1000);
  }
}

void serve_coffe(){
  randNumber = random(4,9);  // Select random number between 4 and 8
  sum_random = 255 / randNumber;  // The amount to increment each second
  intense = 0;
  lcd.print("PREPARANDO");
  lcd.setCursor(0, 1);
  lcd.print("CAFE ...");
  
  for( int t = 0; t < randNumber; t++){
    analogWrite(LED2_PIN, intense);
    intense += sum_random;
    delay(1000);
    wdt_reset();
  }
  analogWrite(LED2_PIN, 0);
  lcd.clear();
  lcd.print("RETIRE BEBIDA");
  delay(3000);
}

void show_menu(String name, float coffe){
  lcd.print(name);
  lcd.setCursor(0, 1);
  lcd.print(coffe);
}

void loop_admin(){
  analogWrite(LED2_PIN, 255);
  digitalWrite(LED1_PIN, HIGH);
  while (admin){
    lcd.clear();

    buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == LOW && one_time == true){
      time_pressed = millis();
      one_time = false;
      time = time2_pressed - time_pressed;
    }
    if (time2_pressed - time_pressed > 5000){
      analogWrite(LED2_PIN, 0);
      digitalWrite(LED1_PIN, LOW);
      time2_pressed = 0;
      time_pressed = 0;
      admin = false;
    } 

    joystick_state();

    joyState = digitalRead(JOY_BUTTON);
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
          price = true;
          loop_price();
          break;
      }
    }
    wdt_reset();
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
    wdt_reset();
  }
}

void sen_loop(){
  while (loop_sen){
    lcd.clear();
    digitalWrite(Trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trigger, LOW);

    t = pulseIn(Echo, HIGH);
    d = t/59;

    lcd. print("Distancia: ");
    lcd.setCursor(0, 1);
    lcd.print(d);
    lcd.print("cm");

    if (analogRead(X_PIN) < 300){
      loop_sen = false;
    }
    wdt_reset();
  }
}

void count_loop(){
  while (loop_count){
    lcd.clear();

    elapsedTime = millis()/ 1000;

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
    wdt_reset();
  }
}

void loop_price(){
  while (price){
    lcd.clear();

    joystick_state();

    if (analogRead(X_PIN) > 700){
      price = false;
    }

    joyState = digitalRead(JOY_BUTTON);
    if (joyState == LOW){
      price_loop = true;
      if (position == 1){
        change_price(name1, coffe1);
      } else if (position == 2){
        change_price(name2, coffe2);
      } else if (position == 3){
        change_price(name3, coffe3);
      } else if (position == 4){
        change_price(name4, coffe4);
      } else if (position == 5){
        change_price(name5, coffe5);
      }
    }
    wdt_reset();
    main_menu();
  }
}

void change_price(String name, float coffe){
  while (price_loop){
    lcd.clear();
    lcd.print(name);
    lcd.setCursor(0, 1);
    lcd.print(coffe);

    if (analogRead(Y_PIN) < 300){
      coffe = coffe + 0.05;
      delay(500);
    } else if (analogRead(Y_PIN) > 700){
      coffe = coffe - 0.05;
      delay(500);
    } else if (analogRead(X_PIN) > 700){
      price_loop = false;
    }

    joyState = digitalRead(JOY_BUTTON);
    if (joyState == LOW){
      if (position == 1){
        coffe1 = coffe;
      } else if (position == 2){
        coffe2 = coffe;
      } else if (position == 3){
        coffe3 = coffe;
      } else if (position == 4){
        coffe4 = coffe;
      } else if (position == 5){
        coffe5 = coffe;
      }
      price_loop = false;
    }
    wdt_reset();
  }
}
