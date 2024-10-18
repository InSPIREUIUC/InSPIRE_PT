import cv2
from flask import Flask, render_template, Response

# Initialize Flask app
app = Flask(__name__)

# Initialize webcam, use 0 for default webcam
camera = cv2.VideoCapture(0)

# Function to generate the frames from the webcam
def generate_frames():
    while True:
        # Read the camera frame
        success, frame = camera.read()
        if not success:
            break
        else:
            # Encode the frame in JPEG format
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            
            # Yield the frame in byte format
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

# Route to serve the video feed
@app.route('/video_feed')
def video_feed():
    # Return the response generated by generate_frames()
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

# Root route for homepage
@app.route('/')
def index():
    return "Webcam feed is available at /video_feed"

if __name__ == "__main__":
    # Run the flask server
    app.run(host="0.0.0.0", port=5000)