const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');
const mqtt = require('mqtt'); // Importar MQTT

const app = express();
const port = 3000;

// Configuración de body-parser para manejar los datos JSON
app.use(bodyParser.json());

// Servir archivos estáticos (HTML, CSS, JS)
app.use(express.static(path.join(__dirname, 'public')));

// Conectar al broker Mosquitto (puedes cambiar localhost por la IP del broker)
const mqttClient = mqtt.connect('mqtt://localhost:1883');

// Variables globales para el estado del LED y los datos de los sensores
let ledStatus = false; // LED apagado por defecto
let sensorData = {
    temperature: 0,
    humidity: 0,
    gasLevel: 0
};

// Conexión al broker MQTT
mqttClient.on('connect', () => {
    console.log('Conectado al broker MQTT');

    // Suscribirse a un tópico (opcional)
    mqttClient.subscribe('esp8266/ledStatus', (err) => {
        if (!err) {
            console.log('Suscrito al tópico esp8266/ledStatus');
        }
    });
});

// Escuchar mensajes MQTT
mqttClient.on('message', (topic, message) => {
    console.log(`Mensaje recibido en el tópico ${topic}: ${message.toString()}`);

    // Actualizar el estado del LED si llega un mensaje
    if (topic === 'esp8266/ledStatus') {
        ledStatus = message.toString() === 'on';
        console.log(`Estado del LED actualizado desde MQTT: ${ledStatus ? 'Encendido' : 'Apagado'}`);
    }
});

// Ruta para recibir los datos del sensor (POST)
app.post('/data', (req, res) => {
    const { temperature, humidity, gasLevel } = req.body;

    // Almacenar los datos del sensor
    sensorData.temperature = temperature;
    sensorData.humidity = humidity;
    sensorData.gasLevel = gasLevel;

    console.log('Datos recibidos:', sensorData);

    // Publicar los datos en un tópico MQTT
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
    console.log(`Servidor escuchando en http://localhost:${port}`);
});
