// Obtener los elementos del DOM
const temperatureElement = document.getElementById('temperature');
const humidityElement = document.getElementById('humidity');
const gasLevelElement = document.getElementById('gas-level');
const toggleLedButton = document.getElementById('toggle-led');

// Función para actualizar los datos del sensor
function updateSensorData() {
    fetch('/data')
        .then(response => response.json())
        .then(data => {
            temperatureElement.textContent = data.temperature;
            humidityElement.textContent = data.humidity;
            gasLevelElement.textContent = data.gasLevel;
        });
}

// Función para cambiar el estado del LED
toggleLedButton.addEventListener('click', () => {
    fetch('/led', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ status: 'toggle' }) // Cambiar de estado
    })
    .then(response => response.json())
    .then(data => {
        alert('Estado del LED actualizado');
        // Actualizar el texto del botón según el estado del LED
        toggleLedButton.textContent = data.ledStatus === 'on' ? 'Apagar LED' : 'Encender LED';
    })
    .catch(error => {
        console.error('Error al cambiar el estado del LED:', error);
    });
});


// Actualizar datos de los sensores cada 10 segundos
setInterval(updateSensorData, 10000);
