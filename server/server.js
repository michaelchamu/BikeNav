const express = require("express");
const {Client} = require("@googlemaps/google-maps-services-js");
const {_api_key} = require('./keys.js');

const client = new Client({config:{key: _api_key}});
var app = express();
app.use(express.urlencoded({extended: true})); 
app.use(express.json());

app.get("/", (req, res, next) => {
    res.json(["Tony","Lisa","Michael","Ginger","Food"]);
   });

app.post("/location-details", (req, res)=>{
//fetch longitude
let longitude = req.body.longitude;
//fetch latitude
let latitude = req.body.latitude;
let range = req.body.range;
//send to GOOGLE API
client.directions({
    params: {
        origin: [{ lat: latitude, lng: longitude }],
        destination:[{ lat: latitude, lng: longitude }],
        mode: "Walking",
      },
      timeout: 1000, // milliseconds
}).then((r)=>{
    console.log(r.data.results[0].directions);
}
).catch((e)=>{console.log(e.response.data.error_message);})
//return result
 res.send({status: "ok"});

});
app.listen(3000, () => {
 console.log("Server running on port 3000");
});

//get directions from GoogleMaps
let getDirections = (latitude, longitude, range)=>{

}