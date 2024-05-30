// Use "Runtime : Node.js 20" and "Entry point : myExpressFunction"

// import express, mysql and cors
// framework of node.js which is a backend language
const functions = require('@google-cloud/functions-framework');
const express = require('express');
const mysql = require('mysql2');
const cors = require('cors');

// express initialized
const app = express();
app.use(cors());

// mysql connection initialized
const db = mysql.createConnection({
  host: '34.101.221.13',
  port: 3306,
  user: 'root',
  password: 'Ta0206sukses!',
  database: 'ta232402006'
});

/**Create API*/
app.get('/', (re, res) => {
  return res.json('From Backend side');
})

// mysql api
app.get('/stat', (req, res) => {
  const statement = 'SELECT * FROM stat_perangkat';
  db.query(statement, (err, data) => {
    if (err) return res.json(err);
    return res.json(data);
  })
})

app.get('/daily', (req, res) => {
  const statement =
      'SELECT * FROM konsumsi_harian ORDER BY waktu_diterima DESC LIMIT 6';
  db.query(statement, (err, data) => {
    if (err) return res.json(err);
    return res.json(data);
  })
})

app.get('/month/:MM/:YY', (req, res) => {
  const {MM, YY} = req.params;

  const statement = `
  SELECT kb.*, kk.cumul_consump
  FROM konsumsi_bulanan kb
  JOIN konsumsi_kumulatif kk ON kb.perangkat_id = kk.perangkat_id
  WHERE kb.MM = ? AND kb.YY = ?
  `;

  const values = [MM, YY];
  db.query(statement, values, (err, data) => {
    if (err) return res.json(err);
    return res.json(data);
  })
})

app.get('/force/:k', (req, res) => {
  const k = req.params.k;


  // SQL query to select data from force_data table
  const statement = `
    SELECT *
    FROM force_data
    WHERE perangkat_id = ?
  `;

  const values = [k];
  
  db.query(statement, values, (err, data) => {
    if (err) return res.json(err);
    return res.json(data);
  });
});

// Register the Express app as an HTTP function
functions.http('myExpressFunction', app);
