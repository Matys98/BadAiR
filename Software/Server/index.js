var express = require("express");
var path = require("path");
var http = require("http");
var app = express();
var Sequelize = require("sequelize");
var hostname = 'localhost';
var port = process.env.PORT || 3000;
var bodyParser = require("body-parser");

var moment = require("moment");
moment().format();

const server = http.createServer(app); //create a server

app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());

const WebSocket = require("ws");
const wsServer = new WebSocket.Server({ server });

// connecting via sequelize, sqlite file is required
const sequelize = new Sequelize({
  dialect: "sqlite",
  storage: "database/baza.sqlite",
  define: {
    timestamps: false,
    id: false
  }
});

const Model = Sequelize.Model;
const Op = Sequelize.Op; // used in "and", "or" operations
class Drone_readings extends Model {}
Drone_readings.init(
  {
    // attributes
    ID: {
      type: Sequelize.INTEGER,
      allowNull: false
    },
    GPS_Latitude: {
      type: Sequelize.INTEGER,
      allowNull: false
    },
    GPS_Longitude: {
      type: Sequelize.INTEGER,
      allowNull: false
    },
    GPS_Adtitude: {
      type: Sequelize.INTEGER,
      allowNull: false
    },
    Air_Temperature: {
      type: Sequelize.INTEGER,
      allowNull: false
    },
    Air_Humidity: {
      type: Sequelize.INTEGER,
      allowNull: false
    },
    Presure: {
      type: Sequelize.INTEGER,
      allowNull: false
    },
    MQ7: {
      type: Sequelize.INTEGER,
      allowNull: false
    },
    MQ7_max: {
      type: Sequelize.INTEGER,
      allowNull: false
    },
    PM1_0: {
      type: Sequelize.INTEGER,
      allowNull: false
    },
    PM2_5: {
      type: Sequelize.INTEGER,
      allowNull: false
    },
    PM10: {
      type: Sequelize.INTEGER,
      allowNull: false
    },
    Date: {
      type: Sequelize.INTEGER,
      allowNull: false
    }
  },
  {
    sequelize // inherits from sequelize class
    //modelName: 'Drone_readings'
    // options
  }
);

//Getting css file
//app.use('/views/css',express.static(__dirname + '/views/css'));
app.use(express.static(path.join(__dirname, "views")));

// used to parse request body
app.use(express.json());

//Getting html file
app.get("/", function(req, resp) {
  resp.sendFile("index.html", { root: path.join(__dirname, "./views") });
});

app.get("/readings", async function(req, resp) {
  try {
    let data = await Drone_readings.findOne({
      raw: true,
      order: [["ID", "DESC"]]
    });
    let datachart = await Drone_readings.findAll({
      raw: true,
      order: [["ID"]]
    }); // datachart is an array of arrays, we can use datachart[] to show them in console

    var resptable = [data, datachart, data.ID];
    resp.send(JSON.stringify(resptable));
  } catch (err) {
    resp.sendStatus(500);
  }
});

//make defined actions after a POST on /control website
app.post("/control", (req, res) => {
  let action = req.body.action; // let is sth like var
  var datetime = new Date(); // getting date
  console.log(datetime.toISOString()); // inner function is converting date to string
  console.log(action);

  Drone_readings.findAll({ raw: true }).then(Drone_readings => {
    //console.log(Drone_readings);
  });

  res.send("1");
});

//bootstrap usage?
//app.use('/views/css', express.static(__dirname + '/node_modules/bootstrap/dist/css'));
require("dns").lookup(require("os").hostname(), function(err, localhost, fam) {
  console.log("addr: " + localhost);
  console.log("port: " + port);
});

wsServer.on("connection", function(ws, req) {
  /******* when server receives messsage from client trigger function with argument message *****/
  ws.on("message", function(message) {
    //var message = "<lat:12345;lon:12345;adt:12;tem:26;hum:20;pre:1035;mq:480;mq_max:600;pm1_0:20;pm2_5:30;pm10:40;>";
    console.log("Received: " + message);
       

    var n = message.length;
    var i = 0;
    var readingsArray = []; // array of read values
    var colonPosition, semicolonPosition, temp, temp2;
    var newMessage = message;

    for (i = 0; i < 11; i++) { //newMessage.count(";")
      colonPosition = newMessage.indexOf(":");
      semicolonPosition = newMessage.indexOf(";");
      temp = newMessage.substring(colonPosition + 1, semicolonPosition);
      temp2 = temp.valueOf();
      readingsArray.push(temp2);
      newMessage = newMessage.slice(semicolonPosition + 1, n);
    }
    let data = Drone_readings.findOne({
      raw: true,
      order: [["ID", "DESC"]]
    });

    if (
      message[0] == "<" &&
      message[message.length - 1] == ">" &&
      message != "<Update>" &&
      message != "<Clear>"
    ) {
      Drone_readings.build({
        ID: data.ID + 1,
        GPS_Latitude: readingsArray[0],
        GPS_Longitude: readingsArray[1],
        GPS_Adtitude: readingsArray[2],
        Air_Temperature: readingsArray[3],
        Air_Humidity: readingsArray[4],
        Presure: readingsArray[5],
        MQ7: readingsArray[6],
        MQ7_max: readingsArray[7],
        PM1_0: readingsArray[8],
        PM2_5: readingsArray[9],
        PM10: readingsArray[10],
        Date: new Date()
      })
        .save()
        .then(anotherTask => {
          Drone_readings.reload();
        })
        .catch(error => {
          console.log("blad wpisu do bazy");
          console.log(error);
        });
    }

    wsServer.clients.forEach(function(client) {
      //broadcast incoming message to all clients (s.clients)
      if (client != ws && client.readyState) {
        //except to the same client (ws) that sent this message
        client.send("broadcast: " + message);
      }
    });
    //console.log(message); ify

    // ws.send("From Server only to sender: "+ message); //send to client where message is from
  });
  ws.on("close", function() {
    console.log("lost one client");
  });
  //ws.send("new client connected");
  console.log("new client connected");
});

//Listening on port
app.listen(port, hostname, function() {
  console.log("Application worker " + process.pid + " started...");
});
server.listen(port);