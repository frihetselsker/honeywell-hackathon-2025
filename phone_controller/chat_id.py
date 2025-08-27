# get_chat_id.py
import requests, os

TOKEN = os.environ.get("TG_TOKEN") or "8257890117:AAHdOYwVhzeX0FpbqzBbZi8lGEZ_xd3KPa4"
# ia ultimele mesaje către bot (trebuie să-i fi scris botului deja)
upd = requests.get(f"https://api.telegram.org/bot{TOKEN}/getUpdates").json()
for r in upd.get("result", []):
    msg = r.get("message") or r.get("channel_post") or {}
    chat = msg.get("chat") or {}
    if "id" in chat:
        print("chat_id =", chat["id"])
