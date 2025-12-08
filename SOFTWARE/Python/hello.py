from flask import Flask, jsonify, abort, render_template

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
    if index != len(welcome):
        abort(404)
    else:
        return jsonify({"index": index, "val": welcome[index]}), {"Content-Type": "application/json"}
    abort(404)

@app.errorhandler(404)
def page_not_found(error):
    return render_template('page_not_found.html'), 404
