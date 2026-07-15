from flask import Flask, request, jsonify
import subprocess

app = Flask(__name__)


@app.route('/lookup', methods=['POST'])
def lookup():
    data = request.get_json()
    username = data['username']
    result = subprocess.run(["sherlock", username],
                            capture_output=True, text=True)
    raw_output = result.stdout
    return jsonify({
        "username": username,
        "raw_output": raw_output,
        "error_output": result.stderr
    })


if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000)
