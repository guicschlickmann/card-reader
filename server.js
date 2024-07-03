const express = require('express');
const bodyParser = require('body-parser');
const mysql = require('mysql');
const cors = require('cors');
const path = require('path');

const app = express();
const port = 3000;

app.use(bodyParser.json());
app.use(cors());

// Servir arquivos estáticos da pasta 'public'
app.use(express.static(path.join(__dirname, 'public')));

const db = mysql.createConnection({
  host: 'localhost',
  user: 'root',
  password: '1234',
  database: 'rfid_tags'
});

db.connect((err) => {
  if (err) throw err;
  console.log('Connected to database');
});

app.post('/api/rfid', (req, res) => {
  const tag = req.body.tag;
  const sql = 'INSERT INTO rfid_tags (tag) VALUES (?)';
  db.query(sql, [tag], (err, result) => {
    if (err) {
      res.status(500).send(err);
    } else {
      res.status(200).send('Tag saved to database');
    }
  });
});

app.get('/api/rfid-daily', (req, res) => {
  const sql = 'SELECT tag, COUNT(*) as count, DATE_FORMAT(timestamp, "%Y-%m-%d") as date FROM rfid_tags WHERE DATE(timestamp) = CURDATE() GROUP BY tag, date';
  db.query(sql, (err, results) => {
    if (err) {
      res.status(500).send(err);
    } else {
      res.json(results);
    }
  });
});

app.get('/api/rfid-all', (req, res) => {
  const sql = 'SELECT tag, COUNT(*) as count FROM rfid_tags GROUP BY tag';
  db.query(sql, (err, results) => {
    if (err) {
      res.status(500).send(err);
    } else {
      res.json(results);
    }
  });
});

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

app.listen(port, () => {
  console.log(`Server running on http://localhost:${port}`);
});
