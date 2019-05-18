$(document).ready(function(){

    $('#id').click(function(){

        var url = "/php/dataChart.php";
        var UUID = $(this).val();

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

                for(var i in data){
                    labels.push( data[i].label );
                    values.push( data[i].value );
                }

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
        })

    });
});