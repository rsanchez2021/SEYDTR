int analogPin = A0;
int LED_PIN = 6; // Pin con ~

void setup() {
  Serial.begin(9600);
  pinMode(analogPin, input);
  pinMode(LED_PIN, output)
  // put your setup code here, to run once:

}

void loop() {
  unsigned int val = analogRead(analogPin);

  Serial.println(val);
  int res = (val/4); // pasa de 1024 a 255 como máximo analogico -> digital
  Serial.println(res);

  analogWrite(LED_PIN, res);
  delay(100)

  // put your main code here, to run repeatedly:
}
// Si quiero hacer dos lecturas con diferente delay puedo hacer dos cosas:
// 1. Calcular los delay para que funcione
// 2. Cre tres variables que serán el tiempo para hacer espera activa
/*
t1, t2, t3 = milis()

loop()

if (milis() - t1 > 0):
  sensar
  t1 = milis()

problema:
Muchos if -> kk
*/