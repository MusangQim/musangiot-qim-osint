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
    lines = raw_output.split("\n")
    sites_found = []
    for line in lines:
        if line.startswith("[+]"):
            parts = line.split(":")
            socmed_name = parts[0]
            clean_name = socmed_name[4:]
            sites_found.append(clean_name)
    return jsonify({
        "username": username,
        "raw_output": raw_output,
        "error_output": result.stderr,
        "sites_found": sites_found,
        "count": len(sites_found)
    })


if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000)
