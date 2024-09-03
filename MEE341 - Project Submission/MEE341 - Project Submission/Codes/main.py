import cv2 as cv
import numpy as np
import sys
import serial
import time

# Initialize Serial communication
ser = serial.Serial('COM7', 9600)  
ser.flush()

video = cv.VideoCapture(0)

if not video.isOpened():
    sys.exit("No camera detected")

while True:
    ret, frame = video.read()
    
    if not ret:
        print("Not all frames captured")
        break  # Exit the loop if no frames are captured
    
    cvt = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
    
    lower_brown = np.array([10, 50, 50])
    upper_brown = np.array([30, 255, 255])
    
    mask = cv.inRange(cvt, lower_brown, upper_brown)
    
    kernel = np.ones((5,5), np.uint8)
    reduce = cv.morphologyEx(mask, cv.MORPH_OPEN, kernel)
    
    cv.imshow("reduce", reduce)
    cv.imshow("frame", frame)
    
    key = cv.waitKey(1)
    
    if key == ord("q"):
        break

    brown_area = cv.countNonZero(reduce)
    if brown_area > 5000:  # Adjust threshold as needed
        ser.write(b'b')    # Send command to Arduino
        ser.flush()        # Flush serial buffer to ensure immediate data transmission

# Close OpenCV windows
cv.destroyAllWindows()

# Close the video capture device
if video.isOpened():
    video.release()

# Close Serial connection
ser.close()
