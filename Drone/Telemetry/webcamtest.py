from flask import Flask, Response
import cv2
from Comp_vision.detect.tiny_yolo import ObjectDetector

# Initialize Flask app
app = Flask(__name__)

# Initialize the object detector
detector = ObjectDetector()

# Function to generate frames with detections
def generate_frames():
    while True:
        frame = detector.get_frame_with_detections()
        if frame is not None:
            # Encode the frame in JPEG format
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()

            # Yield the frame in byte format
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

# Route to serve the video feed
@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

# Root route for homepage
@app.route('/')
def index():
    return "Detection feed is available at /video_feed"

if __name__ == "__main__":
    # Run the Flask server
    app.run(host="0.0.0.0", port=5000)