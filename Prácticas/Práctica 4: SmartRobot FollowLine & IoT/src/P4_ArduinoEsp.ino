// Programa a cargar en Arduino UNO
#include <Arduino_FreeRTOS.h>
#include <Thread.h>

Thread myThread = Thread();

// Motores
#define PIN_Motor_STBY 3
#define PIN_Motor_PWMA 5
#define PIN_Motor_PWMB 6
#define PIN_Motor_AIN_1 7
#define PIN_Motor_BIN_1 8

// Ultrasonidos
#define TRIG_PIN 13  
#define ECHO_PIN 12 

// Infrarrojo
#define PIN_ITR20001_LEFT   A2
#define PIN_ITR20001_MIDDLE A1
#define PIN_ITR20001_RIGHT  A0

// Umbral de la línea
#define LINEVALUE 100
#define LINE_MIDDLE_VALUE 60
#define LINE_FRECUENCY 5

// Tiempo entre cada mensaje de PING
#define PERIOD_PING 4000

// Velocidades
#define FORWARD 25
#define TURN 60
#define MINI_TURN 35
#define STOP 0

// Constantes sensor de distancia
int ULTRASONIC_FRECUENCY = 20;
long duracion;
int distancia;
int ultimaDistancia = 0;

// Ajustar la velocidad según los valores del infrarrojos
static void CheckLine(void* pvParameters) {

  TickType_t xLastWakeTime, aux;

  while(1){

    xLastWakeTime = xTaskGetTickCount();
    aux = xLastWakeTime;

    int leftValue = analogRead(PIN_ITR20001_LEFT);
    int middleValue = analogRead(PIN_ITR20001_MIDDLE);
    int rightValue = analogRead(PIN_ITR20001_RIGHT);    

    // Linea centrada
    if(middleValue > LINEVALUE) { 
      analogWrite(PIN_Motor_PWMA,FORWARD);
      analogWrite(PIN_Motor_PWMB,FORWARD);      
    }
    // Linea ligeramente a la derecha
    else if(rightValue > LINE_MIDDLE_VALUE && middleValue > LINE_MIDDLE_VALUE){
      analogWrite(PIN_Motor_PWMA,STOP);
      analogWrite(PIN_Motor_PWMB,MINI_TURN);           
    }
    // Línea a la derecha
    else if (rightValue > LINEVALUE) {
      analogWrite(PIN_Motor_PWMA,STOP);
      analogWrite(PIN_Motor_PWMB,TURN);
    }
    // Línea ligeramente a la izquierda
    else if(leftValue > LINE_MIDDLE_VALUE && middleValue > LINE_MIDDLE_VALUE){
      analogWrite(PIN_Motor_PWMA,MINI_TURN);
      analogWrite(PIN_Motor_PWMB,STOP);          
    }  
    // Linea a la izquierda
    else if (leftValue > LINEVALUE) {
      analogWrite(PIN_Motor_PWMA,TURN);
      analogWrite(PIN_Motor_PWMB,STOP);
    // Fuera de la linea
    } else {
      analogWrite(PIN_Motor_PWMA,STOP);
      analogWrite(PIN_Motor_PWMB,STOP);
      Serial.print("{LINE_LOST}");
    }
    xTaskDelayUntil(&xLastWakeTime,LINE_FRECUENCY);
    delay(200);
  }
}

long time_start;

// Pararse antes del obstáculo en el rango 5-8 cm
static void CheckDistance(void* pvParameters) {

  TickType_t xLastWakeTime, aux;
  
  while(1){

    xLastWakeTime = xTaskGetTickCount();
    aux = xLastWakeTime;

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    duracion = pulseIn(ECHO_PIN, HIGH);
    distancia = (duracion * 0.034) / 2;
    xTaskDelayUntil(&xLastWakeTime,ULTRASONIC_FRECUENCY);

    if(distancia < 50 && distancia > 8){
      ULTRASONIC_FRECUENCY = 5;  
      Serial.print("{OBSTACLE_DETECTED}");
    }

    else if(distancia <= 8 && distancia >= 5){
      digitalWrite(PIN_Motor_STBY,LOW);
      long time_end = millis() - time_start;
      Serial.print("{END_LAP}" + String(time_end) + "*");
    }

    else{
      ULTRASONIC_FRECUENCY = 20;
    }
    delay(200);
  }   
}

// Mensaje de PING (cada 4 segundos)
void callback_ping() {
  long time = millis();
  Serial.print("{PING}" + String(time) + "*"); 
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_ITR20001_LEFT, INPUT);
  pinMode(PIN_ITR20001_MIDDLE, INPUT);
  pinMode(PIN_ITR20001_RIGHT, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIN_Motor_STBY,OUTPUT);
  digitalWrite(PIN_Motor_STBY,HIGH);
  digitalWrite(PIN_Motor_AIN_1,HIGH);
  digitalWrite(PIN_Motor_BIN_1,HIGH);

  String sendBuff;

  Serial.print("{CONNECT}");

  while(1) {
    if (Serial.available()) {
      char c = Serial.read();
      sendBuff += c;
      if (c == '}')  {            
        if(sendBuff == "{CONNECTED}"){
          break;
        }
        sendBuff = "";
      } 
    }
  }

  Serial.print("{START_LAP}");
  time_start = millis();

  //Serial.print("{PING}" + String(millis() - time_start) + "*"); 

  xTaskCreate(CheckLine, "CheckLine", 100, NULL, 2, NULL);
  xTaskCreate(CheckDistance, "CheckDistance", 100, NULL, 1, NULL);

  myThread.enabled = true;
  myThread.setInterval(4000);  
  myThread.onRun(callback_ping);
}

void loop(){
  if(myThread.shouldRun()){
    myThread.run();
  } 
}
