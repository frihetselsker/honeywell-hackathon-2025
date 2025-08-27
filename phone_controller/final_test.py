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
# --------------------------------------------


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
    Accepts lines like:
      'Received: GAS:127,NOISE:556,TEMP:26.00,HUM:27.00'
    Returns dict with floats where possible:
      {'GAS':127.0,'NOISE':556.0,'TEMP':26.0,'HUM':27.0}
    """
    if line.startswith("Received:"):
        line = line[len("Received:"):].strip()
    data = {}
    for token in line.split(","):
        token = token.strip()
        if ":" in token:
            k, v = token.split(":", 1)
            k = k.strip().upper()
            try:
                data[k] = float(v.strip())
            except ValueError:
                data[k] = v.strip()
    return data


class AlertState:
    def __init__(self):
        self.ewma_temp = None
        self.temp_in_alert = False
        self.temp_above_count = 0
        self.last_alert_time = {"TEMP": 0.0, "GAS": 0.0, "NOISE": 0.0, "HUM": 0.0}

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

    return msgs


def main():
    print(f"[*] Starting. Chat ID: {CHAT_ID}  Port: {PORT}")
    send("🚀 Monitor started on laptop.")  # optional

    # Open serial (exclusive — close Arduino Serial Monitor)
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
        while True:
            raw = ser.readline()
            if not raw:
                continue
            line = raw.decode(errors="ignore").strip()
            if not line:
                continue

            print("Serial:", line)
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
