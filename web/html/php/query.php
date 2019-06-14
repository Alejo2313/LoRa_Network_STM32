<?php

    $get_sensor = "SELECT * FROM node_list ORDER BY L_CONECTION DESC";

    $get_sensor_values = "SELECT Date, Val FROM SensorVals WHERE SensorID = %u ORDER BY Date ASC";

    $get_sensors = "SELECT * FROM USensors  WHERE UUID = %u ORDER BY Ind ASC";

    $get_sensor_des = "SELECT Description FROM Sensors WHERE SensorType = %u";


    $set_config = "UPDATE DefaultConfig SET POWER = %u, BW = %u, SF = %u, SLEEP = %u, RXWIN = %u WHERE UUID = %u ";
    $update_nickname = "UPDATE node_list SET Nickname = '%s' WHERE UUID = %u";

    $enable_update = "UPDATE node_list SET UPDT = '1' WHERE UUID = %u";
?>