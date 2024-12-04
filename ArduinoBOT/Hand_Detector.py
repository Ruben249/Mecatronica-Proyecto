import cv2
import mediapipe as mp
import numpy as np
import mss
import pygetwindow as gw

# Inicialización de MediaPipe para la detección de la mano
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(min_detection_confidence=0.7, min_tracking_confidence=0.5)
mp_drawing = mp.solutions.drawing_utils

# Función para capturar la imagen de la ventana de Chrome con la URL
def capture_chrome_window():
    try:
        # Buscar la ventana de Chrome por su título (puedes ajustar el título si es necesario)
        window = gw.getWindowsWithTitle('ESP32 OV2460')[0]  # Asumiendo que el título es "ESP32 Camera"
        if window:
            # Capturar la región de la ventana de Chrome
            with mss.mss() as sct:
                # Definir la región para capturar
                monitor = {
                    'top': window.top,
                    'left': window.left,
                    'width': window.width,
                    'height': window.height
                }

                # Capturar la imagen de la ventana de Chrome
                screenshot = sct.grab(monitor)
                # Convertir la imagen a un array de NumPy (de BGRA a BGR)
                img = np.array(screenshot)
                img = cv2.cvtColor(img, cv2.COLOR_BGRA2BGR)
                return img
    except Exception as e:
        print(f"Error al capturar la ventana de Chrome: {e}")
        return None

# Función para contar los dedos levantados
def count_raised_fingers(landmarks):
    raised_fingers = 0
    
    # Dedo índice (dedo 1, base 0-1)
    if landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP].y < landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_MCP].y:
        raised_fingers += 1
    
    # Dedo medio (dedo 2, base 0-1)
    if landmarks.landmark[mp_hands.HandLandmark.MIDDLE_FINGER_TIP].y < landmarks.landmark[mp_hands.HandLandmark.MIDDLE_FINGER_MCP].y:
        raised_fingers += 1
    
    # Dedo anular (dedo 3, base 0-1)
    if landmarks.landmark[mp_hands.HandLandmark.RING_FINGER_TIP].y < landmarks.landmark[mp_hands.HandLandmark.RING_FINGER_MCP].y:
        raised_fingers += 1
    
    # Dedo meñique (dedo 4, base 0-1)
    if landmarks.landmark[mp_hands.HandLandmark.PINKY_TIP].y < landmarks.landmark[mp_hands.HandLandmark.PINKY_MCP].y:
        raised_fingers += 1
    
    # Dedo pulgar (dedo 0, base 0-1)
    if landmarks.landmark[mp_hands.HandLandmark.THUMB_TIP].x > landmarks.landmark[mp_hands.HandLandmark.THUMB_IP].x:
        raised_fingers += 1
    
    return raised_fingers

# Función para procesar el video y detectar la mano
def detect_hand():
    cv2.namedWindow("ESP32 Hand Detection", cv2.WINDOW_AUTOSIZE)
    
    while True:
        # Obtener la imagen desde la ventana de Chrome
        frame = capture_chrome_window()

        # Si la imagen no es válida, continuar al siguiente ciclo
        if frame is None:
            print("No se pudo obtener una imagen válida desde la ventana de Chrome.")
            continue

        # Convertir el frame a RGB para MediaPipe
        rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        
        # Procesar la imagen para detectar las manos
        results = hands.process(rgb_frame)
        
        # Dibujar los puntos de la mano en la imagen si se detecta alguna mano
        if results.multi_hand_landmarks:
            for landmarks in results.multi_hand_landmarks:
                mp_drawing.draw_landmarks(frame, landmarks, mp_hands.HAND_CONNECTIONS)

                # Contar los dedos levantados
                raised_fingers = count_raised_fingers(landmarks)

                # Imprimir el resultado dependiendo de los dedos levantados
                if raised_fingers == 0:
                    print("1: Mano cerrada")
                elif raised_fingers == 1:
                    print("2: Un dedo levantados")
                elif raised_fingers == 5:
                    print("3: Mano abierta")
                else:
                    print("4: Otro gesto o sin mano")

        # Mostrar la imagen con las marcas de las manos
        cv2.imshow('ESP32 Hand Detection', frame)

        # Salir si se presiona 'q'
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cv2.destroyAllWindows()

if __name__ == '__main__':
    print("Iniciando transmisión...")
    detect_hand()
