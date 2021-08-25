const express = require("express");

var app = express();
app.use(express.urlencoded({extended: true})); 
app.use(express.json());
app.get("/", (req, res, next) => {
    res.json(["Tony","Lisa","Michael","Ginger","Food"]);
   });

app.post('/update', (req, res, next)=>{
//fetch longitude

//fetch latitude
//fetch range
//send to GOOGLE API
//return result
});
app.listen(3000, () => {
 console.log("Server running on port 3000");
});