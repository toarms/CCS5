#! python3
import serial
import struct
from datetime import datetime

ser = serial.Serial(4, 115200, timeout=1)

f = open('C:/Users/PC/Desktop/hello.log', 'w')
f2 = open('C:/Users/PC/Desktop/hello2.log', 'w')

k=0
j=0

last = datetime.utcnow()
for i in range(1,10000000):
 b = ser.read()
 print("(1):" , b);
 if b == b'\x00': 
#   print("(2):", b);
   b2 = ser.read()
   print("(3):", b2);
   if b2 == b'\xff':
#     print("(4):", b2);
     this = datetime.utcnow()
     c = this - last
     last = datetime.utcnow()
     value = (this, c.microseconds)
     s = str(value)
     f.write(s)
     f.write("\t\t\t\n")
     f2.write(s)
     f2.write("\t\t\t\n")

     b = ser.read(2)
     value = (b)
     s2 = str(value)

     freq_LSB = ser.read()
     freq_HSB = ser.read()
     count_LSB = ser.read()
     count_HSB = ser.read()
     if b == b'if':
       k += 1
       freq = ord(freq_HSB) * 256 + ord(freq_LSB)
       f.write(str(k))
       f.write(" ")
       f.write(str(freq))
       f.write(" ")
       f.write(str(ord(count_HSB)))
       f.write(" ")
       f.write(str(ord(count_LSB)))
       f.write("\n")

     if b == b'rf':
       j += 1
       freq = ord(freq_HSB) * 256 + ord(freq_LSB)
       f2.write(str(j))
       f2.write(" ")
       f2.write(str(freq))
       f2.write(" ")
       f2.write(str(ord(count_HSB)))
       f2.write(" ")
       f2.write(str(ord(count_LSB)))
       f2.write("\n")

#     f.write(s)



# ss = str(value)
# f.write(ss)
# f.write('\n')

