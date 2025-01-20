



// Navigation Logic
const tabs = document.querySelectorAll('.tab');
const contents = document.querySelectorAll('.main-content');

tabs.forEach((tab) => {
    tab.addEventListener('click', () => {
        // Remove active state from all tabs and contents
        tabs.forEach((t) => t.classList.remove('active'));
        contents.forEach((c) => c.classList.remove('active'));

        // Add active state to clicked tab and corresponding content
        tab.classList.add('active');
        const target = tab.getAttribute('data-target');
        document.getElementById(target).classList.add('active');
    });
});

 

// Firebase Configuration
const firebaseConfig = {
    apiKey: "AIzaSyAiF-WaEXEUcyDSUKSzvq_-INOAJLqQ3s0",
    authDomain: "smarthome-b2cca.firebaseapp.com",
    projectId: "smarthome-b2cca",
    storageBucket: "smarthome-b2cca.firebasestorage.app",
    messagingSenderId: "756026887503",
    appId: "1:756026887503:web:956b1d1f15a2272dbd893a",
    measurementId: "G-B776FJFRV4"
  };

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

// Fetch Latest Home Data
const homeRef = database.ref("home");
homeRef.limitToLast(1).on("child_added", (snapshot) => {
    const data = snapshot.val();
    if (data) {
        const temperature = data.temp;
        const humidity = data.humidity;

        // Update Temperature and Humidity
        document.getElementById("temp-value").textContent = `${temperature}°C`;
        document.getElementById("humidity-value").textContent = `${humidity}%`;

        // Update Graph
        tempHumidityGraph.data.datasets[0].data.shift();
        tempHumidityGraph.data.datasets[0].data.push(temperature);
        tempHumidityGraph.data.datasets[1].data.shift();
        tempHumidityGraph.data.datasets[1].data.push(humidity);
        tempHumidityGraph.update();
    }
});

const ctx = document.getElementById('tempHumidityGraph').getContext('2d');
const tempHumidityGraph = new Chart(ctx, {
    type: 'line',
    data: {
        labels: Array(8).fill(""),
        datasets: [
            {
                label: "Temperature (°C)",
                data: Array(8).fill(22),
                borderColor: "#ff6384",
                borderWidth: 2,
                fill: false,
            },
            {
                label: "Humidity (%)",
                data: Array(8).fill(50),
                borderColor: "#36a2eb",
                borderWidth: 2,
                fill: false,
            },
        ],
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            x: {
                ticks: { color: "#fff" },
                grid: { color: "rgba(255, 255, 255, 0.1)" },
            },
            y: {
                ticks: { color: "#fff" },
                grid: { color: "rgba(255, 255, 255, 0.1)" },
            },
        },
    },
});
console.log("Canvas Element:", document.getElementById("tempHumidityGraph"));
console.log("Chart Initialization:", tempHumidityGraph);


// Home Fan Logic
function updateHomeFan(temp) {
    const fanStatusText = document.getElementById("fan-status-text");
    const homeFan = document.getElementById("home-fan");

    if (temp >= 27) {
        homeFan.classList.add("active");
        fanStatusText.textContent = "Storeroom Ventilation ON";
    } else {
        homeFan.classList.remove("active");
        fanStatusText.textContent = "Storeroom Ventilation OFF";
    }
}

// Control Button Logic
const controlButton = document.getElementById("control-button");
let acHeaterEnabled = false;

controlButton.addEventListener("click", () => {
    acHeaterEnabled = !acHeaterEnabled;
    controlButton.textContent = acHeaterEnabled ? "Turn OFF AC/Heater" : "Turn ON AC/Heater";
    controlButton.classList.toggle("active", acHeaterEnabled);
});




// Garage Door Logic
const garageButton = document.getElementById("garage-btn");
const doorOpenIcon = document.getElementById("door-open-icon");
const doorClosedIcon = document.getElementById("door-closed-icon");

let isGarageOpen = false;

garageButton.addEventListener("click", () => {
    isGarageOpen = !isGarageOpen;

    if (isGarageOpen) {
        garageButton.textContent = "Close Garage Door";
        doorOpenIcon.classList.add("active");
        doorClosedIcon.classList.remove("active");
    } else {
        garageButton.textContent = "Open Garage Door";
        doorOpenIcon.classList.remove("active");
        doorClosedIcon.classList.add("active");
    }
});


// Update data every 2 seconds
setInterval(updateData, 2000);

