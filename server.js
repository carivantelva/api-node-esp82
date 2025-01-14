const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');
const app = express();
const port = 3000;

// Configuración de body-parser para manejar los datos JSON
app.use(bodyParser.json());

// Servir archivos estáticos (HTML, CSS, JS)
app.use(express.static(path.join(__dirname, 'public')));

// Variables globales para almacenar el estado del LED y los datos de los sensores
let ledStatus = false;  // LED apagado por defecto
let sensorData = {
    temperature: 0,
    humidity: 0,
    gasLevel: 0
};

// Ruta para recibir los datos del sensor (POST)
app.post('/data', (req, res) => {
    const { temperature, humidity, gasLevel } = req.body;

    // Almacenar los datos del sensor
    sensorData.temperature = temperature;
    sensorData.humidity = humidity;
    sensorData.gasLevel = gasLevel;

    console.log('Datos recibidos:', sensorData);

    // Responder al ESP8266 con un mensaje de éxito
    res.status(200).send({ message: 'Datos recibidos correctamente' });
});

// Ruta para obtener el estado del LED (GET)
app.get('/led', (req, res) => {
    // Enviar el estado del LED como respuesta
    res.status(200).send(ledStatus ? 'true' : 'false');
});

// Ruta para cambiar el estado del LED (POST)
app.post('/led', (req, res) => {
    const { status } = req.body;
    if (status === 'toggle') {
        // Alternar el estado del LED
        ledStatus = !ledStatus;
    }

    console.log('Estado del LED actualizado:', ledStatus ? 'Encendido' : 'Apagado');
    res.status(200).json({ message: 'Estado del LED actualizado', ledStatus: ledStatus ? 'on' : 'off' });
});

// Ruta para obtener los datos del sensor (GET)
app.get('/data', (req, res) => {
    // Enviar los datos de los sensores como respuesta
    res.status(200).json(sensorData);
});




// Iniciar el servidor
app.listen(port, () => {
    console.log(`Servidor escuchando en http://localhost:${port}`);
});
