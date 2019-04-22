import pymongo
import datetime
import serial
import time
import re 
import sys

verbose = True

##database config
dbname = "nodedb"

##Serial config

port = '/dev/ttyACM0'
baudrate = 115200

if verbose: 
    print('Configuring serial... \n \t port: '+port+ "\n \t baudrate: "+ str(baudrate)+"\n")

try:
    ser = serial.Serial(port,baudrate)
except serial.SerialException:
    sys.exit("Error opening port! so, closing...")

if ser.is_open != True:
    sys.exit("Can't open port! so, closing")

if verbose:
    print('Port ' + ser.name+ ' opended!\nListening..')
    print("Opening database... \n")

myclient = pymongo.MongoClient("mongodb://localhost:27017/")
dbList = myclient.list_database_names()

mydb = myclient[dbname]

if dbname in dbList:
    print("OK \n")
else:
    print("database no found!, creating! \n")
    
col = mydb.list_collection_names()
print(col)

if "address" in col:
    colAddr = mydb["address"]
else:
    print("adress collection no exist! \n")
    
    colAddr = mydb["address"]
    for i in range(256):
        dic =colAddr.insert_one({"_id":i, "used":False})

if "tsensor" in col:
    colTSensor = mydb["tsensor"]
else:
    print("No sensor type collection!, creating \n")
    colTSensor = mydb["tsensor"]
    for i in range(64):
        colTSensor.insert_one({"_id": i, "desc": "sensor "+str(i)})


colNodes = mydb["nodes"]
colSensor = mydb["sensor"]
colValues = mydb["values"]

while 1:
    tdata = ser.read_until();          # Wait forever for anything
    print(tdata)
    
    splited = re.split('\s', tdata)
    
    print(splited[3])
    if splited[3] == '3':

        print("Join request! \n")
        addr = colAddr.find_one({"used":False})
        ser.write(chr(addr["_id"]))
        colAddr.update(addr, {"$set":{"used":True}})

        UUID = int(splited[5]+ splited[6], 16)

        if colNodes.find({"_id":UUID}).count() == 0:
            colNodes.insert_one({"_id": UUID, "last":datetime.datetime.utcnow() , "address_id": addr["_id"], "nsensor": int(splited[4],16)-2})
            print("new node "+splited[5]+ splited[6])

            for i in range(int(splited[4],16)-2):
                val = int(splited[i+7], 16) 
                colSensor.insert_one({"node_id": UUID, "tsensor_id": val >> 2, "index": i, "size": (val &3) + 1})
                
        else:
            colNodes.update({"_id": UUID}, { "$set":{"last":datetime.datetime.utcnow(), "address_id": addr["_id"],"nsensor": int(splited[4],16)-2}})




    elif splited[3] == '0':

        node = colNodes.find_one({"address_id": int(splited[1], 16)})
        if len(node) == 0:
            print("ERROR, no exist!")
        else:
            sensors = colSensor.find({"node_id": node["_id"] }).sort("index", pymongo.ASCENDING)
            index = 5
            for s in sensors:
                value = ''
                for i in range(s["size"]):
                    value = value + splited[index]
                    index = index + 1
                colValues.insert_one({"sensor_id": s["_id"], "value" : int(value, 16), "date": datetime.datetime.utcnow()})

        

        
        


    
