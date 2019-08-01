const express = require('express')
const app = express();
// var index = require('./index.htm')

app.get('/', (req, res) => {
  res.sendFile('/Users/cedriclor/Documents/PlatformIO/Projects/Laser Controller/data/index.htm')
});

app.get('/index.css', (req, res) => {
  res.sendFile('/Users/cedriclor/Documents/PlatformIO/Projects/Laser Controller/data/index.css')
});

app.get('/index.js', (req, res) => {
  res.sendFile('/Users/cedriclor/Documents/PlatformIO/Projects/Laser Controller/data/index.js')
});

app.listen(8000, () => {
  console.log('Example app listening on port 8000!')
});
