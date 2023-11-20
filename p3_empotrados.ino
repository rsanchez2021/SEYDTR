#include <LiquidCrystal.h>

const int LED1_PIN = 10;
const int LED2_PIN = A2;
//Crear el objeto LCD con los números correspondientes (rs, en, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


void setup() {
  Serial.begin(9600);    //iniciar puerto serie
  pinMode(LED1_PIN , OUTPUT);  //definir pin como salida
  pinMode(LED2_PIN , OUTPUT);
  // Inicializar el LCD con el número de  columnas y filas del LCD
  lcd.begin(16, 2);
  lcd.print("Hola Mundo");
  lcd.print("Hola Mundo");// Se escribe de seguido
}
 
void loop(){
  digitalWrite(LED1_PIN , HIGH);   // poner el Pin en HIGH
  digitalWrite(LED2_PIN , HIGH);
  delay(1000);                   // esperar un segundo
  digitalWrite(LED1_PIN , LOW);    // poner el Pin en LOW
  digitalWrite(LED2_PIN , LOW);
  delay(1000);                   // esperar un segundo
}