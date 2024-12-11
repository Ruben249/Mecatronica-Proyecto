void setup() {
  Serial.begin(9600);
  Serial.println("Arduino inicializado");
}

void loop() {
  // Leer mensajes desde la ESP32
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n'); // Leer hasta nueva línea
    Serial.print("Mensaje recibido: ");
    Serial.println(message);

    // Procesar mensaje
    if (message == "a") {
      Serial.println("Acción: Ejecutando acción para GESTO_A");
      // Agregar lógica para GESTO_A
    } else if (message == "b") {
      Serial.println("Acción: Ejecutando acción para GESTO_B");
      // Agregar lógica para GESTO_B
    }
  }
}
