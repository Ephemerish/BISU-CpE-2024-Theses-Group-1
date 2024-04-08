import re
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import serial
from datetime import datetime
import threading

# Open serial port
ser = serial.Serial('COM4', baudrate=115200, timeout=1)

# List to store past data points
past_data = []
start_time = datetime.now()

# Initialize the plot
plt.figure(figsize=(12, 6))

# Subplot for the map
ax1 = plt.subplot(121)  # Map

# Subplot for the data feed
ax2 = plt.subplot(122)  # Data feed
ax2.axis('off')

# Function to update plot with new data
def update_plot(frame):
    global past_data, start_time
    
    try:
        # Clear previous plot
        ax1.clear()

        # Plot past data points
        for i in range(len(past_data)-1):
            data_point = past_data[i]
            next_data_point = past_data[i+1]
            age = (datetime.now() - data_point['date/time']).total_seconds()
            color = age_to_color(age)
            ax1.plot(data_point['lng'], data_point['lat'], 'o', color=color)
            ax1.plot([data_point['lng'], next_data_point['lng']], [data_point['lat'], next_data_point['lat']], color=color)  # Plot lines

        # Plot current data point
        if past_data:
            data_point = past_data[-1]
            age = (datetime.now() - data_point['date/time']).total_seconds()
            color = age_to_color(age)
            ax1.plot(data_point['lng'], data_point['lat'], 'o', color=color)  # Plot dots

        ax1.set_xlabel('Longitude')
        ax1.set_ylabel('Latitude')
        ax1.set_title('Location Data Visualization')
        ax1.grid(True)

        # Update data feed
        ax2.clear()
        ax2.annotate(f'Data Feed ({len(past_data)})', (0.1, 0.95), xycoords='axes fraction', fontsize=12, fontweight='bold')
        ax2.annotate(f'Time elapsed: {str(datetime.now() - start_time).split(".")[0]}', (0.6, 0.95), xycoords='axes fraction', fontsize=12, fontweight='bold')
        for idx, data_point in enumerate(reversed(past_data[-18:])):
            ax2.annotate(f"{data_point['date/time'].strftime('%H:%M:%S')} - {data_point['sender']} - Lat: {data_point['lat']}, Lng: {data_point['lng']}", 
                         (0.1, 0.90 - idx * 0.05), xycoords='axes fraction', fontsize=10)
    except Exception as e:
        print(f"Error updating plot: {e}")

# Function to convert age to color
def age_to_color(age):
    if age <= 60:  # Less than 1 minute
        # Linear interpolation between green and black
        green = 1.0 - age / 60.0
        return (0, green, 0)  # RGB tuple
    else:
        return 'black'

# Function to extract latitude and longitude from received packet
def extract_data(packet):
    packet_counter_match = re.search(r'P: (\d+)', packet)
    lat_match = re.search(r'Lat: ([\d.]+)', packet)
    lng_match = re.search(r'Lng: ([\d.]+)', packet)
    date_time_match = re.search(r'D/T: (\d+/\d+/\d+) (\d+:\d+:\d+)', packet)
    sender_match = re.search(r'S: ([a-f0-9]+)', packet)
    if packet_counter_match and lat_match and lng_match and date_time_match and sender_match:
        packet_counter = int(packet_counter_match.group(1))
        lat = float(lat_match.group(1))
        lng = float(lng_match.group(1))
        date = (date_time_match.group(1))
        time = (date_time_match.group(2))
        sender = (sender_match.group(1))
        return packet_counter, lat, lng, date, time, sender
    else:
        return None, None, None, None, None, None

# Function to update data
def update_data(packet_counter, lat, lng, date, time, sender):
    global past_data
    try:
        unique_combination = (lat, lng, time, sender)
        # if unique_combination not in [(entry['lat'], entry['lng'], entry['time'], entry['sender']) for entry in past_data] and sender == "8d1f9e80fa4":
        if unique_combination not in [(entry['lat'], entry['lng'], entry['time'], entry['sender']) for entry in past_data]:    
            past_data.append({
                'lat': lat,
                'lng': lng,
                'time': time,
                'date/time': datetime.now(),
                'sender': sender,
                'packet_counter': packet_counter
            })
            today = datetime.today().strftime("%d-%m-%Y")
            with open(f"{today}.csv", "a") as f:
                f.write(f"{packet_counter}, {lat}, {lng}, {date}, {time}, {sender}\n")
    except Exception as e:
        print(f"Error processing data: {e}")

# Initialize frame counter
update_data.frame_count = 0

# Update data every second
def update_data_wrapper():
    while True:
        try:
            # Read data from serial port
            data = ser.readline().decode().strip()
            if data:
                print(data)
                packet_counter, lat, lng, date, time, sender = extract_data(data)
                if lat is not None and lng is not None:
                    update_data(packet_counter, lat, lng, date, time, sender)
        except Exception as e:
            print(f"Error processing data: {e}")

# Create and start threads
update_data_thread = threading.Thread(target=update_data_wrapper)
update_data_thread.daemon = True
update_data_thread.start()

# Create animation
ani = FuncAnimation(plt.gcf(), update_plot, interval=1000)

plt.tight_layout()
plt.show()
