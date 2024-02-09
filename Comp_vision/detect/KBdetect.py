import cv2 as cv
import argparse

from ultralytics import YOLO
import supervision as sv
# To Do - 
# 1. Delete all the unused packages except for the used ones down below
# Use pip uninstall
# pip list : numpy 1.24.2, torch 2.0.0, torchvision 0.15.1, ultralytics 8.0.82, supervision 0.6.0


# Helper function to improve frame rate (In progress)
def parse_arguments() -> argparse.Namespace:   
    parser = argparse.ArgumentParser(description="Yolov8")  # Use argparser to improve frame rate somehow
    parser.add_argument("--resolution", type=str, default="640x360", help="input resolution for the detection model (wxh). lower resolutions to help speed.")
    parser.add_argument("--skip-frames", type=int, default=1, help="Process every nth frame to improve speed. 1 = process every frame.")

    return parser.parse_args()

# Main detect function
def detect():
    cap = cv.VideoCapture(0)    # Select webcam
    model = YOLO("yolov8l.pt")  # Select model Yolov8 from Roboflow
    model.predict(source="0", show=True, stream=True, classes=0)    # Select class '0' which only predicts People
    # Formats the bounding boxes
    box_annotator = sv.BoxAnnotator(                        
        thickness=2,    
        text_thickness=2,
        text_scale=1
    )
    while True:
        ret, frame = cap.read()     # get frame
        result = model(frame)[0]    # get results from first argument of model(frame) which is a tuple
        detections = sv.Detections.from_yolov8(result)  # Create detections var from detections of Yolov8
        # Labels the bounding boxes
        labels = [
            f"{model.model.names[class_id]} {confidence:0.2f}"
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

detect()