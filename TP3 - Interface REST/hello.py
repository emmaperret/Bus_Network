from flask import Flask, abort, render_template, jsonify, request
import json
app = Flask(__name__)

welcome = "Welcome to 3ESE API!"

@app.route('/')
def hello_world():
    return 'Hello, World!\n'

@app.route('/api/welcome/', methods=['GET', 'POST', 'PUT', 'PATCH', 'DELETE'])
def api_welcome():
    welcome = "Welcome to 3ESE API!"

    if request.method == 'POST':
        data = request.get_json(force=True)
        welcome = data["sentence"]

    if request.method == 'GET':
        return json.dumps({"sentence": welcome}), {"Content-Type": "application/json"}

    return welcome

@app.route('/api/welcome/<int:index>', methods=['GET','POST', 'PUT', 'PATCH', 'DELETE'])
def api_welcome_index(index):
    if(index>19):
        abort(404)
    else :
        if request.method == 'GET':
            return json.dumps({"index": index, "val": welcome[index]}), {"Content-Type": "application/json"}
        if request.method == 'PUT':
            data = request.get_json(force=True)
            welcome[index] = data["word"]
            return welcome
        if request.method == 'PATCH':
            data = request.get_json(force=True)
            welcome[index] = data["letter"]
            return welcome

    return json.dumps({"index": index, "val": welcome[index]}), {"Content-Type": "application/json"}

@app.errorhandler(404)
def page_not_found(error):
    return render_template('page_not_found.html'), 404

@app.route('/api/request/', methods=['GET', 'POST'])
@app.route('/api/request/<path>', methods=['GET','POST'])
def api_request(path=None):
    resp = {
            "method":   request.method,
            "url" :  request.url,
            "path" : path,
            "args": request.args,
            "headers": dict(request.headers),
    }
    if request.method == 'POST':
        resp["POST"] = {
                "data" : request.get_json(force=True),
                }
    return jsonify(resp)