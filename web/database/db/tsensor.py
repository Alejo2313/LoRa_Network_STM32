import mysql.connector
import re 
import serial
import sys
from query import *
from datetime import date, datetime, timedelta


port = '/dev/ttyACM0'
baudrate = 115200

try:
    ser = serial.Serial(port,baudrate)
except serial.SerialException:
    sys.exit("Error opening port! so, closing...")

if ser.is_open != True:
    sys.exit("Can't open port! so, closing")



cnx = mysql.connector.connect(user='phpmyadmin', password='klapausius23',
                              host='127.0.0.1',
                              database='nodes')

cursor = cnx.cursor(buffered=True)



while 1:

    tdata = ser.read_until();          # Wait forever for anything

    #tdata = input("Insert paylaod test: ")

    print(tdata)
    
    splited = re.split('\s', tdata)
    
    date =  datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    if splited[3] == '3':

        print("Join request! \n")

        UUID = int(splited[5]+ splited[6], 16)
        cursor.execute( get_addr, (UUID,)  )

        addr =  cursor.fetchone()

        if( addr == None):
            print("new node "+splited[5]+ splited[6])
            cursor.execute( get_addr, (0,)  )
            addr =  cursor.fetchone()

            ser.write(chr(addr[0]))
            # print("address" + str(addr[0]))

            cursor.execute(upd_addr, (UUID, date, addr[0]) )

            cursor.execute(add_node, (UUID, addr[0], int(splited[4],16)-2, date))
            
            for i in range(int(splited[4],16)-2):
                val = int(splited[i+7], 16) 
                cursor.execute(add_sensors, (UUID, val >> 2, i, (val &3) + 1) )


        else:
            # ser.write(chr(addr[0]))

            print("address" + str(addr[0]))
            cursor.execute(upd_addr, (UUID, date, addr[0]) )
            cursor.execute(upd_node_date, (date, UUID) )



    elif splited[3] == '0':
        addr = int(splited[1], 16)

        cursor.execute(get_uuid, (addr,))

        UUID = cursor.fetchone()

        if UUID != None:
            cursor.execute(get_sensors, (UUID[0],))
            
            index =  5
            for sensor in cursor.fetchall():

                value = ''

                for i in range(sensor[1]):
                    value = value + splited[index]
                    index = index + 1
                
                cursor.execute(add_value, (sensor[0], date, int(value, 16)) )
                

    cnx.commit()

cnx.close()

