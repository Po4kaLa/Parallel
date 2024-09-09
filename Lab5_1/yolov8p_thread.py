import cv2  
import threading  
import time  
from ultralytics import YOLO  
from queue import Queue 
import argparse

# Non-Thread-Safe Example: Single Model Instance
# from thrading import Thread
# from ultralytics import YOLO

# # Instantiate the model outside the thread

# shared_model = YOLO("yolov8n.pt")
# def predict(image_path):
#     """Predicts objects in an image using a preloaded YOLO model, take path string to image as argument."""
#     results = shared_model.predict(image_path)
#     # Process results

# # Starting threads that share the same model instance
# Thread(target=predict, args=("image1.jpg",)).start()
# Thread(target=predict, args=("image2.jpg",)).start()

# Thread-Safe Example
# Here's how to instantiate a YOLO model inside each thread for safe parallel inference:

# # Safe: Instantiating a single model inside each thread
# from threading import Thread

# from ultralytics import YOLO

# def thread_safe_predict(image_path):
#     """Predict on an image using a new YOLO model instance in a thread-safe manner; takes image path as input."""
#     local_model = YOLO("yolov8n.pt")
#     results = local_model.predict(image_path)
#     # Process results

# # Starting threads that each have their own model instance
# Thread(target=thread_safe_predict, args=("image1.jpg",)).start()
# Thread(target=thread_safe_predict, args=("image2.jpg",)).start()

def process_video_single_thread(video_path, output_path):
    model = YOLO("yolov8n-pose.pt") 
    cap = cv2.VideoCapture(video_path)
    width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    print(width, height)

    fourcc = cv2.VideoWriter_fourcc(*'mp4v') 
    out = cv2.VideoWriter(output_path, fourcc, 30.0, (width, height))
    start_time = time.time()
    while cap.isOpened():
        ret, frame = cap.read()
        
        if ret:
            results = results = model(frame, save=False, verbose=False) 
            annotated_frames = results[0].plot()
            out.write(annotated_frames)
            # if cv2.waitKey(1) & 0xFF == ord('q'):
            #     break
        else:
            break
    end_time = time.time()
    time_single = end_time - start_time
    cap.release()
    cv2.destroyAllWindows()
    return time_single


def process_video_multi_thread(video_path, output_path, num_threads):    
    cap = cv2.VideoCapture(video_path)    
    frames = []    
        
    while cap.isOpened():    
        ret, frame = cap.read()    
        if not ret:    
            break    
        frames.append(frame)    
    cap.release()    

    results_dict = {}  

    def worker(frame_queue):    
        model = YOLO("yolov8n-pose.pt")   
        while True: 
            index, frame = frame_queue.get() 
            if frame is None: 
                break 
            results = model(frame, save=False, verbose=False)   
            annotated_frame = results[0].plot()   
            results_dict[index] = annotated_frame  
            frame_queue.task_done() 

    frame_queue = Queue(maxsize=0) 
     
    threads = [] 
    for i in range(num_threads):    
        thread = threading.Thread(target=worker, args=(frame_queue,))    
        threads.append(thread)    
        thread.start()    

    start_time = time.time()    

    for index, frame in enumerate(frames): 
        frame_queue.put((index, frame))  

    frame_queue.join() 

    for _ in range(num_threads): 
        frame_queue.put((None, None)) 

    for thread in threads:    
        thread.join()

    height, width, _ = frames[0].shape
    fourcc = cv2.VideoWriter_fourcc(*'mp4v') 
    out = cv2.VideoWriter(output_path, fourcc, 30.0, (width, height))

    delay = int(1000/30) 
    for index in range(len(frames)): 
        out.write(results_dict[index])
        cv2.imshow("Inference", results_dict[index]) 
        cv2.waitKey(delay)   

    cv2.destroyAllWindows()    
    end_time = time.time()    
    return end_time - start_time   


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Process a video with YOLO.")
    parser.add_argument("video_path", type=str, help="Path to the input video (640x480 resolution).")
    parser.add_argument("mode", type=str, choices=["single", "multi"], help="Execution mode: 'single' or 'multi'.")
    parser.add_argument("output_path", type=str, help="Path to the output video file.")

    args = parser.parse_args()

    if args.mode == "single":
        execution_time = process_video_single_thread(args.video_path, args.output_path)
        print(f"Single-threaded execution time: {execution_time:.2f} seconds")
    
    elif args.mode == "multi":
        num_threads = 6  # Вы можете изменить количество потоков по своему усмотрению
        execution_time = process_video_multi_thread(args.video_path, args.output_path, num_threads)
        print(f"Multi-threaded execution time: {execution_time:.2f} seconds")