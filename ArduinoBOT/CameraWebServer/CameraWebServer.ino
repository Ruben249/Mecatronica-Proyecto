#include "esp_camera.h"
#include <WiFi.h>
#include <PubSubClient.h>  // Librería para MQTT

// ===================
// Select camera model
// ===================
#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
#include "camera_pins.h"

// ===========================
// Enter your WiFi credentials
// ===========================
const char *ssid = "Redmi10C";
const char *password = "bichodeguerra";

// Configuración MQTT
const char* mqtt_server = "test.mosquitto.org";  // Cambia esto por la IP o hostname de tu broker MQTT
const int mqtt_port = 1883;                    // Puerto estándar de MQTT
const char* mqtt_topic = "gestos/mano";        // El topic al que se va a suscribir el ESP32

WiFiClient espClient;
PubSubClient client(espClient);

// Variable para almacenar el mensaje recibido
String mqtt_message = "";

// Funciones
void startCameraServer();
void setupLedFlash(int pin);
void reconnectMQTT();
void callback(char* topic, byte* payload, unsigned int length);

void setup() {
  Serial.begin(115200);  // Comunicación serie con el puerto serial
  Serial.setDebugOutput(true);
  Serial.println();

  // Configuración de la cámara
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // Inicialización de la cámara
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);
    s->set_brightness(s, 1);
    s->set_saturation(s, -2);
  }
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

  // Conexión Wi-Fi
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Inicialización del servidor web de la cámara
  startCameraServer();
  
  // Configuración MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  // Reconectar a MQTT si se pierde la conexión
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();  // Mantener el cliente MQTT activo

  // Enviar el mensaje recibido a través de la conexión serie
  if (mqtt_message != "") {
    Serial.print("Mensaje MQTT recibido: ");
    Serial.println(mqtt_message);  // Mostrar el mensaje recibido en el puerto serie

    // Enviar el mensaje a Arduino por puerto serie
    Serial.println(mqtt_message);  // Lo que recibe el ESP32 se pasa al Arduino
    
    mqtt_message = "";  // Limpiar el mensaje después de enviarlo
  }

  delay(100);
}

// Función de reconexión MQTT
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Cam")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);  // Nos suscribimos al topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);  // Intentar de nuevo después de 5 segundos
    }
  }
}

// Función de callback para manejar los mensajes MQTT recibidos
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Mensaje recibido: ");
  Serial.println(message);

  // Almacenar el mensaje recibido en la variable mqtt_message
  mqtt_message = message;
}
