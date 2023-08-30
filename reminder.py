from pytz import timezone
from datetime import datetime
from sqlalchemy import create_engine, text
import time
import requests

apiUrl = "http://api.callmebot.com/start.php"
engine = create_engine('sqlite:///medicineBox.db', echo = True)
engine.connect()

while(True):
	result = engine.execute(text("SELECT * FROM Info;"))
	for row in result.fetchall():
		if str(row["time"])==str(datetime.now(timezone("Asia/Kolkata")).strftime("%H:%M")):
			payload = {"user":row["telegram"], "text":"Take medicines properly and its time to take the medicine "+row["medicine"], "cc": "missed"}
			requests.get(url=apiUrl, params=payload)
		else:
			print("No")
		print(datetime.now(timezone("Asia/Kolkata")).strftime("%H:%M"))
		print(row["time"])
	time.sleep(55)


