import sqlite3
import json
import threading
from flask import Flask, render_template_string
import paho.mqtt.client as mqtt
from gpiozero import LED

# --- Hardware Configuration (using BCM pin numbers) ---
# gpiozero automatically handles pin setup and cleanup
red_led = LED(17)
amber_led = LED(27)
green_led = LED(22)

# --- SQLite Setup ---
DB_NAME = "telemetry.db"


def init_db():
    conn = sqlite3.connect(DB_NAME)
    cursor = conn.cursor()
    cursor.execute("""         
        CREATE TABLE IF NOT EXISTS logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            temperature REAL,
            humidity REAL,
            motion INTEGER,
            light REAL,
            status TEXT
        )
    """)
    conn.commit()
    conn.close()


# --- Shared Cache (Thread-Safe) ---
latest_data = {
    "temperature": 0.0,
    "humidity": 0.0,
    "motion": 0,
    "light": 1024,
    "status": "INIT",
}
data_lock = threading.Lock()


# --- MQTT Business & Context-Aware Logic ---
def on_message(client, userdata, msg):
    global latest_data
    payload = json.loads(msg.payload.decode("utf-8"))

    with data_lock:
        latest_data.update(payload)

    # Store in Database
    conn = sqlite3.connect(DB_NAME)
    cursor = conn.cursor()
    cursor.execute(
        """
        INSERT INTO logs (temperature, humidity, motion, light, status)
        VALUES (?, ?, ?, ?, ?)
    """,
        (
            payload["temperature"],
            payload["humidity"],
            payload["motion"],
            payload["light"],
            payload["status"],
        ),
    )
    conn.commit()
    conn.close()

    # Retrieve values for logic parsing
    motion = payload.get("motion", 0)
    light = payload.get("light", 1024)
    status = payload.get("status", "OK")

    # Context-Aware Security LED Actions using gpiozero
    if status == "SENSOR_ERROR":
        # System fault: Amber LED active only
        green_led.off()
        amber_led.on()
        red_led.off()

    elif motion == 1 and light > 600:
        # High Threat: Motion in complete darkness triggers high alarm (Red)
        green_led.off()
        amber_led.off()
        red_led.on()

    elif motion == 1 and light <= 600:
        # Minor event: Motion during daytime (Green and Amber both active)
        green_led.on()
        amber_led.on()
        red_led.off()

    else:
        # Normal State: System is secure (Green active only)
        green_led.on()
        amber_led.off()
        red_led.off()


def run_mqtt():
    client = mqtt.Client()
    client.on_message = on_message
    client.connect("localhost", 1883, 60)
    client.subscribe("home/sensors")
    client.loop_forever()


# --- Flask Server Thread ---
app = Flask(__name__)

INDEX_HTML = """
<!DOCTYPE html>
<html>
<head>
    <title>Smart Logger Dashboard</title>
    <meta http-equiv="refresh" content="3">
    <style>
        body { font-family: monospace; background: #121212; color: #00ff00; padding: 40px; }
        .card { border: 1px solid #00ff00; padding: 20px; display: inline-block; }
    </style>
</head>
<body>
    <h2>Telemetry Dashboard</h2>
    <div class="card">
        <p>Status: {{ data.status }}</p>
        <p>Temp: {{ data.temperature }} °C</p>
        <p>Humidity: {{ data.humidity }} %</p>
        <p>Motion Detected: {{ 'YES' if data.motion == 1 else 'NO' }}</p>
        <p>Light Level: {{ data.light }}</p>
    </div>
</body>
</html>
"""


@app.route("/")
def home():
    with data_lock:
        data_copy = dict(latest_data)
    return render_template_string(INDEX_HTML, data=data_copy)


# --- Initialization Execution ---
if __name__ == "__main__":
    init_db()

    # Spin up MQTT Background Thread
    mqtt_thread = threading.Thread(target=run_mqtt, daemon=True)
    mqtt_thread.start()

    # Run Flask App on Main Thread (gpiozero handles its own OS-level signal cleanup)
    app.run(host="0.0.0.0", port=5000, debug=False, use_reloader=False)
