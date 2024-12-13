RoomClimateMonitor

RoomClimateMonitor is an embedded systems project designed to monitor and display environmental conditions such as temperature and humidity, and calculate the Comfort Index to provide insights into room climate quality. The system is implemented using 4 Arduino UNOs, a DHT11 sensor, and a button, with inter-board communication facilitated by I2C serial communication.
Features

    Temperature and Humidity Measurement: Utilizes the DHT11 sensor to collect environmental data.

    Comfort Index Calculation: Calculates a simplified comfort index based on temperature and humidity readings.

    Multi-Board Architecture:
        Board 1: Reads temperature and humidity from the DHT11 sensor.
        Board 2: Handles LCD display and user interface.
        Board 3: Processes the Comfort Index calculation and 
        Board 4: handles button inputs.

    I2C Communication:
        Data exchange between boards is achieved through I2C, with each board assigned a unique address.

    User Interaction:
        A button allows toggling between modes for displaying temperature, humidity, and the Comfort Index.

    Persistent Storage:
        The button press count is stored using the EEPROM to retain the state across power cycles.

Hardware Requirements

    4 Arduino UNO boards
    DHT11 sensor
    16x2 LCD with I2C module
    Push Button
    Jumper wires
    Breadboard

Software Requirements

    Arduino IDE
    DHT11 Library: Adafruit DHT Sensor Library
    Wire Library (for I2C communication)

System Architecture

The system is divided into four primary components:
Board 1: Sensor Node

    Reads temperature and humidity using the DHT11 sensor.
    Sends data to Board 2 or Board 3 for processing.

Board 2: Display Node

    Receives environmental data via I2C.
    Displays temperature, humidity, or Comfort Index based on the selected mode.

Board 3: Processing Node

    Calculates the Comfort Index based on received temperature and humidity.
    Handles user input through a button.
    Acts as an I2C master or intermediary for data requests.

Board 4:Button 

    it handles button inputs alternating between the different display modes.
    
