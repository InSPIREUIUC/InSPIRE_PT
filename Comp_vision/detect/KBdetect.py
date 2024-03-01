import cv2 as cv
#import argparse
import supervision as sv
import argparse
#import time
#from multiprocessing import Pool, Queue

from ultralytics import YOLO
#yolo.v8.detect.predict import DetectionPredictor

cap = cv.VideoCapture(0)    # Select webcam
#set gpu as device to run model

# To Do - 
# 1. Delete all the unused packages except for the used ones down below
# Use pip uninstall
# pip list : numpy 1.24.2, torch 2.0.0, torchvision 0.15.1, ultralytics 8.0.82, supervision 0.6.0


# Helper function to improve frame rate (In progress)

def parse_arguments() -> argparse.Namespace:   
    parser = argparse.ArgumentParser(description="Yolov8")  # Three command line arguments, one for resolution, one for scaling, one for skipping frames
    parser.add_argument("--resolution", type=str, default="640x360", help="input resolution for the detection model (wxh). lower resolutions to help speed.")
    parser.add_argument("--scale", type=float, default=1.0, help="maintains aspect ratio by scaling both width and height of the window.")
    parser.add_argument("--skip", type=int, default=1, help="number of frames to skip. increases video feed fps but image capture doesn't update as fast")
    return parser.parse_args()

def rescale(width, height):
    cap.set(3, width)
    cap.set(4, height)

# Main detect function
def detect(interval):
    model = YOLO("yolov8n.pt")  # Select model Yolov8 from Roboflow
    model.predict(source="0", stream=True, classes=0)    # Select class '0' which only predicts People
    # Formats the bounding boxes
    box_annotator = sv.BoxAnnotator(     
        color=sv.Color(0, 255, 0),                   
        thickness=2,    
        text_thickness=2,
        text_scale=1
    )
    count = 0
    while True:
        ret, frame = cap.read()     # get frame
        if (count % interval == 0):
            result = model(frame)[0]
            detections = sv.Detections.from_yolov8(result)
          # Create detections var from detections of Yolov8
        # Label and formats the bounding boxes
        labels = [
            f"{model.model.names[class_id]} Confidence: {confidence:0.2f}"
            for _, confidence, class_id, _
            in detections
        ]
        frame = box_annotator.annotate(
            scene = frame,
            detections = detections,
            labels = labels
        )
        frame = box_annotator.annotate(scene=frame, detections=detections)
        # Display results
        cv.imshow("yolov8", frame)

        if (cv.waitKey(30) == 27):
             break
        count += 1

#Parse arguments from command line
args = parse_arguments()
resolution = args.resolution.split('x')
scale = args.scale
skip_frames = args.skip
rescale(float(resolution[0]) * scale, float(resolution[1]) * scale)

#Run detection
detect(skip_frames)
