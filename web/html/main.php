

<div class="container-fluid">
      <div class="row">

        <div class="col-md-1"></div>

        <div role="main" class="col-md-8 ml-sm-auto ">
          <div class="d-flex justify-content-between flex-wrap flex-md-nowrap align-items-center pb-2 mb-3 border-bottom">
            <h1 class="h2">Dashboard</h1>

            <div class="row">
              <div class="col-sm-4 btn-toolbar mb-2 mb-md-0">
                <div class="btn-group mr-2">
                  <button class="btn btn-sm btn-outline-secondary" id="export">Export</button>
                </div>
                <button class="btn btn-sm btn-outline-secondary dropdown-toggle">
                  <span data-feather="calendar"></span>
                  This week
                </button>
              </div>

              <div class="col-sm-4"></div>



              <form action="/action_page.php">
                <select id = "node" name="nodes" >
                  <option value="" selected disabled hidden>Choose Node</option>
                  </select>
                
                
                <select  id = 'sensors' name="sensors">
                  <option value="" selected disabled hidden>Choose Sensor</option>'
                </select>
                
              </form>
                
              
            </div>

          </div>

          <canvas class="my-4" id="myChart"style="min-width: 310px; height: 400px; margin: 0 auto"></canvas>

          <h2>Sensor List</h2>
          <div class="table-responsive">
            <table class="table table-striped table-sm">
              <thead>
                <tr>
                  <th>UUID</th>
                  <th>Nickname</th>
                  <th>Address</th>
                  <th>Sensor's Number</th>
                  <th>Last connection</th>
                  <th></th>
                </tr>
              </thead>
              <tbody id="node_list">

              <?php

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

              ?>

              </tbody>
            </table>
          </div>
        </div>

        <div class="col-md-3">
          <div class="panel panel-default">
            <div class="panel-heading">Sensor Statics</div>
            <div class="panel-body">
            <ul class="list-group">
              <li class="list-group-item">Max value = <span id="MaxVal" > </span> </li>
              <li class="list-group-item">Min value = <span id= "MinVal"> </span></li>
              <li class="list-group-item">Samples = <span id="Samples" > </span></li>
              <li class="list-group-item">Time separation = <span id="TimeS" > </span></li>
              <li class="list-group-item">Variance = <span id="Variance" > </span></li>
              <li class="list-group-item">Mean = <span id="Mean" > </span></li>
              <li class="list-group-item">Median = <span id="Median" > </span></li>

            </ul> 
          
            </div>
          </div>
        </div>

      </div>
    </div>


