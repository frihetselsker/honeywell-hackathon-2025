# --- Telegram command polling ---
def get_updates(offset=None):
    url = f"https://api.telegram.org/bot{TOKEN}/getUpdates"
    params = {"timeout": 5, "allowed_updates": ["message"]}
    if offset:
        params["offset"] = offset
    try:
        resp = requests.get(url, params=params, timeout=8)
        if resp.status_code == 200:
            return resp.json().get("result", [])
    except Exception as e:
        print(f"[TG poll] {e}")
    return []

def handle_command(cmd, ser):
    cmd = cmd.lower()
    if cmd == "/data":
        # Send last known data
        msg = f"Last data: {latest_metrics}\nPower: {latest_power_usage_w} W" if latest_metrics else "No data yet."
        send(msg)
    elif cmd == "/buzzer":
        try:
            ser.write(b"TOGGLE_BUZZER\n")
            send("Buzzer toggle command sent.")
        except Exception as e:
            send(f"Failed to send: {e}")
    elif cmd == "/window":
        try:
            ser.write(b"TOGGLE_WINDOW\n")
            send("Window toggle command sent.")
        except Exception as e:
            send(f"Failed to send: {e}")
    elif cmd == "/cooler":
        try:
            ser.write(b"TOGGLE_COOLER\n")
            send("Cooler toggle command sent.")
        except Exception as e:
            send(f"Failed to send: {e}")
    else:
        send("Unknown command. Available: /data, /buzzer, /window, /cooler")

# monitor_serial_and_notify.py
import os, sys, time, math
import requests
import serial

# ------------------ Config ------------------
TOKEN   = os.environ.get("TG_TOKEN")   or "8257890117:AAHdOYwVhzeX0FpbqzBbZi8lGEZ_xd3KPa4"
CHAT_ID = os.environ.get("TG_CHAT_ID") or "7505346302"
PORT    = os.environ.get("SERIAL_PORT") or "COM6"
BAUD    = 9600

MIN_INTERVAL = 10  # seconds between alerts (per sensor)
SEND_FIRST_DATA = True  # set False if you don't want the first data echo

# EWMA + hysteresis for TEMP
DT = 1.0      # seconds between samples (approx)
TAU = 30.0    # smoothing time constant (seconds)
ALPHA = 1 - math.exp(-DT / TAU)

THIGH_TEMP = 40.0  # raise alert when EWMA >= THIGH_TEMP for M_TEMP samples
TLOW_TEMP  = 38.0  # clear alert when EWMA <= TLOW_TEMP
M_TEMP     = 3     # consecutive samples needed

# Simple thresholds for other sensors (tune as needed)
GAS_HIGH   = 200.0
NOISE_HIGH = 600.0
HUM_LOW    = 20.0
HUM_HIGH   = 80.0

# Global variable to store latest metrics
latest_metrics = {}
# Global variable to store latest power usage (in watts)
latest_power_usage_w = None


def send(msg: str):
    try:
        resp = requests.post(
            f"https://api.telegram.org/bot{TOKEN}/sendMessage",
            json={"chat_id": CHAT_ID, "text": msg},
            timeout=8
        )
        if resp.status_code != 200 or not resp.json().get("ok", False):
            print(f"[TG] Error {resp.status_code}: {resp.text}")
        return resp
    except Exception as e:
        print(f"[TG] Exception: {e}")
        return None


def parse_metrics(line: str):
    """
    Parses Arduino data lines like:
      'GAS:123,NOISE:45,TEMP:23.45,HUM:56.78,DIST:100,WINDOW:1,COOLER:0,BUZZER:1'
    Stores result in global variable latest_metrics and returns the dict.
    """
    global latest_metrics
    data = {}
    for token in line.split(","):
        token = token.strip()
        if ":" in token:
            k, v = token.split(":", 1)
            k = k.strip().upper()
            v = v.strip()
            # Try to convert to float or int if possible
            try:
                if "." in v:
                    data[k] = float(v)
                else:
                    data[k] = int(v)
            except ValueError:
                data[k] = v
    latest_metrics = data.copy()
    return data


class AlertState:
    def __init__(self):
        self.ewma_temp = None
        self.temp_in_alert = False
        self.temp_above_count = 0
        self.last_alert_time = {"TEMP": 0.0, "GAS": 0.0, "NOISE": 0.0, "HUM": 0.0}
        # For garage arrival detection
        self.home_arrived = False

    def can_send(self, sensor: str, now: float) -> bool:
        return now - self.last_alert_time.get(sensor, 0.0) >= MIN_INTERVAL

    def mark_sent(self, sensor: str, now: float):
        self.last_alert_time[sensor] = now


