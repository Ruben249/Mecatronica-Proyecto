/* ------------ DECLARACIÓN DE VARIABLES Y PINES ------------ */

// Definición de pines para el sensor de ultrasonidos
#define TRIG_PIN 13  
#define ECHO_PIN 12  

// Definición de pines para sensores infrarrojos
#define PIN_ITR20001_LEFT   A2
#define PIN_ITR20001_MIDDLE A1
#define PIN_ITR20001_RIGHT  A0

// Definición de pines para control de motores
#define PIN_Motor_STBY 3
#define PIN_Motor_AIN_1 7
#define PIN_Motor_PWMA 5
#define PIN_Motor_BIN_1 8
#define PIN_Motor_PWMB 6

// Velocidad del coche
#define carSpeed 100

/* ------------ FUNCIONES DE MOVIMIENTO ------------ */ 

// Función para mover el coche hacia adelante
void forward(){
  analogWrite(PIN_Motor_PWMA , carSpeed);
  analogWrite(PIN_Motor_PWMB, carSpeed);
  digitalWrite(PIN_Motor_AIN_1, HIGH);
  digitalWrite(PIN_Motor_BIN_1, HIGH);
}

// Función para mover el coche hacia atrás
void back() {
  analogWrite(PIN_Motor_PWMA, carSpeed);
  analogWrite(PIN_Motor_PWMB, carSpeed);
  digitalWrite(PIN_Motor_AIN_1, LOW);
  digitalWrite(PIN_Motor_BIN_1, LOW);
}

// Función para detener el coche
void stop() {
  analogWrite(PIN_Motor_PWMA, 0);
  analogWrite(PIN_Motor_PWMB, 0);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Arduino inicializado");
  pinMode(ECHO_PIN, INPUT);    
  pinMode(TRIG_PIN, OUTPUT);  
  digitalWrite(TRIG_PIN, LOW);

  // Configuración de pines de motores y sensores
  pinMode(PIN_Motor_STBY, OUTPUT);
  pinMode(PIN_Motor_AIN_1, OUTPUT);
  pinMode(PIN_Motor_BIN_1, OUTPUT);
  pinMode(PIN_Motor_PWMA, OUTPUT);
  pinMode(PIN_Motor_PWMB, OUTPUT);
 
  pinMode(PIN_ITR20001_LEFT, INPUT);
  pinMode(PIN_ITR20001_RIGHT, INPUT);
  pinMode(PIN_ITR20001_MIDDLE, INPUT);
 
  digitalWrite(PIN_Motor_STBY, HIGH);
}


void loop() {
  // Leer mensajes desde la ESP32
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n'); // Leer hasta nueva línea
    message.trim(); // Eliminar caracteres invisibles como '\r' y '\n'
    
    Serial.print("Mensaje recibido: ");
    Serial.println(message);
    
    // Procesar mensaje
    if (message == "a") {
      Serial.println("Acción: Avanzar");
      forward();
    } else if (message == "b") {
      Serial.println("Acción: Retroceder");
      back();
    } else if (message == "c") {
      Serial.println("Acción: PARAR");
      stop();
    } else {
      Serial.println("Acción: Mensaje no reconocido");
    }
  }
}
