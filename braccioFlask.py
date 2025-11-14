from flask import Flask, render_template, request
import serial
import logging

logging.basicConfig(level=logging.DEBUG)

app = Flask(__name__)

#arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
arduino = serial.Serial('/dev/ttyACM1', 9600, timeout=1)

@app.route('/')
def index():
    return render_template('braccio4.html')

@app.route('/move', methods=['POST'])
def move():
    data = request.json
    logging.debug(f"Move request received with servo values: base={data['base']}, shoulder={data['shoulder']}, elbow={data['elbow']}, wrist_ver={data['wrist_ver']}, wrist_rot={data['wrist_rot']}, gripper={data['gripper']}")

    command = f"MOVE {data['base']} {data['shoulder']} {data['elbow']} {data['wrist_ver']} {data['wrist_rot']} {data['gripper']}\n"
    try:
        arduino.write(command.encode())
        arduino.flush()  # flush output buffer to ensure sending
        logging.debug("Command sent and flushed successfully.")
    except Exception as e:
        logging.error(f"Failed to send command: {e}")

    return '', 204

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)