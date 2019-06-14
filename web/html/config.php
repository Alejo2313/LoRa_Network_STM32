
<div class="container">


<div class="panel panel-default">

<div class="panel-heading">Sensor Configuration</div>
<div class="panel-body">

<div class = "col-sm-offset-1">
<form class="form-horizontal " action="./index.php?id=1" method="post">
    <div class="form-group ">
        <label for="node">Node:</label>
        <select name="node">
 <?php 
        $result = $cnx->query($get_sensor);
        if ($result->num_rows > 0) {
            // output data of each row
             while($row = $result->fetch_assoc()) {
                echo  '<option value="' .($row["UUID"]).'">'.($row["Nickname"]).'</option> \n';
            }
        }

 ?>           
        </select>
    </div>

    <div class="control-group">
        <span class="control-label">.</span>
        <div class="controls form-inline">
          <div class="col-md-12 col-sm-offset-1">
            <label for="inputKey">Rx Window</label>
            <input type="number" name="rxwin" min="1" max="10" value = "1">

            <label for="inputKey">Sleep Time</label >
            <input type="number" name="sleep" min="1" max="65535">
          </div>
        </div>
    </div>


    <div class="control-group">
        <span class="control-label">.</span>
        <div class="controls form-inline">
          <div class="col-md-12 col-sm-offset-1">
            <label for="inputKey">Spreading </label>
            <input type="number" name="sf" min="7" max="12" value="7">


            <label for="power">Power Tx</label>
            <input type="number" name="power" min="0" max="20" value = "14">
          </div>
        </div>
    </div>

    <div class="control-group">
        <span class="control-label">.</span>
        <div class="controls form-inline">
          <div class="col-md-12 ">
            <label for="bw">Bandwidth:</label>
            <select name="bw">
              <option value="0"> 125 KHz</option>
              <option value="1"> 250 KHz</option>
              <option value="2"> 500 KHz</option>
            </select>
          </div>
        </div>
    </div>

    <div class="control-group">
      <label><input type="checkbox" name="default"> Set as default configuration</label>
    </div>
    <div class=" col-sm-offset-9">
      <button type="submit" class="btn btn-default" id="setConf">Submit</button>
    </div>
  </form>
  </div>
  </div>
</div>

</div>