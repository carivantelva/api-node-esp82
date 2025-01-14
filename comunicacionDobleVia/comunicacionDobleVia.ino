#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

// Configuración del DHT11
#define DHTPIN D4 // Pin del sensor DHT11 (D4 = GPIO 2)
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Configuración del sensor de gas
#define GAS_SENSOR_PIN A0 // Pin del sensor de gas (A0 = entrada analógica)

// Configuración Wi-Fi
const char* ssid = "NIKO";          // Reemplaza con tu SSID
const char* password = "3113315176";  // Reemplaza con tu contraseña

// Dirección del servidor API
const String serverUrl = "http://192.168.18.92:3000"; // Reemplaza con la IP de tu servidor

WiFiClient client; // Cliente WiFi

// Pin del LED
#define LEDPIN D2 // Cambia el pin si lo deseas

void setup() {
  Serial.begin(9600);
  dht.begin(); // Iniciar el sensor DHT11

  pinMode(LEDPIN, OUTPUT); // Configurar el pin del LED como salida

  // Conexión Wi-Fi
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado.");
  Serial.print("IP asignada: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Leer temperatura y humedad del DHT11
  float temperature = dht.readTemperature(); // Leer temperatura en grados Celsius
  float humidity = dht.readHumidity();       // Leer humedad relativa (%)

  // Leer nivel de gas del sensor
  int gasLevel = analogRead(GAS_SENSOR_PIN); // Leer valor analógico del sensor de gas

  // Validar lectura del DHT11
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Error al leer el sensor DHT11");
    return;
  }

  // Enviar datos al servidor
  sendDataToServer(temperature, humidity, gasLevel);

  // Leer el estado del LED desde el servidor
  getLedStatusFromServer();

  // Esperar 10 segundos antes de la próxima lectura
  delay(10000);
}

// Función para enviar datos al servidor (POST)
void sendDataToServer(float temperature, float humidity, int gasLevel) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(client, serverUrl + "/data"); // Ruta /data de la API

    // Crear el cuerpo del mensaje en formato JSON
    String jsonData = "{\"temperature\": " + String(temperature) +
                      ", \"humidity\": " + String(humidity) +
                      ", \"gasLevel\": " + String(gasLevel) + "}"; // Agregar gasLevel
    http.addHeader("Content-Type", "application/json"); // Encabezado JSON

    // Enviar la solicitud POST
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Respuesta del servidor: " + response);
    } else {
      Serial.println("Error al enviar datos. Código: " + String(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi no conectado. No se pueden enviar datos.");
  }
}

// Función para obtener el estado del LED desde el servidor (GET)
void getLedStatusFromServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(client, serverUrl + "/led"); // Ruta /led para obtener el estado del LED

    int httpResponseCode = http.GET();
    
    if (httpResponseCode == 200) {
      String response = http.getString();
      Serial.println("Respuesta del servidor sobre el LED: " + response);

      // Si la respuesta es "true", encendemos el LED
      if (response == "true") {
        digitalWrite(LEDPIN, HIGH); // Encender el LED
        Serial.println("LED encendido");
      } else {
        digitalWrite(LEDPIN, LOW); // Apagar el LED
        Serial.println("LED apagado");
      }
    } else {
      Serial.println("Error al obtener estado del LED. Código: " + String(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi no conectado. No se puede obtener el estado del LED.");
  }
}
