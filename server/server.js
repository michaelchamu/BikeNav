var express = require("express");
var app = express();
app.get("/", (req, res, next) => {
    res.json(["Tony","Lisa","Michael","Ginger","Food"]);
   });

app.post('/update', (req, res, next)=>{

});
app.listen(3000, () => {
 console.log("Server running on port 3000");
});