function hola(){

    alert("hola");
}


function getData(data, success){
    var url = "php/dataChart.php";

    $.ajax({
        type: "POST",
        url: url,
        data: data,
        success: success
    })
}

function updateNodeList(){
    getData({"action":4}, function(data){
        $('#node_list').html(data);
    });
}

function updateChart(labels, values){
    Highcharts.chart('myChart', {
        chart: {
          type: 'line'
        },
        title: {
          text: 'Monthly Average Temperature'
        },
        subtitle: {
          text: 'Source: WorldClimate.com'
        },
        xAxis: {
          categories: null
        },
        yAxis: {
          title: {
            text: 'Temperature (°C)'
          }
        },
        plotOptions: {
          line: {
            dataLabels: {
              enabled: false
            },
            enableMouseTracking: false
          }
        },
        series: [{
          name: 'nn',
          data: values
        }]
      });



}




function updateChart2(labels, values){
    var ctx = document.getElementById("myChart");
    var myChart = new Chart(ctx, {
      type: 'line',
      data: {
        labels: labels,
        datasets: [{
          data: values,
          lineTension: 0,
          backgroundColor: 'transparent',
          borderColor: '#007bff',
          borderWidth: 4,
          pointBackgroundColor: '#007bff'
        }]
      },
      options: {
        scales: {
          yAxes: [{
            ticks: {
              beginAtZero: false
            }
          }]
        },
        legend: {
          display: false,
        }
      }
    });
}

$(document).ready(function(){    

    getData({"action":0}, function(data){
        $('#node').html(data);
    });

    updateChart(null, null);
    

    $('#id').click(function(){

        var url = "php/dataChart.php";
        var UUID = $(this).val();

    });

    $('button.EDBTN').click(function(){

        if($("#edit_"+$(this).val()).prop('disabled')){
            $("#edit_"+$(this).val()).prop('disabled', false);
            $(this).html("Save!");
        }
        else{
            
            var UUID = $(this).val();
            var name = $("#edit_"+UUID).val();

            $("#edit_"+$(this).val()).prop('disabled', true);


            getData({"action":3 ,'UUID': UUID, 'Nickname': name }, function(data){
                alert("Node " + UUID + "name changed to "+name);
            });

            $(this).html("Edit!");
        }


    });

    $('#node').change(function(){

        var url = "php/dataChart.php";
        var UUID = $(this).val();

        $.ajax({
            type: "POST",
            url: url,
            data: {"action":1 ,'UUID': UUID},
            success: function(msg){
                $('#sensors').html(msg);
            }
        })

        
    });



    $('#sensors').change(function(){

        var url = "php/dataChart.php";
        var SensorID = $(this).val();

        $.ajax({
            type: "POST",
            dataType: 'json',
            url: url,
            data: {"action":2 ,'SensorID': SensorID},
            success: function(data){
                
                var labels = [];
                var values = [];
                var times =[];

                var sep = new Date( data[0].label);

                for(var i in data){
                    labels.push( data[i].label );
                    values.push( data[i].value );

                    if(i > 0){
                        aux = new Date( data[i].label)
                        times.push(aux - sep);
                        sep = aux;
                    }

                }
                
                updateChart(labels, values);

                $('#Mean').html(math.round(math.mean(values)));
                $('#TimeS').html(math.round(math.mean(times)/1000, 2));               
                $('#MaxVal').html(math.max(values));
                $('#MinVal').html(math.min(values));
                $('#Samples').html(values.length);
                $('#Median').html(math.median(values));
                $('#Variance').html(math.var(values));

            }
        })

    });
});