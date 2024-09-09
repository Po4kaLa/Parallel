import cv2
import time
import threading
import queue
import argparse
import logging
import os
import numpy as np

log_dir = 'log'
os.makedirs(log_dir, exist_ok=True)
logging.basicConfig(filename=os.path.join(log_dir, 'sensor.log'), level=logging.ERROR)


class Sensor:
    def get(self):
        raise NotImplementedError("Subclasses must implement method get()")

class SensorX(Sensor):
    '''SensorX'''
    def __init__(self, delay:float):
        self._delay = delay
        self._data = 0
    
    def get(self) -> int:
        time.sleep(self._delay)
        self._data += 1
        return self._data


class SensorCam(Sensor):
    def __init__(self, camera_name: str, resolution: tuple):
        self.camera_name = camera_name
        self.resolution = resolution
        self.cap = cv2.VideoCapture(int(self.camera_name))

        if not self.cap.isOpened():
            logging.error(f"Camera {self.camera_name} could not be opened.")
            raise Exception(f"Camera {self.camera_name} could not be opened.")

        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, resolution[0])
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, resolution[1])

    def get(self):
        ret, frame = self.cap.read()
        if not ret:
            logging.error(f"Camera {self.camera_name} failed to read frame.")
            raise Exception(f"Camera {self.camera_name} failed to read frame.")
        return frame

    def __del__(self):
        self.cap.release()

class WindowImage:
    def __init__(self, display_frequency: float):
        self.display_frequency = display_frequency
        self.window_name = "Sensor Data"
        cv2.namedWindow(self.window_name)

    def show(self, img):
        cv2.imshow(self.window_name, img)
        cv2.waitKey(int(1000 / self.display_frequency))

    def __del__(self):
        cv2.destroyAllWindows()

def sensor_thread(sensor, data_queue, stop_event):
    while not stop_event.is_set():
        try:
            data = sensor.get()
            data_queue.put(data)
        except Exception as e:
            logging.error(str(e))
            break

# def sensor_thread(sensor, data_queue):
#     while True:
#         try:
#             data = sensor.get()
#             data_queue.put(data)
#         except Exception as e:
#             logging.error(str(e))
#             break

# def main(cam = 1, width=1280, height=720, display_frequency=100):
def main(cam, width, height, display_frequency):
    camera = SensorCam(cam, (width, height))
    sensors = [SensorX(0.1), SensorX(1), SensorX(10.0)]  # 10Hz, 1Hz, 0.1Hz
    data_queues = [queue.Queue() for _ in sensors]
    threads = []
    stop_event = threading.Event()
    for sensor, data_queue in zip(sensors, data_queues):
        thread = threading.Thread(target=sensor_thread, args=(sensor, data_queue, stop_event))
        thread.start()
        threads.append(thread)
    window = WindowImage(display_frequency=display_frequency)
    sensor_names = [f"SensorX_{i}" for i in range(len(sensors))]
    sensor_values = [None] * len(sensors)

    running = True
    while running:
        cam_frame = camera.get()
        for i, data_queue in enumerate(data_queues):
            if not data_queue.empty():
                sensor_values[i] = data_queue.get()

        for i, name in enumerate(sensor_names):
            value_text = f"{name}: {sensor_values[i]}" if sensor_values[i] is not None else f"{name}: Waiting..."
            cv2.putText(cam_frame, value_text, (10, 30 + i * 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
        window.show(cam_frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            running = False

    stop_event.set()  
    for thread in threads:
        thread.join()

    del window
    del camera
    print("Приложение завершено корректно.")
    return 0

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Camera and sensor data display.')
    parser.add_argument('--camera', type=str, required=True, help='Camera index or name')
    parser.add_argument('--resolution', type=str, default='1280x720', help='Resolution of the camera (widthxheight)')
    parser.add_argument('--frequency', type=float, default=30.0, help='Display frequency in Hz')

    args = parser.parse_args()
    width, height = map(int, args.resolution.split('x'))
    camera = args.camera
    display_frequency=args.frequency

    main(camera, width, height, display_frequency)
