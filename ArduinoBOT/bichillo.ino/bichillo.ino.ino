void setup() {
  // Inicializamos la comunicación serial
  Serial.begin(9600);
}

void loop() {
  // Verificamos si hay datos disponibles para leer desde el puerto serie
  if (Serial.available()){
    // Leemos el valor recibido (0 o 1) y lo imprimimos
    String received = Serial.readString();
    Serial.print("Valor recibido: ");
    Serial.println(received);
  }
}
