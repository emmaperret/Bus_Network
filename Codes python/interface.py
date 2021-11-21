from flask import Flask, abort, render_template, jsonify, request
import json
app = Flask(__name__)

temp = {
	'temp1' : val1,
	'temp2' : val1,
	'temp3' : val1,
	'temp4' : val1,
}

pres = {
	'pres1' : val1,
	'pres2' : val1,
	'pres3' : val1,
	'pres4' : val1,
}

@app.route('/')
def welcome():
	return 'Welcome to our API'

@app.route('/api/temp/', methods=['GET', 'POST'])
def api_temp():
	#pb = comment numéroter nouvelle valeur ajoutée
	
	if request.method == 'GET':
		#renvoie toutes les valeurs de température
		return json.dumps({"temp1": temp["temp1"], "temp2": temp["temp2"]}), {"Content-Type": "application/json"}

	if request.method == 'POST':
		#récupère une nouvelle valeur de température
		temp = {
			'temp' : request.get_json(force=True),
		}
	return jsonify(temp)


@app.route('/api/temp/<int:index>', methods=['GET', 'DELETE'])
def api_temp_index(index):
	if request.method == 'GET':
		#renvoie la valeur de température numéro index
		return json.dumps({"temp" index : temp[index]}), {"Content-Type": "application/json"}

	if request.method == 'DELETE':
		#supprime la valeur de température numéro index
		del temp[index]
	return jsonify(temp)

@app.route('/api/pres/', methods=['GET', 'POST'])
def api_pres():
	#pb = comment numéroter nouvelle valeur ajoutée
	i=1;
	if request.method == 'GET':
		#renvoie toutes les valeurs de température
		return json.dumps({"pres1": pres["pres1"], "pres2": pres["pres2"]}), {"Content-Type": "application/json"}

	if request.method == 'POST':
		#récupère une nouvelle valeur de température
		pres = {
			'pres' i : request.get_json(force=True),
		}
		i++;
	return jsonify(pres)

@app.route('/api/pres/<int:index>', methods=['GET', 'DELETE'])
def api_pres_index(index):
	if request.method == 'GET':
		#renvoie la valeur de température numéro index
		return json.dumps({"pres" index : pres[index]}), {"Content-Type": "application/json"}

	if request.method == 'DELETE':
		#supprime la valeur de température numéro index
		del pres[index]
	return jsonify(pres)