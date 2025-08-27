# monitor_serial_and_notify.py
import requests, serial, time, os, sys, math

TOKEN   = os.environ.get("TG_TOKEN")   or "8257890117:AAHdOYwVhzeX0FpbqzBbZi8lGEZ_xd3KPa4"
CHAT_ID = os.environ.get("TG_CHAT_ID") or "7505346302"
PORT    = os.environ.get("SERIAL_PORT") or "COM6"
BAUD    = 9600

MIN_INTERVAL = 10  # seconds between alerts

# EWMA and hysteresis parameters for TEMP (adjust as needed)
DT = 1.0      # seconds between samples
TAU = 30.0    # smoothing time constant (seconds)
ALPHA = 1 - math.exp(-DT / TAU)

THIGH_TEMP = 40.0  # High threshold for TEMP alert
TLOW_TEMP = 38.0   # Low threshold for TEMP alert exit (hysteresis)
M_TEMP = 3         # Number of consecutive samples above threshold

def send(msg):
    try:
        resp = requests.post(
            f"https://api.telegram.org/bot{TOKEN}/sendMessage",
            json={"chat_id": CHAT_ID, "text": msg},
            timeout=8
        )
        if resp.status_code != 200:
            print(f"[TG] HTTP {resp.status_code}: {resp.text}")
        else:
            ok = resp.json().get("ok")
            if not ok:
                print(f"[TG] API error: {resp.text}")
        return resp
    except Exception as e:
        print(f"[TG] Exception: {e}")
        return None

def parse_metrics(line: str):
    """
    Accepts lines like:
    'Received: GAS:127,NOISE:556,TEMP:26.00,HUM:27.00'
    Returns dict: {'GAS':127.0,'NOISE':556.0,'TEMP':26.0,'HUM':27.0}
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
                # keep raw if not numeric
                data[k] = v.strip()
    return data


def should_alert(data: dict, last_sent_time: float):
    global ewma_state, ewma_alert_state, ewma_above_counter, last_alerts
    if 'ewma_state' not in globals():
        ewma_state = {'TEMP': None}
    if 'ewma_alert_state' not in globals():
        ewma_alert_state = {'TEMP': False}
    if 'ewma_above_counter' not in globals():
        ewma_above_counter = {'TEMP': 0}
    if 'last_alerts' not in globals():
        last_alerts = {'TEMP': 0}

    now = time.time()
    triggered_sensors = []

    # Only apply EWMA and hysteresis to TEMP for now
    if 'TEMP' in data and isinstance(data['TEMP'], (int, float)):
        x = data['TEMP']
        prev = ewma_state['TEMP']
        if prev is None:
            ewma = x
        else:
            ewma = ALPHA * x + (1 - ALPHA) * prev
        ewma_state['TEMP'] = ewma

        # Hysteresis logic
        if not ewma_alert_state['TEMP']:
            if ewma >= THIGH_TEMP:
                ewma_above_counter['TEMP'] += 1
                if ewma_above_counter['TEMP'] >= M_TEMP and now - last_alerts['TEMP'] >= MIN_INTERVAL:
                    triggered_sensors.append('TEMP')
                    ewma_alert_state['TEMP'] = True
                    last_alerts['TEMP'] = now
                    ewma_above_counter['TEMP'] = 0
            else:
                ewma_above_counter['TEMP'] = 0
        else:
            # In alert state, exit only if EWMA drops below TLOW
            if ewma <= TLOW_TEMP:
                ewma_alert_state['TEMP'] = False
                ewma_above_counter['TEMP'] = 0

    # ...existing code for other sensors (GAS, NOISE, HUM)...
    # You can implement similar EWMA/hysteresis logic for them if desired.

    return triggered_sensors, now if triggered_sensors else last_sent_time


def main():
    print("[*] Starting. Chat ID:", CHAT_ID, "Port:", PORT)
    ping = send("🚀 Monitor started on laptop.")
    if ping is None:
        print("[!] Couldn’t reach Telegram. Check internet/firewall/proxy.")
    else:
        print("[*] Telegram ping sent (see above for any API errors).")

    # Open serial (exclusive access—close Serial Monitor in Arduino IDE)
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
    except Exception as e:
        print(f"[!] Failed to open {PORT} @ {BAUD}: {e}")
        sys.exit(1)

    # Give Arduino time to reset on port open
    time.sleep(2.0)

    last_sent = 0
    first_payload_sent = False

    while True:
        try:
            raw = ser.readline()
            if not raw:
                continue
            line = raw.decode(errors="ignore").strip()
            if not line:
                continue

            print("Serial:", line)
            data = parse_metrics(line)

            if data:
                # Send the first parsed payload so you can verify end-to-end
                if not first_payload_sent:
                    send(f"📡 First data: {data}")
                    first_payload_sent = True

                triggered_sensors, last_sent = should_alert(data, last_sent)
                for sensor in triggered_sensors:
                    if sensor == 'TEMP':
                        msg = f"⚠️ TEMPERATURE ALERT (EWMA): {ewma_state['TEMP']:.2f}°C (Threshold: {THIGH_TEMP}°C)"
                    # ...existing code for other sensors...
                    send(msg)

            else:
                # optional keyword triggers from your original code
                if any(k in line for k in ("ALERT","INTRUDER","DANGER")):
                    now = time.time()
                    if now - last_sent > MIN_INTERVAL:
                        send(f"⚠️ Alert from laptop: {line}")
                        last_sent = now

        except KeyboardInterrupt:
            print("\n[!] Stopping…")
            break
        except Exception as e:
            print(f"[loop] Error: {e}")

    try:
        ser.close()
    except:
        pass

if __name__ == "__main__":
    main()
        try:
            raw = ser.readline()
            if not raw:
                continue
            line = raw.decode(errors="ignore").strip()
            if not line:
                continue

            print("Serial:", line)
            data = parse_metrics(line)

            if data:
                # Send the first parsed payload so you can verify end-to-end
                if not first_payload_sent:
                    send(f"📡 First data: {data}")
                    first_payload_sent = True

                triggered_sensors, last_sent = should_alert(data, last_sent)

                # Send separate alerts for each triggered sensor
                for sensor in triggered_sensors:
                    if sensor == 'GAS':
                        msg = f"⚠️ GAS ALERT: Level={data.get('GAS','?')} (Threshold: 100)"
                    elif sensor == 'NOISE':
                        msg = f"⚠️ NOISE ALERT: Level={data.get('NOISE','?')} (Threshold: 600)"
                    elif sensor == 'TEMP':
                        msg = f"⚠️ TEMPERATURE ALERT: {data.get('TEMP','?')}°C (Threshold: 40°C)"
                    elif sensor == 'HUM':
                        hum_value = data.get('HUM','?')
                        if hum_value != '?' and float(hum_value) < 20:
                            msg = f"⚠️ LOW HUMIDITY ALERT: {hum_value}% (Below threshold: 20%)"
                        else:
                            msg = f"⚠️ HIGH HUMIDITY ALERT: {hum_value}% (Above threshold: 80%)"

                    send(msg)

            else:
                # optional keyword triggers from your original code
                if any(k in line for k in ("ALERT","INTRUDER","DANGER")):
                    now = time.time()
                    if now - last_sent > MIN_INTERVAL:
                        send(f"⚠️ Alert from laptop: {line}")
                        last_sent = now

        except KeyboardInterrupt:
            print("\n[!] Stopping…")
            break
        except Exception as e:
            print(f"[loop] Error: {e}")

    try:
        ser.close()
    except:
        pass

if __name__ == "__main__":
    main()
