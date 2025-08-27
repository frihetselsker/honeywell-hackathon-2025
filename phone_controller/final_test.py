# monitor_serial_and_notify.py
import requests, serial, time, os

TOKEN   = os.environ.get("TG_TOKEN")   or "8257890117:AAHdOYwVhzeX0FpbqzBbZi8lGEZ_xd3KPa4"
CHAT_ID = os.environ.get("TG_CHAT_ID") or "7505346302"
PORT    = os.environ.get("SERIAL_PORT") or "COM6"   
BAUD    = 9600

def send(msg):
    requests.post(f"https://api.telegram.org/bot{TOKEN}/sendMessage",
                  json={"chat_id": CHAT_ID, "text": msg})

def main():
    ser = serial(PORT, BAUD, timeout=1)
    last_sent = 0
    min_interval = 10  # secunde între alerte (anti-spam)

    send("🚀 Monitor started on laptop.")

    while True:
        line = ser.readline().decode(errors="ignore").strip()
        if not line:
            continue

        # Exemplu: linia poate fi "GAS=512 NOISE=230 TEMP=24.5 HUM=60"
        # Poți adapta parserul cum vrei:
        print("Serial:", line)

        # Trigger simplu pe cuvinte-cheie:
        if "ALERT" in line or "INTRUDER" in line or "DANGER" in line:
            if time.time() - last_sent > min_interval:
                send(f"⚠️ Alert from laptop: {line}")
                last_sent = time.time()

        # Trigger pe prag numeric (ex: GAS>700)
        if "GAS:" in line:
            try:
                gas = int(line.split("GAS:")[1].split()[0])
                if gas > 200 and time.time() - last_sent > min_interval:
                    send(f"🔥 Gas high: {gas}")
                    last_sent = time.time()
            except:
                pass

if __name__ == "__main__":
    main()
