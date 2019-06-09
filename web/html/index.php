<!doctype html>
<html lang="en">
  <head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.0/css/bootstrap.min.css">
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.0/jquery.min.js"></script>
  <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.4.0/js/bootstrap.min.js"></script>


    <?php

      include "./php/query.php";

      $serverName = "localhost";
      $userName   = "phpmyadmin";
      $password   = "klapausius23";
      $dbnames    = "nodes";

      $cnx  = new mysqli( $serverName, $userName, $password, $dbnames);

    ?>


    <!-- Custom styles for this template -->
    <link href="dashboard.css" rel="stylesheet">
  </head>

  <body>

    <nav class="navbar navbar-inverse">
      <div class="container-fluid">
        <div class="navbar-header">
          <button type="button" class="navbar-toggle" data-toggle="collapse" data-target="#myNavbar">
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>                        
          </button>
          <a class="navbar-brand" href="#">Logo</a>
        </div>
        <div class="collapse navbar-collapse" id="myNavbar">
          <ul class="nav navbar-nav">
            <li class="active"><a href="#">Home</a></li>
            <li><a href="#">About</a></li>
            <li><a href="#">Projects</a></li>
            <li><a href="#">Contact</a></li>
          </ul>
          <ul class="nav navbar-nav navbar-right">
            <li><a href="#"><span class="glyphicon glyphicon-log-in"></span> Login</a></li>
          </ul>
        </div>
      </div>
    </nav>

    <div class="container-fluid">
      <div class="row">

        <div class="col-md-1"></div>

        <div role="main" class="col-md-8 ml-sm-auto ">
          <div class="d-flex justify-content-between flex-wrap flex-md-nowrap align-items-center pb-2 mb-3 border-bottom">
            <h1 class="h2">Dashboard</h1>

            <div class="row">
              <div class="col-sm-4 btn-toolbar mb-2 mb-md-0">
                <div class="btn-group mr-2">
                  <button class="btn btn-sm btn-outline-secondary">Share</button>
                  <button class="btn btn-sm btn-outline-secondary">Export</button>
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

          <h2>Section title</h2>
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

    <!-- Bootstrap core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.0/jquery.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/mathjs/5.10.3/math.js" type="text/javascript"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.12.9/umd/popper.min.js" >
    <!-- Icons -->
    <script src="https://unpkg.com/feather-icons/dist/feather.min.js"></script>
    <script type="text/javascript" src="js/dataloader.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.1/Chart.min.js"></script>
    <script src="https://code.highcharts.com/highcharts.js"></script>


  </body>
</html>
