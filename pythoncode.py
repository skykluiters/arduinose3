import serial
import time
import tkinter as tk
from tkinter import Label, messagebox

# Seriële poort instellen
ser = serial.Serial('COM4', 9600, timeout=1)
time.sleep(2)

dark_detected = False  
last_dark_time = 0
sound_detected = False  
last_sound_time = 0    

def show_popup(value):
    root = tk.Tk()
    root.withdraw()
    root.attributes('-topmost', True)  # Zorgt dat het altijd bovenaan komt
    if value == "DARK":
        messagebox.showwarning("Waarschuwing", "Het is te donker in de kamer!")
    if value == "LUID":
        messagebox.showwarning("Waarschuwing", "Het is te luid! Wees wat zachter.")
    root.destroy()

def update_values():
    global dark_detected, last_dark_time, sound_detected, last_sound_time
   
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').strip()
        ser.flush()
        print(line)
       
        if "AirQuality=" in line:
            co2_value = line.split('=')[1].split(' ')[0]
            co2_label.config(text=f"CO₂: {co2_value} PPM")
       
        elif "Temperatuur:" in line:
            parts = line.split(', ')
            temp_value = parts[0].split(': ')[1].strip()
            humidity_value = parts[1].split(': ')[1].strip()
            temp_label.config(text=f"Temperatuur: {temp_value}")
            humidity_label.config(text=f"Luchtvochtigheid: {humidity_value}")
       
        elif "DARK" in line:
            light_label.config(text="Licht: Te donker")
            current_time = time.time()
            if not dark_detected or (current_time - last_dark_time >= 10):
                show_popup("DARK")
                dark_detected = True
                last_dark_time = current_time
        elif "LIGHT" in line:
            light_label.config(text="Licht: Voldoende")
            dark_detected = False  # Reset bij licht
       
        elif "LUID" in line:
            sound_label.config(text="Geluid: Te luid")
            current_time = time.time()
            if not sound_detected or (current_time - last_sound_time >= 10):
                show_popup("LUID")
                sound_detected = True
                last_sound_time = current_time
        elif "STIL" in line:
            sound_label.config(text="Geluid: Stil")
            sound_detected = False  # Reset bij normaal volume
   
    root.after(10, update_values)  # Update elke 0.1 seconde

# GUI instellen
root = tk.Tk()
root.title("Sensorwaarden")
root.geometry("300x200")

co2_label = Label(root, text="CO₂: Laden...", font=("Arial", 12))
co2_label.pack(pady=5)

temp_label = Label(root, text="Temperatuur: Laden...", font=("Arial", 12))
temp_label.pack(pady=5)

humidity_label = Label(root, text="Luchtvochtigheid: Laden...", font=("Arial", 12))
humidity_label.pack(pady=5)

light_label = Label(root, text="Licht: Laden...", font=("Arial", 12))
light_label.pack(pady=5)

sound_label = Label(root, text="Geluid: Laden...", font=("Arial", 12))
sound_label.pack(pady=5)

update_values()
root.mainloop()
