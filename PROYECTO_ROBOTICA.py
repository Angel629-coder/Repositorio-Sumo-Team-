import tkinter as tk
from tkinter import messagebox
from tkinter import Listbox
import serial
import serial.tools.list_ports
import numpy as np

def matriz_rotacion_x(angulo):
    angulo = np.radians(angulo)
    return np.array([
        [1, 0, 0, 0],
        [0, np.cos(angulo), -np.sin(angulo), 0],
        [0, np.sin(angulo), np.cos(angulo), 0],
        [0, 0, 0, 1]
    ])

def matriz_rotacion_z(angulo):
    angulo = np.radians(angulo)
    return np.array([
        [np.cos(angulo), -np.sin(angulo), 0, 0],
        [np.sin(angulo), np.cos(angulo), 0, 0],
        [0, 0, 1, 0],
        [0, 0, 0, 1]
    ])

def matriz_traslacion_z(distancia):
    return np.array([
        [1, 0, 0, 0],
        [0, 1, 0, 0],
        [0, 0, 1, distancia],
        [0, 0, 0, 1]
    ])

def cinemática_directa(l1, l2, q1, q2, q3):
    A1 = matriz_rotacion_x(-90)
    A2 = matriz_rotacion_z(-q1)
    A3 = matriz_rotacion_x(90)
    A4 = matriz_rotacion_z(-q2)
    A5 = matriz_rotacion_x(-90)
    A6 = matriz_traslacion_z(l1)
    A7 = matriz_rotacion_x(90)
    A8 = matriz_rotacion_z(-q3)
    A9 = matriz_rotacion_x(-90)
    A10 = matriz_traslacion_z(l2)
    A11 = matriz_rotacion_x(90)
    
    T = A1 @ A2 @ A3 @ A4 @ A5 @ A6 @ A7 @ A8 @ A9 @ A10 @ A11
    return T[0, 3], T[1, 3], T[2, 3]

import numpy as np

def calcular_cinematica_inversa(px, py, pz, l1, l2):
    # Cálculo de q1
    q1_inverso = np.arctan2(py, px)
    
    # Proyección en el plano xy
    r = np.sqrt((px * px) + (py * py))
    
    # Distancia efectiva desde la base en el plano z
    d = pz
    
    # Cálculo de cos(q3)
    cos_q3 = ((r * r) + (d * d) - (l1 * l1) - (l2 * l2)) / (2 * l1 * l2)
    
    # Asegurarnos de que cos_q3 esté dentro del rango [-1, 1]
    if cos_q3 < -1:
        cos_q3 = -1
    elif cos_q3 > 1:
        cos_q3 = 1
    
    # Cálculo de q3
    sin_q3 = np.sqrt(1 - (cos_q3 * cos_q3))
    q3_inverso = np.arctan2(sin_q3, cos_q3)
    
    # Cálculo de q2
    k1 = l1 + (l2 * np.cos(q3_inverso))
    k2 = l2 * np.sin(q3_inverso)
    q2_inverso = np.arctan2(d, r) - np.arctan2(k2, k1)
    
    return np.degrees(q1_inverso), np.degrees(q2_inverso), np.degrees(q3_inverso)

def obtener_puertos_disponibles():

    return [port.device for port in serial.tools.list_ports.comports()]

def enviar_serial(data, puerto):
    try:
        ser = serial.Serial(puerto, 9600)
        ser.write(data.encode())
        ser.close()
    except Exception as e:
        messagebox.showerror("Error de conexión", f"No se pudo conectar al puerto serial: {e}")

def calcular():
    try:
        l1 = float(entry_l1.get())
        l2 = float(entry_l2.get())
        q1 = float(entry_q1.get())
        q2 = float(entry_q2.get())
        q3 = float(entry_q3.get())
        
        x, y, z = cinemática_directa(l1, l2, q1, q2, q3)
        label_directa.config(text=f"Cinemática Directa: X={x:.2f}, Y={y:.2f}, Z={z:.2f}")
        
        q1_, q2_, q3_ = calcular_cinematica_inversa(x, y, z, l1, l2)
        label_inversa.config(text=f"Cinemática Inversa: q1={q1_:.2f}, q2={q2_:.2f}, q3={q3_:.2f}")
        
        # Guardar ángulos globalmente para el envío
        global angulos_calculados
        angulos_calculados = (q1, q2, q3)
    except ValueError:
        messagebox.showerror("Entrada inválida", "Por favor, ingresa valores numéricos válidos.")

def enviar_datos():
    try:
        puerto_seleccionado = listbox_puertos.get(listbox_puertos.curselection())
        if not angulos_calculados:
            raise NameError("No hay valores calculados")
        
        q1, q2, q3 = angulos_calculados
        data = f"{int(q1)},{int(q2)},{int(q3)}"
        enviar_serial(data, puerto_seleccionado)
        messagebox.showinfo("Enviado", f"Datos enviados correctamente: {data}")
    except NameError:
        messagebox.showerror("Error", "Primero calcula los valores antes de enviarlos.")
    except IndexError:
        messagebox.showerror("Error", "Selecciona un puerto antes de enviar los datos.")

def actualizar_puertos():
    listbox_puertos.delete(0, tk.END)
    puertos_disponibles = obtener_puertos_disponibles()
    for puerto in puertos_disponibles:
        listbox_puertos.insert(tk.END, puerto)

root = tk.Tk()
root.title("Calculadora de Cinemática")

tk.Label(root, text="Longitud de Brazo 1 (L1)").grid(row=0, column=0)
tk.Label(root, text="Longitud de Brazo 2 (L2)").grid(row=1, column=0)
tk.Label(root, text="Ángulo 1 (Q1)").grid(row=2, column=0)
tk.Label(root, text="Ángulo 2 (Q2)").grid(row=3, column=0)
tk.Label(root, text="Ángulo 3 (Q3)").grid(row=4, column=0)

entry_l1 = tk.Entry(root)
entry_l2 = tk.Entry(root)
entry_q1 = tk.Entry(root)
entry_q2 = tk.Entry(root)
entry_q3 = tk.Entry(root)

entry_l1.grid(row=0, column=1)
entry_l2.grid(row=1, column=1)
entry_q1.grid(row=2, column=1)
entry_q2.grid(row=3, column=1)
entry_q3.grid(row=4, column=1)

listbox_puertos = Listbox(root, selectmode=tk.SINGLE)
listbox_puertos.grid(row=8, column=0, columnspan=2)
actualizar_puertos()

tk.Button(root, text="Calcular", command=calcular).grid(row=5, column=0, columnspan=2)
tk.Button(root, text="Enviar Datos", command=enviar_datos).grid(row=6, column=0, columnspan=2)
tk.Button(root, text="Actualizar Puertos", command=actualizar_puertos).grid(row=7, column=0, columnspan=2)

label_directa = tk.Label(root, text="Cinemática Directa: X=, Y=, Z=")
label_directa.grid(row=9, column=0, columnspan=2)
label_inversa = tk.Label(root, text="Cinemática Inversa: q1=, q2=, q3=")
label_inversa.grid(row=10, column=0, columnspan=2)

# Inicializar variable global para almacenar ángulos calculados
angulos_calculados = None

root.mainloop()
