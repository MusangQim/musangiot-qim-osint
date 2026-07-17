from flask import Flask, request, jsonify
import subprocess
import threading
import uuid

app = Flask(__name__)
jobs = {}


def run_sherlock(job_id, username):
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
    jobs[job_id]["status"] = "done"
    jobs[job_id]["sites_found"] = sites_found
    jobs[job_id]["count"] = len(sites_found)


@app.route('/lookup', methods=['POST'])
def lookup():
    data = request.get_json()
    username = data['username']
    job_id = str(uuid.uuid4())
    jobs[job_id] = {"status": "processing", "username": username}
    thread = threading.Thread(target=run_sherlock, args=(job_id, username))
    thread.start()
    return jsonify({
        "job_id": job_id,
        "status": "processing"
    })


@app.route('/result/<job_id>', methods=['GET'])
def get_result(job_id):
    if job_id not in jobs:
        return jsonify({"status": "not_found"}), 404
    job = jobs[job_id]
    return jsonify(job)


if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000)
