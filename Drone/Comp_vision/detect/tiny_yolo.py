import depthai as dai
import numpy as np
from pathlib import Path
import cv2

class ObjectDetector:
    def __init__(self):
        # Define the path to the YOLOv4 model blob file
        self.nnPath = str((Path(__file__).parent / Path('../models/yolo-v4-tiny-tf_openvino_2021.4_6shave.blob')).resolve().absolute())

        # YOLOv4 label texts
        self.labelMap = [
            "person",         "bicycle",    "car",           "motorbike",     "aeroplane",   "bus",           "train",
            "truck",          "boat",       "traffic light", "fire hydrant",  "stop sign",   "parking meter", "bench",
            "bird",           "cat",        "dog",           "horse",         "sheep",       "cow",           "elephant",
            "bear",           "zebra",      "giraffe",       "backpack",      "umbrella",    "handbag",       "tie",
            "suitcase",       "frisbee",    "skis",          "snowboard",     "sports ball", "kite",          "baseball bat",
            "baseball glove", "skateboard", "surfboard",     "tennis racket", "bottle",      "wine glass",    "cup",
            "fork",           "knife",      "spoon",         "bowl",          "banana",      "apple",         "sandwich",
            "orange",         "broccoli",   "carrot",        "hot dog",       "pizza",       "donut",         "cake",
            "chair",          "sofa",       "pottedplant",   "bed",           "diningtable", "toilet",        "tvmonitor",
            "laptop",         "mouse",      "remote",        "keyboard",      "cell phone",  "microwave",     "oven",
            "toaster",        "sink",       "refrigerator",  "book",          "clock",       "vase",          "scissors",
            "teddy bear",     "hair drier", "toothbrush"
        ]

        self.syncNN = True

        # Create pipeline
        self.pipeline = dai.Pipeline()

        # Define sources and outputs
        self.camRgb = self.pipeline.create(dai.node.ColorCamera)
        self.detectionNetwork = self.pipeline.create(dai.node.YoloDetectionNetwork)
        self.xoutRgb = self.pipeline.create(dai.node.XLinkOut)
        self.nnOut = self.pipeline.create(dai.node.XLinkOut)

        self.xoutRgb.setStreamName("rgb")
        self.nnOut.setStreamName("nn")

        # Properties
        self.camRgb.setPreviewSize(416, 416)
        self.camRgb.setResolution(dai.ColorCameraProperties.SensorResolution.THE_1080_P)
        self.camRgb.setInterleaved(False)
        self.camRgb.setColorOrder(dai.ColorCameraProperties.ColorOrder.BGR)
        self.camRgb.setFps(40)

        # Network specific settings
        self.detectionNetwork.setConfidenceThreshold(0.5)
        self.detectionNetwork.setNumClasses(80)
        self.detectionNetwork.setCoordinateSize(4)
        self.detectionNetwork.setAnchors([10, 14, 23, 27, 37, 58, 81, 82, 135, 169, 344, 319])
        self.detectionNetwork.setAnchorMasks({"side26": [1, 2, 3], "side13": [3, 4, 5]})
        self.detectionNetwork.setIouThreshold(0.5)
        self.detectionNetwork.setBlobPath(self.nnPath)
        self.detectionNetwork.setNumInferenceThreads(2)
        self.detectionNetwork.input.setBlocking(False)

        self.frame = None
        self.detections = []
        self.color2 = (255, 255, 255)

        # Linking
        self.camRgb.preview.link(self.detectionNetwork.input)
        if self.syncNN:
            self.detectionNetwork.passthrough.link(self.xoutRgb.input)
        else:
            self.camRgb.preview.link(self.xoutRgb.input)

        self.detectionNetwork.out.link(self.nnOut.input)

        # Connect to device and start pipeline
        self.device = dai.Device(self.pipeline)

        # Output queues
        self.qRgb = self.device.getOutputQueue(name="rgb", maxSize=4, blocking=False)
        self.qDet = self.device.getOutputQueue(name="nn", maxSize=4, blocking=False)

    def frameNorm(self, frame, bbox):
        normVals = np.full(len(bbox), frame.shape[0])
        normVals[::2] = frame.shape[1]
        return (np.clip(np.array(bbox), 0, 1) * normVals).astype(int)

    def displayFrame(self, name, frame):
        color = (255, 0, 0)
        for detection in self.detections:
            bbox = self.frameNorm(frame, (detection.xmin, detection.ymin, detection.xmax, detection.ymax))
            cv2.putText(frame, self.labelMap[detection.label], (bbox[0] + 10, bbox[1] + 20), cv2.FONT_HERSHEY_TRIPLEX, 0.5, 255)
            cv2.putText(frame, f"{int(detection.confidence * 100)}%", (bbox[0] + 10, bbox[1] + 40), cv2.FONT_HERSHEY_TRIPLEX, 0.5, 255)
            cv2.rectangle(frame, (bbox[0], bbox[1]), (bbox[2], bbox[3]), color, 2)
        # Show the frame
        cv2.imshow(name, frame)

    def get_frame_with_detections(self):
        while True:
            if cv2.waitKey(1) == ord('q'):
                break
            
            if self.syncNN:
                inRgb = self.qRgb.get()
                inDet = self.qDet.get()
            else:
                inRgb = self.qRgb.tryGet()
                inDet = self.qDet.tryGet()

            if inRgb is not None:
                frame = inRgb.getCvFrame()
                cv2.putText(frame, "NN fps: ", (2, frame.shape[0] - 4), cv2.FONT_HERSHEY_TRIPLEX, 0.4, self.color2)

            if inDet is not None:
                # detections = inDet.detections
                self.detections.extend([detection for detection in inDet.detections if detection.label == 0])

            if frame is not None:
                self.displayFrame("rgb", frame)

                    