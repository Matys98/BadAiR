var sock = new WebSocket("ws://localhost:3000");

async function send_command(data) {
  sock.send(data);
}

async function update_database() {
  sock.send("<Update>");
}

async function clear_database() {
  sock.send("<Clear>");
  let response = await fetch("/readings", {
    method: "delete",
    headers: {
      "Content-Type": "application/json"
    }
  });
}

async function get_from_database() {
  try {
    let response = await fetch("/readings", {
      method: "get",
      headers: {
        "Content-Type": "application/json"
      }
    });

    response = await response.json();
    console.log("Got response!", response);
    $("#gps_latitude").text(response[0].GPS_Latitude);
    $("#gps_longitude").text(response[0].GPS_Longitude);
    $("#gps_adtitude").text(response[0].GPS_Adtitude + "m");
    $("#air_temperature").text(response[0].Air_Temperature + "°C");
    $("#air_humidity").text(response[0].Air_Humidity + "%");
    $("#presure").text(response[0].Presure + "hPa");
    $("#mq7").text(response[0].MQ7);
    $("#mq7_max").text(response[0].MQ7_max);
    $("#pm1_0").text(response[0].PM1_0 + "ug/m3");
    $("#pm2_5").text(response[0].PM2_5 + "ug/m3");
    $("#pm10").text(response[0].PM10 + "ug/m3");
    makeChart(response[1], response[2]); // response 1 is data for chart, response 2 is amount of data
  } catch (err) {
    console.error(`Error: ${err}`);
    $("#gps_latitude").text("err");
    $("#gps_longitude").text("err");
    $("#gps_adtitude").text("err");
    $("#air_temperature").text("err");
    $("#air_humidity").text("err");
    $("#presure").text("err");
    $("#mq7").text("err");
    $("#mq7_max").text("err");
    $("#pm1_0").text("err");
    $("#pm2_5").text("err");
    $("#pm10").text("err");
  }
}
var d, h, m, s, animate;

function init() {
  d = new Date();
  h = d.getHours();
  m = d.getMinutes();
  s = d.getSeconds();
  clock();
}

function clock() {
  s++;
  if (s == 60) {
    s = 0;
    m++;
    if (m == 60) {
      m = 0;
      h++;
      if (h == 24) {
        h = 0;
      }
    }
  }
  $("sec", s);
  $("min", m);
  $("hr", h);
  animate = setTimeout(clock, 1000);
}

function $(id, val) {
  if (val < 10) {
    val = "0" + val;
  }
  document.getElementById(id).innerHTML = val;
}

function makeChart(chartData, count) {
  var ctx = document.getElementById("myChart").getContext("2d");
  var i;
  let dataset1 = [];
  let dataset2 = [];
  let dataset10 = [];
  let labels = [];
  for (i = 0; i < count; i++) {
    let date = new Date(chartData[i].Date);
    dataset1[i] = { x: date.toISOString(), y: chartData[i].PM1_0};
    dataset2[i] = { x: date.toISOString(), y: chartData[i].PM2_5};
    dataset10[i] = { x: date.toISOString(), y: chartData[i].PM10};
    labels.push(date.toISOString());
  }

  

  var chart = new Chart(ctx, {
    // The type of chart we want to create
    type: "line",
    // The data for our dataset
    data: {
      labels: labels,
      datasets: [
        {
          label: "PM 1.0",
          data: dataset1,
          backgroundColor: ["rgba(23, 96, 180, 0.5)"],
          borderColor: ["rgba(23, 96, 180, 0.5)"],
          borderWidth: 2,
          fill: false
        },
        {
          label: "PM 2.5",
          data: dataset2,
          backgroundColor: ["rgba(106, 33, 190, 0.5)"],
          borderColor: ["rgba(106, 33, 190, 0.5)"],
          borderWidth: 2,
          fill: false
        },
        {
          label: "PM 10",
          data: dataset10,
          backgroundColor: ["rgba(200, 43, 116, 0.5)"],
          borderColor: ["rgba(200, 43, 116, 0.5)"],
          borderWidth: 2,
          fill: false
        }
      ]
    },

    options: {
      scales: {
        xAxes: [
          {
            display: true,
            time: {
              unit: "day",
              displayFormats: {
                day: "DD.MM.YYYY HH:mm"
              }
            },

            type: "time",

            distribution: "series"
          }
        ],
        yAxes: [
          {
            display: true,
            ticks: {
              beginAtZero: true,
              steps: 10,
              stepValue: 5,
              max: 100,
              unit: "ug/m3"
            }
          }
        ]
      },
      animation: {
        duration: 0 // general animation time
      },
      hover: {
        animationDuration: 0 // duration of animations when hovering an item
      },
      responsiveAnimationDuration: 0 // animation duration after a resize
    }
  });
}
window.onload = init;
