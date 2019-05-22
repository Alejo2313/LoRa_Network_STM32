

add_node =  (   "INSERT INTO node_list "
                "(UUID, Nickname, ADDR, SENSOR_N, L_CONECTION)"
                " VALUES (%s,%s, %s,%s,%s) ")

is_update = (   "SELECT UPDT FROM node_list "
                "WHERE UUID = %s"
            )

unset_update = (    "UPDATE node_list SET UPDT = 0 "
                    "WHERE UUID = %s"
                )
add_addr = (    "INSERT INTO Address "
                "(ADDR, UUID, LDate)"
                "VALUES (%s, %s, %s)")

get_uuid = (    "SELECT UUID, LDate FROM Address "
                "WHERE ADDR = %s")

get_addr = (    "SELECT ADDR FROM Address WHERE UUID = %s")

upd_addr = (    "UPDATE Address SET  UUID = %s, LDate = %s "
                "WHERE ADDR = %s")

add_sensors =   (   "INSERT INTO USensors "
                    "(UUID, SensorType, Ind, Size) "
                    "VALUES (%s, %s, %s, %s) ")     

add_value   =   (   "INSERT INTO SensorVals "
                    "(SensorID, Date, Val) "
                    "VALUES (%s, %s,%s, %s) "
                )

get_sensors =   (   "SELECT SensorID, Size FROM USensors "
                    "WHERE UUID = %s "
                    "ORDER BY Ind ASC "
                )

upd_node_date = (   "UPDATE node_list SET L_CONECTION = %s " 
                    "WHERE UUID = %s"
                )

add_config = (  "INSERT INTO DefaultConfig "
                "( UUID, POWER, BW, SF, SLEEP, RXWIN) "
                " VALUES (%s, %s, %s, %s, %s, %s)"                   
             )                

update_config = (   "UPDATE DefaultConfig SET POWER = %s, BW = %s, SF = %s, SLEEP = %s, RXWIN = %s "
                    "WHERE UUID = %s"
                )  

get_config = ( "SELECT POWER, BW, SF, SLEEP, RXWIN FROM DefaultConfig "
                "WHERE UUID = %s"
            )   


get_def_config = ( "SELECT  POWER, BW, SF, SLEEP, RXWIN FROM DefaultConfig "
                "WHERE UUID = 0"
            )             