from flask import Flask, render_template, request, redirect
from flask_sqlalchemy import SQLAlchemy

app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///medicineBox.db'
db = SQLAlchemy(app)

scanning = 0
incharge = "@Lozerx"

class Info(db.Model):
	id = db.Column(db.Integer, primary_key=True)
	name = db.Column(db.String(300), nullable=False)
	card = db.Column(db.String(300), nullable=True)
	medicine = db.Column(db.String(300), nullable=False)
	time = db.Column(db.String(300), nullable=False)
	telegram = db.Column(db.String(300), nullable=False)

	def __repr__(self):
		return "<Info %r>" % self.id



@app.route('/')
def home():
	users = Info.query.all()
	return render_template("index.html", users=users)

@app.route("/scan/<int:id>", methods=["GET", "POST"])
def scan(id):
	global scanning
	user = Info.query.get_or_404(id)
	if request.method=="POST":
		user.card = request.form["data"]
		try:
			db.session.commit()
			scanning = "false"
			return "true"
		except:
			return "false"
	else:
		scanning = str(id)
		return render_template("scan.html", user=user)

@app.route("/delete/<int:id>")
def delete(id):
	user = Info.query.get_or_404(id)
	try:
		db.session.delete(user)
		db.session.commit()
		return redirect("/")
	except:
		return "There was an error in deleting!"

@app.route("/update/<int:id>", methods=["GET", "POST"])
def update(id):
	user = Info.query.get_or_404(id)
	if request.method=="POST":
		user.name = request.form["name"]
		user.time = request.form["time"]
		user.medicine = request.form["medicine"]
		user.telegram = request.form["telegram"]
		user.card = request.form["card"]
		try:
			db.session.commit()
			return redirect("/")
		except:
			return "There was an issue in updating!"
	else:
		return render_template("update.html", user=user)

@app.route("/add", methods=["GET", "POST"])
def add():
	global scanning
	if request.method=="POST":
		name = request.form["name"]
		time = request.form["time"]
		medicine = request.form["medicine"]
		telegram = request.form["telegram"]
		user = Info(name=name, time=time, medicine=medicine, telegram=telegram, card="none")
		try:
			db.session.add(user)
			db.session.commit()
			db.session.refresh(user)
			scanning = str(user.id)
			return redirect("/scan/"+str(user.id))
		except:
			return "There was an error in adding!"
	else:
		return render_template("/register.html")

@app.route("/card", methods=["GET"])
def card():
	global scanning
	id = int(request.args.get("id"))
	user = Info.query.get_or_404(id)
	user.card = request.args.get("card")
	try:
		db.session.commit()
		scanning = 0
		return user.card
	except:
		return "There was problem in scanning"


@app.route("/testing")
def testing():
	global scanning, incharge
	scanning = 0
	incharge = request.args.get("telegram")
	return redirect("/")

#ESP Helping Endpoints

@app.route("/read")
def read():
	return str(scanning)

@app.route("/incharge")
def incharge():
	return incharge

if __name__=='__main__':
	app.run(host='0.0.0.0', port=80)
