let client = null;
let connected = false;
let ledState = false;

const brokerUrl = "wss://broker.masihch.com:443/mqtt";
const username = "freeplan";
const password = "12345678";

const statusText = document.getElementById("status");
const connectBtn = document.getElementById("connectBtn");
const toggleBtn = document.getElementById("toggleBtn");
const topicInput = document.getElementById("topicInput");

// Connect to broker
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
    console.log("MQTT Connected!");
  });

  client.on("error", (err) => {
    statusText.innerHTML = "Connection error ❌";
    console.error(err);
  });
};

// Toggle LED publish
toggleBtn.onclick = () => {
  if (!connected) {
    alert("Please connect to the broker first.");
    return;
  }

  ledState = !ledState;
  const message = ledState ? "on" : "off";
  const topic = topicInput.value || "home/led";

  client.publish(topic, message);
  toggleBtn.classList.toggle("active", ledState);

  statusText.innerHTML = `Message sent: ${message} to topic "${topic}"`;
  console.log(`Published: ${message} to ${topic}`);
};
