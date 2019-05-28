import mysql.connector
import re 
import serial
import time
import sys

from query import *
from datetime import date, datetime, timedelta


port = '/dev/ttyACM0'
baudrate = 115200


try:
    ser = serial.Serial(port,baudrate)
except serial.SerialException:
    sys.exit("Error opening port! so, closing...")

#while(ser.is_open != True):


cnx = mysql.connector.connect(user='phpmyadmin', password='klapausius23',
                              host='127.0.0.1',
                              database='nodes')

cursor = cnx.cursor(buffered=True)

count = 0


while 1:

    tdata = ser.readline().decode('utf-8')       # Wait forever for anything
 #   tdata = input()

    print(str(tdata))
    
    splited = re.split('\s', tdata)
    
    date =  datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    if splited[3] == '3':

        print("Join request! \n")

        UUID = int(splited[5]+ splited[6], 16)
        cursor.execute( get_addr, (UUID,)  )

        addr =  cursor.fetchone()

        if( addr == None):
            print("new node "+splited[5]+ splited[6])
            cursor.execute( get_addr, (0,) )
            addr =  cursor.fetchone()

            cursor.execute(get_def_config)
            config = cursor.fetchone()

            ser.write(chr(addr[0]).encode())

            if config != None:
                for val in config:
                # ser.write(chr(val).encode())  #TODO finish it
                    print(str(val) + " ", end = '')

            print("\n address " + str(addr[0]))

            cursor.execute(add_config, (UUID, config[0], config[1], config[2], config[3], config[4]) )
            cursor.execute(upd_addr, (UUID, date, addr[0]) )
            cursor.execute(add_node, (UUID,"Node"+splited[5]+splited[6], addr[0], int(splited[4],16)-2, date))            


            for i in range(int(splited[4],16)-2):
                val = int(splited[i+7], 16) 
                cursor.execute(add_sensors, (UUID, val >> 2, i, (val &3) + 1) )


        else:

            cursor.execute(get_config, (UUID,))
            config = cursor.fetchone()

            if(config != None):
                for val in config:
                    #ser.write(chr(val).encode()) -> #TODO finish it
                    print(str(val) + " ", end = '')

            ser.write(chr(addr[0]).encode())

            print("\n address" + str(addr[0]))
            cursor.execute(upd_addr, (UUID, date, addr[0]) )
            cursor.execute(upd_node_date, (date, UUID) )

        


    elif splited[3] == '0':
        addr = int(splited[1], 16)

        cursor.execute(get_uuid, (addr,))
        UUID = cursor.fetchone()[0]

        cursor.execute(is_update, (UUID,))

        if cursor.fetchone()[0] == 1:
            cursor.execute(get_config,1 (UUID,))
            config = cursor.fetchone()

            print("ADDRESS " + str(addr))
            for val in config:
                ser.write(chr(val).encode()) #TODO ->FINISH IT
                print(str(val) + " ", end = '')
            
            cursor.execute(unset_update, (UUID,))


        if UUID != None:
            cursor.execute(get_sensors, (UUID,))

            index =  5
            for sensor in cursor.fetchall():

                value = ''

                for i in range(sensor[1]):
                    value = value + splited[index]
                    index = index + 1
                
                cursor.execute(add_value, (sensor[0], date, int(value, 16)) )
                cursor.execute(upd_addr, (UUID, date, addr) )
                cursor.execute(upd_node_date, (date, UUID) )
                

    cnx.commit()

cnx.close()

