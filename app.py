from flask import Flask, request, jsonify

app = Flask(__name__)


@app.route('/lookup', methods=['POST'])
def lookup():
    data = request.get_json()
    username = data['username']
    return jsonify({
        "username": username,
        "status": "received",
        "message": "Server got your request!"
    })


if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000)
