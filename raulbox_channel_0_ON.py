import serial as s

message = [0, 5]
ser = s.Serial('/dev/ttyUSB0', 38400)
b = bytes(message)
ser.write(b)
ser.close()

