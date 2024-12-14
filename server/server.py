from flask import Flask, request, render_template
import cv2

app = Flask(__name__)

sensor_data = []

@app.route('/receive_image', methods=['POST'])
def receive_image():
    if request.method == 'POST':
        image_data = request.get_data()
        with open('received_image.jpg', 'wb') as f:
            f.write(image_data)
        print("Image received and saved.")

        # Process the image using OpenCV
        img = cv2.imread('received_image.jpg')
        cv2.imshow('Received Image', img)
        cv2.waitKey(0)
        cv2.destroyAllWindows()

        return 'Image received successfully'
@app.route('/receive_data', methods=['POST'])
def receive_data():
    data = request.get_json()
    timestamp = data['timestamp']
    distance = data['distance']
    sensor_data.append({'timestamp': timestamp, 'distance': distance})

    # Process the received data
    print(f"Received data: timestamp={timestamp}, distance={distance}")

    # You can store the data in a database, trigger actions, or display it
    # ...

    return "Data received successfully"

@app.route('/')
def dashboard():
    # Process sensor data and create a chart or other visualization
    # ...

    return render_template('index.html', data=sensor_data)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)
