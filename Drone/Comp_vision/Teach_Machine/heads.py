from __future__ import print_function
import cv2 as cv
import argparse
import supervision as sv
from roboflow import Roboflow
rf = Roboflow(api_key="")

project = rf.workspace().project("head_count-66hcq") 
model = project.version(3).model

annotator = sv.BoxAnnotator()
       
def on_prediction(predictions, frame):
    labels = [p["class"] for p in predictions["predictions"]]
    detections = sv.Detections.from_roboflow(predictions)
    cv.imshow(
        "Prediction", 
        annotator.annotate(
            scene=frame, 
            detections=detections,
            labels=labels
        )
    )

parser = argparse.ArgumentParser(description='Code for Cascade Classifier tutorial.')
parser.add_argument('--camera', help='Camera divide number.', type=int, default=0)
args = parser.parse_args()
camera_device = args.camera
cap = cv.VideoCapture(camera_device)
if not cap.isOpened:
    print('--(!)Error opening video capture')
    exit(0)

while True:
    ret, frame = cap.read()
    if frame is None:
        print('--(!) No captured frame -- Break!')
        break

    predictions = model.predict(frame, confidence=50, overlap=30).json()
    print(predictions)
    on_prediction(predictions, frame)

    if cv.waitKey(10) == 27:
        break