import sys
import serial

PORT = '/dev/ttyUSB0'
BAUD_RATE = 38400
MIN_STRENGTH = 1
MAX_STRENGTH = 5
MIN_EBOW = 1
MAX_EBOW = 7
EBOW_CMD = [0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60]
SLEEP_CMD = 0xFF

def usage():
    print("Usage: rb <ebow> <strength>")
    print("   to sets the specified EBOW to strength") 
    print("   where <ebow> is an integer from 1 to 5")
    print("     and <strength> is an integer from 1 (min power) to 5 (max power)")
    print("Also \"rb -1\" deactivates all EBOWS and puts the Raulbox in sleep mode")
    sys.exit() 

#
# Command line validation
#

sleep = False

if len(sys.argv) <= 1:
    usage()

try:
    ebow = int(sys.argv[1])
except ValueError:
    usage()

if ebow == -1:
    sleep = True
elif ebow < MIN_EBOW or ebow > MAX_EBOW:
    usage()

if not sleep:
    try:
        strength = int(sys.argv[2])
    except ValueError:
        usage()
        
    if strength < MIN_STRENGTH or strength > MAX_STRENGTH:
        usage()
#    if strength == MIN_STRENGTH:
#        strength = MAX_STRENGTH + 1

#
# Issue command to Raulbox
#

s = serial.Serial(PORT, BAUD_RATE)
if sleep:
    message = [0, SLEEP_CMD]
else:
    message = [0, EBOW_CMD[ebow-1]+strength]
b = bytes(message)
s.write(b)
s.close()


