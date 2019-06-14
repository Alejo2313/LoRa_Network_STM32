<?php 

include 'header.php';

if(isset($_GET["id"] )){
  switch($_GET["id"]){

    case 1:
      include 'config.php';

      if(isset($_POST["rxwin"])){
        $query = sprintf($set_config, $_POST["power"],$_POST["bw"],$_POST["sf"],$_POST["sleep"],$_POST["rxwin"], $_POST["node"] );
        $result = $cnx->query($query);

        $query = sprintf($enable_update,  $_POST["node"]);
        $result = $cnx->query($query);

        if(isset( $_POST["default"])){
          $query = sprintf($set_config, $_POST["power"],$_POST["bw"],$_POST["sf"],$_POST["sleep"],$_POST["rxwin"], 0 );
          $result = $cnx->query($query);
        }
      }

      break;
    case 2 :
      include "about.php";
      break;
    
    case 3:
      include "contact.php";
      break;
      
  }
}
else {
  include 'main.php';
}

include 'footer.php';


?>