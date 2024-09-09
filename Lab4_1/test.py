import cv2

cap = cv2.VideoCapture("1")  # Замените 0 на нужный индекс вашей камеры
while True:
    ret, frame = cap.read()
    if ret:
        cv2.imshow('Camera', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()