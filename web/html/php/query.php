<?php

    $get_sensor = "SELECT * FROM node_list ";

    $get_sensor_values = "SELECT Date, Val FROM SensorVals WHERE SensorID = %u ORDER BY Date ASC";

    $get_sensors = "SELECT * FROM USensors  WHERE UUID = %u ORDER BY Ind ASC";

    $get_sensor_des = "SELECT Description FROM Sensors WHERE SensorType = %u";

?>