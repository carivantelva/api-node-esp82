#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

// Configuración de red Wi-Fi
const char* ssid = "NIKO"; // Cambia por el nombre de tu red Wi-Fi
const char* password = "3113315176"; // Cambia por la contraseña de tu red Wi-Fi

// URL de la API desplegada en Render
const String apiBaseUrl = "https://api-node-esp82.onrender.com";

WiFiClientSecure client; // Instancia de WiFiClientSecure

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  // Esperar a que el ESP se conecte al Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");

  // Configurar cliente seguro
  client.setInsecure(); // Permite conexiones a servidores HTTPS sin verificar el certificado
}

void loop() {
  // Verificar conexión Wi-Fi antes de realizar solicitudes
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Pin del LED
    #define LEDPIN D2 // Cambia el pin si lo deseas
    pinMode(D2, OUTPUT);

    // Usar WiFiClientSecure junto con la URL en http.begin()
    http.begin(client, apiBaseUrl + "/led"); // Ruta completa para obtener el estado del LED
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("Estado del LED: " + payload);

      // Comparar el valor del payload con "true"
      if (payload == "true") {
        digitalWrite(LEDPIN, HIGH); // Encender el LED
        Serial.println("LED encendido");
      } else if (payload == "false") {
        digitalWrite(LEDPIN, LOW); // Apagar el LED
        Serial.println("LED apagado");
      }
    } else {
      Serial.println("Error al realizar GET: " + String(httpResponseCode));
    }

    http.end();

    delay(5000); // Esperar antes de realizar otra solicitud
  } else {
    Serial.println("WiFi desconectado");
  }
}
