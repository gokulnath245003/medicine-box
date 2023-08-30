from pytz import timezone
from datetime import datetime
from sqlalchemy import create_engine, text
import time
import requests

apiUrl = "http://api.callmebot.com/start.php"
engine = create_engine('sqlite:///medicineBox.db', echo = True)
engine.connect()

result = engine.execute(text("SELECT * FROM Info;"))
for row in result:
	print(row["id"])
	print(row["card"])

