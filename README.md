# QIM-OSINT 🔍

A handheld OSINT (Open Source Intelligence) lookup terminal — built with ESP32, powered by a Flask + Sherlock backend. No paid APIs, 100% free & open-source stack.

Search any username across hundreds of platforms, right from a pocket-sized device with a screen, touch controls, LED, and buzzer feedback.

```
┌─────────────┐   WiFi/HTTP    ┌──────────────────┐
│   ESP32     │ ─────POST────► │  Python Flask    │
│  + OLED     │                │  + Sherlock      │
│  + Touch x3 │ ◄────JSON───── │  + Threading     │
│  + LED/Buzz │                │  + Job Queue     │
└─────────────┘                └──────────────────┘
```

## ✨ Features

- 🖥️ **OLED display** — live status, results, and scrollable site list
- 👆 **3 capacitive touch sensors** — navigate menus (Up / Down / Select), no buttons needed
- 🔦 **LED feedback** — red blink while searching, green solid when done
- 🔊 **Buzzer feedback** — periodic beep while searching, victory jingle on completion
- 🔎 **Sherlock integration** — scan 400+ platforms for a username
- 🎯 **Two search modes**:
  - **RANDOM FIND** — full scan across all supported platforms
  - **SPECIFIC FIND** — target a single platform (e.g. `--site GitHub`)
- ⚡ **Async job-queue architecture** — no timeouts, even on slow multi-minute scans
- 💸 **Zero cost** — no paid APIs or subscriptions required

## 🛠️ Hardware

| Component | Purpose |
|---|---|
| ESP32 Dev Module | Main controller |
| 0.96" OLED SSD1306 (I2C) | Display |
| 3× TTP223 capacitive touch sensor | Up / Down / Select navigation |
| LED (red + green) | Status indicator |
| Passive buzzer | Audio feedback |

## 💻 Software Stack

- **Arduino IDE / ESP32 core** — firmware (WiFi, HTTP, OLED, FSM logic)
- **ArduinoJson** — JSON parsing on-device
- **Python + Flask** — backend server
- **Sherlock Project** — username OSINT engine
- **Threading + UUID** — async job queue for non-blocking long-running scans

## 🏗️ Architecture

The device uses a **Finite State Machine (FSM)**:

```
SELECTING → MODE_SELECT → SEARCHING → RESULT
   ↑                                      │
   └──────────────────────────────────────┘
        (Select to search again)
```

- **SELECTING** — pick a username (touch Up/Down to cycle, Select to confirm)
- **MODE_SELECT** — choose RANDOM FIND or SPECIFIC FIND (+ target site)
- **SEARCHING** — POST request to Flask, then polls for results every few seconds (no blocking, no timeouts)
- **RESULT** — displays site count + scrollable list of platforms found

### Why a job-queue instead of a direct request?

Sherlock can take 1–3 minutes to scan all platforms. A single blocking HTTP request would time out on the ESP32. Instead:

1. ESP32 `POST /lookup` → Flask replies **instantly** with a `job_id`, and starts Sherlock in a background thread
2. ESP32 `GET /result/<job_id>` periodically → gets `"processing"` until the scan is done, then `"done"` with results

This keeps the device responsive and avoids `-11` (read timeout) errors entirely.

## 🚀 Setup

**1. Flask server (on your laptop):**
```bash
git clone https://github.com/sherlock-project/sherlock.git
cd sherlock
python -m venv venv
source venv/bin/activate      # Windows: venv\Scripts\activate
pip install .
python app.py
```

**2. ESP32 firmware:**
- Install libraries: `Adafruit SSD1306`, `Adafruit GFX`, `ArduinoJson` (v6.x)
- Update `ssid`, `password`, and `serverURL` in the sketch to match your network + Flask server IP
- Upload via Arduino IDE

**3. Wiring:**

| ESP32 Pin | Component |
|---|---|
| GPIO 21 | OLED SDA |
| GPIO 22 | OLED SCL |
| GPIO 18 | Touch Up |
| GPIO 4 | Touch Down |
| GPIO 5 | Touch Select |
| GPIO 19 | LED Red |
| GPIO 23 | LED Green |
| GPIO 2 | Buzzer |

## 📸 Demo

*(add photos/screenshots/GIFs of the device here)*

## 🗺️ Roadmap

- [x] WiFi + HTTP POST basics
- [x] OLED display + JSON parsing
- [x] Flask server + Sherlock subprocess integration
- [x] Async job-queue polling (no timeouts)
- [x] Touch-based FSM navigation
- [x] LED + buzzer feedback
- [x] RANDOM FIND vs SPECIFIC FIND modes
- [ ] On-device keyboard (type any username via touch sensors)
- [ ] WiFiManager (no hardcoded WiFi credentials)
- [ ] mDNS auto-discovery (no manual IP configuration)

## ⚠️ Disclaimer

This tool is intended for **educational and personal OSINT research purposes only**. Always respect platform terms of service and applicable laws. Do not use this tool to harass, stalk, or harm others.

## 📄 License

MIT (or your preferred license)

---

Built as part of a self-directed embedded systems + Python learning journey (42 Malaysia Cadet Core Programme).
