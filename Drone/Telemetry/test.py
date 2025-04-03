from flask import Flask, Response, request, jsonify
from flask_socketio import SocketIO
import cv2
import sys
sys.path.append("/Users/khuselbayar/Desktop/InSPIRE_PT")
from Drone.Comp_vision.tiny_yolo import ObjectDetector

# Initialize Flask app and SocketIO
app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*")

# Global variables to store client commands
client_commands = {}

@socketio.on('connect')
def handle_connect():
    print(f"Client connected: {request.sid}")
    client_commands[request.sid] = {}

@socketio.on('disconnect')
def handle_disconnect():
    print(f"Client disconnected: {request.sid}")
    if request.sid in client_commands:
        del client_commands[request.sid]

@socketio.on('command')
def handle_command(data):
    print(f"Received command: {data}")
    client_commands[request.sid] = data
    # You could modify detector behavior based on commands
    # For example: detector.set_detection_threshold(data.get('threshold', 0.5))

@socketio.on('drop')
def handle_drop():
    print("Drop command received!")
    # Add any specific drop action here
    # For example: drone.drop_payload()
    # You can broadcast a confirmation message if needed
    socketio.emit('drop_status', {'status': 'Payload dropped successfully'})
    print('DROPPED!')

def generate_frames():
    detector = ObjectDetector()
    while True:
        if cv2.waitKey(1) == ord('q'):
            break
        frame = detector.get_frame_with_detections()
        if frame is not None:
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/')
def index():
    return """
    <html>
        <head>
            <title>Object Detection</title>
            <script src="https://cdnjs.cloudflare.com/ajax/libs/socket.io/4.0.1/socket.io.js"></script>
            <style>
                body {
                    font-family: Arial, sans-serif;
                    max-width: 800px;
                    margin: 0 auto;
                    padding: 20px;
                }
                .controls {
                    margin-top: 20px;
                    display: flex;
                    gap: 10px;
                }
                .drop-button {
                    background-color: #ff3b30;
                    color: white;
                    padding: 10px 20px;
                    border: none;
                    border-radius: 4px;
                    cursor: pointer;
                    font-weight: bold;
                    font-size: 16px;
                }
                .drop-button:hover {
                    background-color: #d63428;
                }
                .command-area {
                    display: flex;
                    gap: 10px;
                    margin-bottom: 20px;
                }
                input {
                    padding: 8px;
                    border: 1px solid #ccc;
                    border-radius: 4px;
                }
                button {
                    padding: 8px 15px;
                    background-color: #007aff;
                    color: white;
                    border: none;
                    border-radius: 4px;
                    cursor: pointer;
                }
                button:hover {
                    background-color: #0062cc;
                }
                #status-message {
                    margin-top: 10px;
                    padding: 10px;
                    border-radius: 4px;
                    display: none;
                }
                .success {
                    background-color: #d4edda;
                    color: #155724;
                    border: 1px solid #c3e6cb;
                }
            </style>
            <script type="text/javascript">
                var socket = io();
                
                function sendCommand() {
                    var command = document.getElementById('command').value;
                    socket.emit('command', {command: command});
                }
                
                function sendDropCommand() {
                    if (confirm('Are you sure you want to drop the payload?')) {
                        socket.emit('drop');
                        document.getElementById('status-message').textContent = 'Drop command sent!';
                        document.getElementById('status-message').style.display = 'block';
                        setTimeout(function() {
                            document.getElementById('status-message').style.display = 'none';
                        }, 3000);
                    }
                }
                
                // Listen for drop status from server
                socket.on('drop_status', function(data) {
                    document.getElementById('status-message').textContent = data.status;
                    document.getElementById('status-message').style.display = 'block';
                    setTimeout(function() {
                        document.getElementById('status-message').style.display = 'none';
                    }, 3000);
                });
            </script>
        </head>
        <body>
            <h1>Object Detection Feed</h1>
            <img src="/video_feed" width="640" height="480">
            
            <div class="controls">
                <div class="command-area">
                    <input type="text" id="command" placeholder="Enter command">
                    <button onclick="sendCommand()">Send</button>
                </div>
                
                <button class="drop-button" onclick="sendDropCommand()">DROP</button>
            </div>
            
            <div id="status-message" class="success"></div>
        </body>
    </html>
    """

if __name__ == "__main__":
    socketio.run(app, host="0.0.0.0", port=3000, debug=True)