const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');
const mqtt = require('mqtt');

const app = express();

// Configuración del puerto: Render asignará el puerto automáticamente en la variable de entorno PORT
const port = process.env.PORT || 3000;

// Configuración del broker MQTT: Usar variable de entorno o localhost por defecto
const mqttBroker = process.env.MQTT_BROKER || 'mqtt://localhost:1883';
const mqttClient = mqtt.connect(mqttBroker);

// Variables globales para almacenar el estado del LED y los datos de los sensores
let ledStatus = false; // LED apagado por defecto
let sensorData = {
    temperature: 0,
    humidity: 0,
    gasLevel: 0
};

// Conexión al broker MQTT
mqttClient.on('connect', () => {
    console.log(`Conectado al broker MQTT en ${mqttBroker}`);

    // Suscribirse al tópico para el estado del LED (opcional)
    mqttClient.subscribe('esp8266/ledStatus', (err) => {
        if (!err) {
            console.log('Suscrito al tópico esp8266/ledStatus');
        }
    });
});

// Escuchar mensajes entrantes del broker MQTT
mqttClient.on('message', (topic, message) => {
    console.log(`Mensaje recibido en el tópico ${topic}: ${message.toString()}`);

    // Actualizar el estado del LED si el mensaje proviene del tópico correspondiente
    if (topic === 'esp8266/ledStatus') {
        ledStatus = message.toString() === 'on';
        console.log(`Estado del LED actualizado desde MQTT: ${ledStatus ? 'Encendido' : 'Apagado'}`);
    }
});

// Configuración de body-parser para manejar los datos JSON
app.use(bodyParser.json());

// Servir archivos estáticos (HTML, CSS, JS)
app.use(express.static(path.join(__dirname, 'public')));

// Ruta para recibir los datos del sensor (POST)
app.post('/data', (req, res) => {
    const { temperature, humidity, gasLevel } = req.body;

    // Almacenar los datos del sensor
    sensorData.temperature = temperature;
    sensorData.humidity = humidity;
    sensorData.gasLevel = gasLevel;

    console.log('Datos recibidos:', sensorData);

    // Publicar los datos del sensor en un tópico MQTT
    mqttClient.publish('esp8266/sensorData', JSON.stringify(sensorData));

    res.status(200).send({ message: 'Datos recibidos correctamente' });
});

// Ruta para obtener el estado del LED (GET)
app.get('/led', (req, res) => {
    res.status(200).send(ledStatus ? 'true' : 'false');
});

// Ruta para cambiar el estado del LED (POST)
app.post('/led', (req, res) => {
    const { status } = req.body;
    if (status === 'toggle') {
        // Alternar el estado del LED
        ledStatus = !ledStatus;

        // Publicar el nuevo estado del LED en un tópico MQTT
        mqttClient.publish('esp8266/ledStatus', ledStatus ? 'on' : 'off');
    }

    console.log('Estado del LED actualizado:', ledStatus ? 'Encendido' : 'Apagado');
    res.status(200).json({ message: 'Estado del LED actualizado', ledStatus: ledStatus ? 'on' : 'off' });
});

// Ruta para obtener los datos del sensor (GET)
app.get('/data', (req, res) => {
    res.status(200).json(sensorData);
});

// Iniciar el servidor
app.listen(port, () => {
    console.log(`Servidor escuchando en el puerto ${port}`);
});
