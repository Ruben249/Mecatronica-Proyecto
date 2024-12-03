import cv2
import mediapipe as mp
import numpy as np
import pyautogui

# Inicialización de MediaPipe para la detección de la mano
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(min_detection_confidence=0.7, min_tracking_confidence=0.5)
mp_drawing = mp.solutions.drawing_utils

# Función para capturar la pantalla
def capture_screen():
    # Captura de pantalla utilizando pyautogui
    screenshot = pyautogui.screenshot()
    # Convertir la captura de pantalla a un array de NumPy (de RGB a BGR)
    frame = np.array(screenshot)
    frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
    return frame

# Función para contar los dedos levantados
def count_raised_fingers(landmarks):
    # Asumiendo que la muñeca está en el punto 0 y la base del dedo en el 1
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
    cv2.namedWindow("Live Screen Capture", cv2.WINDOW_AUTOSIZE)
    
    while True:
        # Obtener la imagen de la captura de pantalla
        frame = capture_screen()

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
                    print("2: Dos dedos levantados")
                elif raised_fingers == 5:
                    print("3: Mano abierta")
                else:
                    print("4: Otro gesto o sin mano")

        # Mostrar la imagen con las marcas de las manos
        cv2.imshow('Live Screen Capture', frame)

        # Salir si se presiona 'q'
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cv2.destroyAllWindows()

if __name__ == '__main__':
    print("Iniciando transmisión...")
    detect_hand()
