import cv2 as cv
import numpy as np

# Constants
ON_KEY = ord('n')
OFF_KEY = ord('f')
PLAYBACK_WAIT = 20
WINDOW_TITLE_ROOT = 'Video'

# Open video
vidObj = cv.VideoCapture("startStopExample.mp4")
if (vidObj.isOpened() is False):
    print("Error reading video file")

# Set up video label array
playback_array = np.zeros((1, 3))  # columns: frame num, on/off, IMU time
frame_num = 0

while(vidObj.isOpened()):
    frame_num += 1
    frame_array = np.array([frame_num, 0, 0])
    ret, frame = vidObj.read()
    if ret is True:
        print("Image frame ", frame_num)
        cv.imshow(WINDOW_TITLE_ROOT, frame)

        key = cv.waitKey(PLAYBACK_WAIT)

        if key == ON_KEY:
            print("ON", end=':')
            frame_array[1] = ON_KEY
            cv.setWindowTitle(WINDOW_TITLE_ROOT, WINDOW_TITLE_ROOT + ': On')

        if key == OFF_KEY:
            print("OFF", end=':')
            frame_array[1] = OFF_KEY
            cv.setWindowTitle(WINDOW_TITLE_ROOT, WINDOW_TITLE_ROOT + ': Off')

        playback_array = np.vstack([playback_array, frame_array])
    else:
        break

vidObj.release()

print(playback_array)

cv.destroyAllWindows()
