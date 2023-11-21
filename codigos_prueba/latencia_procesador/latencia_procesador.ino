delay_milis = 100; //No pasarse del valormáximo

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  int t1 = micros();
  delayMicroseconds(delay_micros);
  int t2 = micros();
  int res = (t2-t1) - 10;

  Serial. println(res);
  // put your main code here, to run repeatedly:
}
/*
Debería salir 0, pero cuando lo ccalculo con delay:
Algunas veces sale -1 

Para solucionarlo uso delayMicrosengudos:
La latencia es menos de lo que puede medir
No puede medir menos de 8 -> no hacer delays menos de 8, y hacer delay multiplos de 8
*/