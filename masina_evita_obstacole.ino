//biblioteca pentru folosirea servomotorului
#include <Servo.h>
//biblioteca pentru shieldul de motoare
#include <AFMotor.h>
//piniul pe care e pus servomotorul
#define SERVO 9
//piniul pe care e pus buzzerul
#define BUZZER 10
//pinii pe care e pus senzorul ultrasonic
#define ECHO A4
#define TRIG A3
//pinii pe care sunt puse ledurile
#define RED A2
#define YELLOW A1
#define GREEN A0
//timpul necesar rotirii 90 de grade a robotului (in ms)
#define TURN 400
//distanta de la care se aprinde ledul rosu (in cm)
#define LIMIT1 30
//distanta de la care se aprinde ledul galben (in cm)
#define LIMIT2 50

//motorul din stanga e pus pe canalul 4
AF_DCMotor motor_left(4);
//motorul din dreapta e pus pe canalul 3
AF_DCMotor motor_right(3);
//servomotorul
Servo servo;

void setup() {
  // put your setup code here, to run once:
  //pornim comunicarea seriala (pentru debug)
  Serial.begin(9600);
  //oprim motoarele
  go(RELEASE, RELEASE);
  //atasam pinul la servo
  servo.attach(SERVO);
  //rotim servomotorul la mijloc (in fata)
  servo.write(90);
  //initializam pinii de la ultrasonic
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
  //initializam pinii de la leduri
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  //stingem ledurile
  digitalWrite(RED, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(GREEN, LOW);
  //initializam pinul de la buzzer
  pinMode(BUZZER, OUTPUT);
  //oprim buzzerul
  noTone(BUZZER);
  //asteptam 2s pentru a ne asigura ca initializarea e gata
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  //robotul merge in fata
  go(FORWARD, FORWARD);
  //masuram distanta
  int front = distance();
  //avem un obiect foarte aproape
  if (front < LIMIT1) {
    //pornim ledul rosu
    digitalWrite(RED, HIGH);
    digitalWrite(YELLOW, LOW);
    digitalWrite(GREEN, LOW);
    //pornim buzzerul
    tone(BUZZER, 250);
    //oprim robotul
    go(RELEASE, RELEASE);
    //asteptam o secunda
    delay(1000);
    //rotim servomotorul la dreapta
    servo.write(0);
    //asteptam o secunda
    delay(1000);
    //masuram distanta in dreapta
    int right = distance();
    //rotim servomotorul la stanga
    servo.write(180);
    //asteptam o secunda
    delay(1000);
    //masuram distanta la stanga
    int left = distance();
    //rotim servomotorul la mijloc (in fata)
    servo.write(90);
    //avem mai mult loc in stanga
    if (left > right) {
      //afisam in serial ca o luam la stanga (pentru debug)
      Serial.println("left");
      //pornim motorul drept
      go(RELEASE, FORWARD);
      //asteptam timpul setat sus in #define
      delay(TURN);
      //oprim robotul
      go(RELEASE, RELEASE);
    } else { //avem mai mult loc in dreapta
      //afisam in serial ca o luam la dreapta (pentru debug)
      Serial.println("right");
      //pornim motorul stang
      go(FORWARD, RELEASE);
      //asteptam timpul setat sus in #define
      delay(TURN);
      //oprim robotul
      go(RELEASE, RELEASE);
    }
    //oprim buzzerul
    noTone(BUZZER);
    //asteptam 500ms pentru ca robotul sa fie sigur oprit
    delay(500);
  } else if (front < LIMIT2) { //avem spatiu, dar nu mult
    //pornim ledul galben
    digitalWrite(RED, LOW);
    digitalWrite(YELLOW, HIGH);
    digitalWrite(GREEN, LOW);
  } else { //avem mult spatiu
    //pornim ledul verde
    digitalWrite(RED, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(GREEN, HIGH);
  }
  //astemptam putin intre operatii pentru a nu folosi prea des senzorul ultrasonic (scapam de ecou asa)
  //senzorul emite sunet, iar acesta se intoarce, dar nu in intregime, o parte se reflecta in alte directii si creeaza ecou
  //daca il folosim constant, acesta va receptiona si ecoul generat
  //asa ca asteptam cateva ms pentru a disparea ecoul
  delay(200);
}

//functie care misca robotul
void go(int a, int b) {
  //setam puterea motoarelor
  //motorul din stanga are o putere setata cu aprox 1% mai mica decat cel din dreapta
  //motorul din stanga trage putin mai bine ca cel din dreapta, asa ca pentru a combate asta, am setat o putere mai mica
  //daca puneam puterile egale, robotul o lua spre dreapta
  motor_left.setSpeed(253);
  motor_right.setSpeed(255);
  motor_left.run(a);
  motor_right.run(b);
}

//functie care citeste de 5 ori distanta apoi face media citirilor
//facem asa pentru a elimina eventualele citiri eronate
int distance() {
  float dist = 0;
  //citim de 5 ori distanta
  for (int i = 0; i < 5; i++) {
    //facem suma distantelor
    dist += one_shot();
    //asteptam 50ms intre citiri
    delay(50);
  }
  //returnam media
  return dist / 5.0;
}
//functie care citeste distanta de la ultrasonic
float one_shot() {
  //trimitem un impuls scurt pe pinul de TRIG
  digitalWrite(TRIG, LOW);
  delayMicroseconds(10);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  //masuram cat dureaza pana primim raspunsul pe pinul de ECHO
  int duration = pulseIn(ECHO, HIGH);
  //sunetul parcurge 0.034cm intr-o nanosecunda, distanta se imparte la 2 pentru ca sunetul a mers dus-intors de la senzor
  return (duration / 2) * 0.034;
}