def evaluate_alerts(data: dict, st: AlertState):
    """
    Returns a list of alert message strings to send (may be empty).
    Updates state in-place.
    """
    msgs = []
    now = time.time()

    # --- TEMP (EWMA + hysteresis) ---
    if isinstance(data.get("TEMP"), (int, float)):
        x = float(data["TEMP"])
        st.ewma_temp = x if st.ewma_temp is None else (ALPHA * x + (1 - ALPHA) * st.ewma_temp)

        if not st.temp_in_alert:
            if st.ewma_temp >= THIGH_TEMP:
                st.temp_above_count += 1
                if st.temp_above_count >= M_TEMP and st.can_send("TEMP", now):
                    msgs.append(f"⚠️ TEMPERATURE ALERT (EWMA): {st.ewma_temp:.2f}°C (≥ {THIGH_TEMP}°C)")
                    st.temp_in_alert = True
                    st.mark_sent("TEMP", now)
                    st.temp_above_count = 0
            else:
                st.temp_above_count = 0
        else:
            # In alert, clear only when EWMA <= TLOW_TEMP
            if st.ewma_temp <= TLOW_TEMP:
                st.temp_in_alert = False
                st.temp_above_count = 0

    # --- GAS simple threshold ---
    if isinstance(data.get("GAS"), (int, float)):
        if data["GAS"] > GAS_HIGH and st.can_send("GAS", now):
            msgs.append(f"⚠️ GAS ALERT: {data['GAS']:.0f} (> {GAS_HIGH:.0f})")
            st.mark_sent("GAS", now)

    # --- NOISE simple threshold ---
    if isinstance(data.get("NOISE"), (int, float)):
        if data["NOISE"] > NOISE_HIGH and st.can_send("NOISE", now):
            msgs.append(f"⚠️ NOISE ALERT: {data['NOISE']:.0f} (> {NOISE_HIGH:.0f} BABY IS CRYING!!!!)")
            st.mark_sent("NOISE", now)

    # --- HUM simple band ---
    if isinstance(data.get("HUM"), (int, float)):
        hum = data["HUM"]
        if (hum < HUM_LOW or hum > HUM_HIGH) and st.can_send("HUM", now):
            kind = "LOW" if hum < HUM_LOW else "HIGH"
            limit = HUM_LOW if hum < HUM_LOW else HUM_HIGH
            msgs.append(f"⚠️ {kind} HUMIDITY ALERT: {hum:.0f}% (limit {limit:.0f}%)")
            st.mark_sent("HUM", now)

    # --- DIST garage arrival ---
    DIST_HOME_THRESHOLD = 30  # centimeters, adjust as needed
    if isinstance(data.get("DIST"), (int, float)):
        dist = data["DIST"]
        if dist <= DIST_HOME_THRESHOLD and not st.home_arrived:
            msgs.append("🏠 Home, sweet home! The car has arrived in the garage.")
            st.home_arrived = True
        elif dist > DIST_HOME_THRESHOLD and st.home_arrived:
            st.home_arrived = False

    return msgs


def main():
    print(f"[*] Starting. Chat ID: {CHAT_ID}  Port: {PORT}")
    send("🚀 Monitor started on laptop.")  # optional

    # Open serial
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
    except Exception as e:
        print(f"[!] Failed to open {PORT} @ {BAUD}: {e}")
        sys.exit(1)

    # Give Arduino time to reset after opening the port
    time.sleep(2.0)

    st = AlertState()
    first_payload_sent = False


    try:
        last_update_id = None
        while True:
            # --- Handle Telegram commands ---
            updates = get_updates(last_update_id)
            for upd in updates:
                last_update_id = upd["update_id"] + 1
                msg = upd.get("message", {})
                text = msg.get("text", "")
                if text.startswith("/"):
                    handle_command(text, ser)

            # --- Serial read as before ---
            raw = ser.readline()
            if not raw:
                continue
            line = raw.decode(errors="ignore").strip()
            if not line:
                continue

            print("Serial:", line)

            # Check for power usage string
            if line.startswith("Estimated power usage:"):
                try:
                    import re
                    m = re.search(r"Estimated power usage:\s*([\d.]+)", line)
                    if m:
                        global latest_power_usage_w
                        latest_power_usage_w = float(m.group(1))
                        print(f"Power usage: {latest_power_usage_w} W")
                        send(f"⚡ Power usage: {latest_power_usage_w} W")
                except Exception as e:
                    print(f"[Power parse error] {e}")
                continue

            data = parse_metrics(line)

            if data and SEND_FIRST_DATA and not first_payload_sent:
                send(f"📡 First data: {data}")
                first_payload_sent = True

            if data:
                msgs = evaluate_alerts(data, st)
                for m in msgs:
                    send(m)

            # Optional keyword fallback
            if not data and any(k in line for k in ("ALERT", "INTRUDER", "DANGER")):
                send(f"⚠️ Alert from laptop: {line}")

    except KeyboardInterrupt:
        print("\n[!] Stopping…")
    except Exception as e:
        print(f"[loop] Error: {e}")
    finally:
        try:
            ser.close()
        except:
            pass
        print("🔒 Port closed.")


if __name__ == "__main__":
    main()
