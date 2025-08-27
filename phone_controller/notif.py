# send_telegram.py
import requests, os

TOKEN   = os.environ.get("TG_TOKEN")   or "8257890117:AAHdOYwVhzeX0FpbqzBbZi8lGEZ_xd3KPa4"
CHAT_ID = os.environ.get("TG_CHAT_ID") or "7505346302"

def send(msg):
    url = f"https://api.telegram.org/bot{TOKEN}/sendMessage"
    requests.post(url, json={"chat_id": CHAT_ID, "text": msg})

if __name__ == "__main__":
    send("rekhgjfj!")
