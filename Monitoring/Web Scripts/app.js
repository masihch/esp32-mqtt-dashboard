let client = null;
let connected = false;

const brokerUrl = "wss://broker.masihch.com:443/mqtt"; // SSL WebSocket
const username = "freeplan";
const password = "12345678";

const statusText = document.getElementById("status");
const connectBtn = document.getElementById("connectBtn");
const topicInput = document.getElementById("topicInput");
const tempValue = document.getElementById("tempValue");
const humValue = document.getElementById("humValue");

// --- Chart.js setup for Temperature ---
const tempCtx = document.getElementById("tempChart").getContext("2d");
const tempChart = new Chart(tempCtx, {
  type: "line",
  data: {
    labels: [],
    datasets: [{
      label: "Temperature (°C)",
      borderColor: "red",
      backgroundColor: "rgba(255,0,0,0.2)",
      data: [],
      fill: false
    }]
  },
  options: {
    responsive: true,
    plugins: { legend: { labels: { color: "#fff" } } },
    scales: {
      x: { ticks: { color: "#fff" } },
      y: { ticks: { color: "#fff" } }
    }
  }
});

// --- Chart.js setup for Humidity ---
const humCtx = document.getElementById("humChart").getContext("2d");
const humChart = new Chart(humCtx, {
  type: "line",
  data: {
    labels: [],
    datasets: [{
      label: "Humidity (%)",
      borderColor: "blue",
      backgroundColor: "rgba(0,0,255,0.2)",
      data: [],
      fill: false
    }]
  },
  options: {
    responsive: true,
    plugins: { legend: { labels: { color: "#fff" } } },
    scales: {
      x: { ticks: { color: "#fff" } },
      y: { ticks: { color: "#fff" } }
    }
  }
});

connectBtn.onclick = () => {
  if (connected) {
    statusText.innerHTML = "Already connected";
    return;
  }

  statusText.innerHTML = "Connecting...";

  client = mqtt.connect(brokerUrl, {
    username: username,
    password: password
  });

  client.on("connect", () => {
    connected = true;
    statusText.innerHTML = "Connected ✔";
    const topic = topicInput.value || "esp32/sensor";
    client.subscribe(topic);
    statusText.innerHTML = `Subscribed to ${topic}`;
  });

  client.on("message", (topic, message) => {
    try {
      const data = JSON.parse(message.toString());
      const now = new Date().toLocaleTimeString();

      if (data.temperature !== undefined) {
        tempValue.innerHTML = data.temperature + " °C";
        tempChart.data.labels.push(now);
        tempChart.data.datasets[0].data.push(data.temperature);
        if (tempChart.data.labels.length > 50) {
          tempChart.data.labels.shift();
          tempChart.data.datasets[0].data.shift();
        }
        tempChart.update();
      }

      if (data.humidity !== undefined) {
        humValue.innerHTML = data.humidity + " %";
        humChart.data.labels.push(now);
        humChart.data.datasets[0].data.push(data.humidity);
        if (humChart.data.labels.length > 50) {
          humChart.data.labels.shift();
          humChart.data.datasets[0].data.shift();
        }
        humChart.update();
      }
    } catch (e) {
      console.error("Invalid JSON:", message.toString());
    }
  });

  client.on("error", (err) => {
    statusText.innerHTML = "Connection error ❌";
    console.error(err);
  });
};