// Bedroom Temperature and Fan Logic
const bedroomTempGraphCtx = document.getElementById('bedroomTempGraph').getContext('2d');
const bedroomTempGraph = new Chart(bedroomTempGraphCtx, {
    type: 'line',
    data: {
        labels: ['1s', '2s', '3s', '4s', '5s', '6s', '7s', '8s'],
        datasets: [
            {
                label: 'Temperature (°C)',
                data: Array(8).fill(18),
                borderColor: '#ff6384',
                borderWidth: 2,
                fill: false,
                tension: 0.4,
            },
        ],
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
            legend: {
                display: true,
                position: 'top',
                labels: {
                    color: '#fff',
                },
            },
        },
        scales: {
            x: {
                ticks: {
                    color: '#fff',
                },
                grid: {
                    color: 'rgba(255, 255, 255, 0.1)',
                },
            },
            y: {
                ticks: {
                    color: '#fff',
                },
                grid: {
                    color: 'rgba(255, 255, 255, 0.1)',
                },
            },
        },
    },
});

// Bedroom Fan and LED Logic
const fanStatus = document.getElementById('fan-status');
const bedroomPersonBtn = document.getElementById('bedroom-person-btn');
const ledBulb = document.getElementById('led-bulb');

function updateBedroomData() {
const temp = Math.floor(Math.random() * 10 + 16); // Random temp (16°C - 26°C)

// Update graph data
bedroomTempGraph.data.datasets[0].data.shift();
bedroomTempGraph.data.datasets[0].data.push(temp);
bedroomTempGraph.update();

// Update temperature text
document.getElementById('bedroom-temp-value').textContent = `${temp}°C`;

// Fan logic
const fanStatusText = fanStatus.querySelector("span");
if (temp > 20) {
fanStatus.classList.add('active');
fanStatusText.textContent = "Fan ON"; // Update text to "Fan ON"
} else {
fanStatus.classList.remove('active');
fanStatusText.textContent = "Fan OFF"; // Update text to "Fan OFF"
}
}

    const temp = Math.floor(Math.random() * 10 + 16); // Random temp (16°C - 26°C)

    // Update graph data
    bedroomTempGraph.data.datasets[0].data.shift();
    bedroomTempGraph.data.datasets[0].data.push(temp);
    bedroomTempGraph.update();

    // Update temperature text
    document.getElementById('bedroom-temp-value').textContent = `${temp}°C`;

    // Fan logic
    if (temp > 20) {
        fanStatus.classList.add('active');
    } else {
        fanStatus.classList.remove('active');
    }


// Person Button LED Logic
bedroomPersonBtn.addEventListener('click', () => {
    bedroomPersonBtn.classList.toggle('active');
    ledBulb.classList.toggle('active');
});

// Update data every 2 seconds
setInterval(updateBedroomData, 2000);


// Kitchen Oxygen and Chimney Logic
const kitchenOxygenGraphCtx = document.getElementById('kitchenOxygenGraph').getContext('2d');
const kitchenOxygenGraph = new Chart(kitchenOxygenGraphCtx, {
    type: 'line',
    data: {
        labels: ['1s', '2s', '3s', '4s', '5s', '6s', '7s', '8s'],
        datasets: [
            {
                label: 'Oxygen Level (%)',
                data: Array(8).fill(25),
                borderColor: '#36a2eb',
                borderWidth: 2,
                fill: false,
                tension: 0.4,
            },
        ],
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
            legend: {
                display: true,
                position: 'top',
                labels: {
                    color: '#fff',
                },
            },
        },
        scales: {
            x: {
                ticks: {
                    color: '#fff',
                },
                grid: {
                    color: 'rgba(255, 255, 255, 0.1)',
                },
            },
            y: {
                ticks: {
                    color: '#fff',
                },
                grid: {
                    color: 'rgba(255, 255, 255, 0.1)',
                },
            },
        },
    },
});

function updateKitchenData() {
    const oxygen = Math.floor(Math.random() * 15 + 15); // Random oxygen (15% - 30%)

    // Update graph data
    kitchenOxygenGraph.data.datasets[0].data.shift();
    kitchenOxygenGraph.data.datasets[0].data.push(oxygen);
    kitchenOxygenGraph.update();

    // Update oxygen text
    document.getElementById('kitchen-oxygen-value').textContent = `${oxygen}%`;

    // Chimney logic
    const chimneyStatus = document.getElementById('chimney-status');
    const chimneyStatusText = chimneyStatus.querySelector("span");
    if (oxygen < 20) {
        chimneyStatus.classList.remove('inactive');
        chimneyStatus.classList.add('active');
        chimneyStatusText.textContent = "Chimney ON";
    } else {
        
        chimneyStatus.classList.remove('active');
        chimneyStatus.classList.add('inactive');
        chimneyStatusText.textContent = "Chimney OFF";
    }
}

// Update Kitchen Data every 2 seconds
setInterval(updateKitchenData, 2000);
