const express = require('express')
const servestatic = require('serve-static')

const app = express()

const port = 8080

const setHeaders = (res, path) => {
  res.setHeader('Access-Control-Allow-Origin', '*')
}

app.use(
  servestatic('dist', {
    setHeaders: setHeaders
  })
)

/* eslint-disable */
app.listen(port, () => console.log(`app listening on port ${port}`))
