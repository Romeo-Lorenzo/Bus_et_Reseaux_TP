from flask import Flask, jsonify, abort, render_template, request

app = Flask(__name__)


welcome = list("Welcome to 3ESE API!")


@app.route("/")
def hello_world():
    return jsonify("Hello, World!\n")


@app.route("/api/welcome/", methods=["GET", "POST"])
def api_welcome():
    """
    GET  : renvoie la chaîne complète
    POST : ajoute des caractères à la fin (JSON: { "value": "..." }) -> 202 No Content
    """
    if request.method == "GET":
        return jsonify({
            "length": len(welcome),
            "value": "".join(welcome)
        })

    # POST
    data = request.get_json(silent=True) or {}
    if "value" not in data:
        abort(400)  # Bad Request

    for ch in str(data["value"]):
        welcome.append(ch)

    # Pas de contenu, seulement un code 202 comme demandé
    return ("", 202)


@app.route("/api/welcome/<int:index>", methods=["GET", "PUT", "DELETE"])
def api_welcome_index(index):
    """
    GET    : lire le caractère à l'index
    PUT    : modifier le caractère (JSON: { "value": "X" })
    DELETE : supprimer le caractère à cet index
    """
    # Vérifier que l'index est valide
    if index < 0 or index >= len(welcome):
        abort(404)

    if request.method == "GET":
        # Lecture
        return jsonify({
            "index": index,
            "value": welcome[index]
        })

    elif request.method == "PUT":
        # Mise à jour
        data = request.get_json(silent=True) or {}
        if "value" not in data:
            abort(400)

        new_val = str(data["value"])
        if len(new_val) == 0:
            abort(400)

        welcome[index] = new_val[0]

        return jsonify({
            "index": index,
            "value": welcome[index]
        })

    elif request.method == "DELETE":
        # Suppression
        del welcome[index]
        return ("", 204)  # 204 No Content


# ----------------------
# Route de debug request (GET/POST)
# ----------------------
@app.route("/api/request/", methods=["GET", "POST"])
@app.route("/api/request/<path:path>", methods=["GET", "POST"])
def api_request(path=None):
    resp = {
        "method":  request.method,
        "url":     request.url,
        "path":    path,
        "args":    request.args,              # paramètres dans l’URL ?a=1&b=2
        "headers": dict(request.headers),
    }

    if request.method == "POST":
        resp["POST"] = {
            "data": request.get_json(silent=True),  # JSON dans le corps de la requête
        }

    return jsonify(resp)


# ----------------------
# Handler 404 personnalisé
# ----------------------
@app.errorhandler(404)
def page_not_found(error):
    return render_template("page_not_found.html"), 404
