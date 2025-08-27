# monitor_serial_and_notify.py
import requests, serial, time, os, sys

TOKEN   = os.environ.get("TG_TOKEN")   or "8257890117:AAHdOYwVhzeX0FpbqzBbZi8lGEZ_xd3KPa4"
CHAT_ID = os.environ.get("TG_CHAT_ID") or "7505346302"
PORT    = os.environ.get("SERIAL_PORT") or "COM6"
BAUD    = 9600

MIN_INTERVAL = 10  # seconds between alerts

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
    now = time.time()
    if now - last_sent_time < MIN_INTERVAL:
        return False, last_sent_time

    # If we didn't parse anything useful, skip
    if not data or all(v == "?" for v in data.values()):
        return False, last_sent_time

    # Triggers only on real numeric thresholds
    gas   = data.get("GAS")
    noise = data.get("NOISE")
    temp  = data.get("TEMP")
    hum   = data.get("HUM")

    try:
        if gas is not None and gas > 100:
            return True, now
        if noise is not None and noise > 600:
            return True, now
        if temp is not None and temp > 40:
            return True, now
        if hum is not None and (hum < 20 or hum > 80):
            return True, now
    except TypeError:
        # If any field wasn't numeric, just ignore
        return False, last_sent_time

    return False, last_sent_time


def main():
    # Basic connectivity check to Telegram (optional but helpful)
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

                do_alert, last_sent = should_alert(data, last_sent)
                if do_alert:
                    msg = (f"⚠️ Alert\n"
                           f"GAS={data.get('GAS','?')}  "
                           f"NOISE={data.get('NOISE','?')}  "
                           f"TEMP={data.get('TEMP','?')}°C  "
                           f"HUM={data.get('HUM','?')}%")
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
