from flask import Flask,jsonify

app = Flask(__name__)
@app.route('/')
def hello_world():
    return jsonify('Hello, World!\n')

welcome = "Welcome to 3ESE API!"

@app.route('/api/welcome/')
def api_welcome():
    return jsonify(welcome)

@app.route('/api/welcome/<int:index>')
def api_welcome_index(index):
    return jsonify({"index": index, "val": welcome[index]}), {"Content-Type": "application/json"}
