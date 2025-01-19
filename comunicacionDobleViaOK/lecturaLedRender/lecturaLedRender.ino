#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

// Configuración de red Wi-Fi
const char* ssid = "NIKO";           // Cambia por el nombre de tu red Wi-Fi
const char* password = "3113315176"; // Cambia por la contraseña de tu red Wi-Fi

// URL de la API desplegada en Render
const String apiBaseUrl = "https://api-node-esp82.onrender.com";

// Configuración del sensor DHT11
#define DHTPIN D4     // Pin al que está conectado el DHT11
#define DHTTYPE DHT11 // Tipo de sensor (DHT11)
DHT dht(DHTPIN, DHTTYPE); // Crear instancia del sensor

// Configuración del LED
#define LEDPIN D2 // Pin al que está conectado el LED

WiFiClientSecure client; // Instancia de cliente seguro

void setup() {
  Serial.begin(9600); // Inicializar comunicación serial

  // Conectar al Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");

  // Configurar cliente seguro
  client.setInsecure(); // Permite conexiones a servidores HTTPS sin verificar el certificado

  // Inicializar el sensor DHT y configurar el pin del LED
  dht.begin();
  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // **1. Obtener el estado del LED desde la API**
    http.begin(client, apiBaseUrl + "/led"); // Ruta de la API para el estado del LED
    int httpResponseCode = http.GET(); // Solicitud GET

    if (httpResponseCode > 0) {
      String payload = http.getString(); // Obtener el estado del LED como respuesta
      Serial.println("Estado del LED desde la API: " + payload);

      // Controlar el LED según el estado recibido ("true" o "false")
      if (payload == "true") {
        digitalWrite(LEDPIN, HIGH); // Encender el LED
        Serial.println("LED encendido");
      } else {
        digitalWrite(LEDPIN, LOW); // Apagar el LED
        Serial.println("LED apagado");
      }
    } else {
      Serial.println("Error al obtener el estado del LED: " + String(httpResponseCode));
    }
    http.end(); // Finalizar la conexión HTTP

    // **2. Leer datos del sensor DHT11**
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Verificar si las lecturas son válidas
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Error al leer del sensor DHT11");
      delay(2000); // Esperar antes de volver a intentar
      return;
    }

    // Mostrar los valores en el monitor serie
    Serial.println("Temperatura: " + String(temperature) + " °C");
    Serial.println("Humedad: " + String(humidity) + " %");

    // **3. Enviar datos del sensor a la API**
    http.begin(client, apiBaseUrl + "/data"); // Ruta de la API para enviar los datos
    http.addHeader("Content-Type", "application/json"); // Cabecera para enviar JSON

    // Crear el JSON con los datos del sensor
    String jsonPayload = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
    int postResponseCode = http.POST(jsonPayload); // Enviar los datos mediante POST

    // Manejar la respuesta del servidor
    if (postResponseCode > 0) {
      String response = http.getString(); // Leer la respuesta
      Serial.println("Respuesta de la API al enviar datos: " + response);
    } else {
      Serial.println("Error al enviar datos del sensor: " + String(postResponseCode));
    }
    http.end(); // Finalizar la conexión HTTP

    // **4. Esperar antes de realizar otra iteración**
    delay(5000);
  } else {
    Serial.println("Wi-Fi desconectado");
    delay(1000); // Esperar antes de volver a intentar
  }
}

