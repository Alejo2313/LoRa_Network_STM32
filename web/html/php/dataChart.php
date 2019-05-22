<?php   


    include "../php/query.php";

    $serverName = "localhost";
    $userName   = "phpmyadmin";
    $password   = "klapausius23";
    $dbnames    = "nodes";

    $cnx  = new mysqli( $serverName, $userName, $password, $dbnames);

    $action = $_POST['action'];

    if($action == 0){
        $result = $cnx->query($get_sensor);
        echo '<option value="" selected disabled hidden>Choose Node</option>';

        if ($result->num_rows > 0) {
          // output data of each row
          while($row = $result->fetch_assoc()) {
  
            echo '<option value="'. $row["UUID"].'" > '.($row["Nickname"]).'</otpion>'.PHP_EOL;
  
          }
        }
    }

    if($action == 1){
        $UUID = $_POST['UUID'];

        $query = sprintf($get_sensors, $UUID);

        $result = $cnx->query($query);

        echo '<option value="" selected disabled hidden>Choose Sensor</option>';

        if ($result->num_rows > 0) {
        // output data of each row
            
            while($row = $result->fetch_assoc()) {
                
                
                $res2 = $cnx->query(sprintf($get_sensor_des, $row["SensorType"]));

                if($res2->num_rows > 0){
                    $row2 = $res2->fetch_assoc();
                    
                    echo '<option value="'. $row["SensorID"].'" > '.( $row2["Description"]).' '.($row["Ind"]).'</otpion>'.PHP_EOL;
                }
                else{
                    echo '<option value="'. $row["SensorID"].'" >  User_def '.($row["Ind"]).'</otpion>'.PHP_EOL;
                }

            }
        }

    }
    elseif($action == 2){
        $SensorID = $_POST['SensorID'];

        $query = sprintf($get_sensor_values, $SensorID);

        //echo $query; 

        $result = $cnx->query($query);
        $jsonArray = array();
        if ($result->num_rows > 0) {
            while($row = $result->fetch_assoc()) {

                $jsonArrayItem = array();
                $jsonArrayItem['label'] = $row['Date'];
                $jsonArrayItem['value'] = $row['Val'];
                //append the above created object into the main array.
                array_push($jsonArray, $jsonArrayItem);
            }

            header('Content-type: application/json');
            //output the return value of json encode using the echo function.
            echo json_encode($jsonArray);
        }

    }

    elseif($action == 3){
        $UUID = $_POST['UUID'];
        $name = $_POST['Nickname'];

        $query = sprintf($update_nickname, $name, $UUID);
        $cnx->query($query);

        echo "OK";
    }

    elseif($action == 4){
        $result = $cnx->query($get_sensor);

        if ($result->num_rows > 0) {
          // output data of each row
          while($row = $result->fetch_assoc()) {
  
            echo '<tr>
            <td>' .($row["UUID"]).'</td>
            <td>  <input type="text" id="edit_'.($row["UUID"]).'" value="'.($row["Nickname"]).'" disabled></td>
            <td>' .($row["ADDR"]).'</td>
            <td>' .($row["SENSOR_N"]).'</td>
            <td>' .($row["L_CONECTION"]).'</td>
            <td> <button type="button" class="EDBTN" value="'.($row["UUID"]).'">Edit</button>
          </tr>';
  
          }
        }
        else {
          echo "0 results";
        }
    }


?>