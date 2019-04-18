import express from 'express'
import servestatic from 'serve-static'
const concurrently = require('concurrently')

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
app.listen(port, () => console.log(`app listening on port ${port} 🚀`))

concurrently([
  `node proxy/index.js`
], {
    prefix: 'name',
    restartTries: 1
  }).then(() => {
    console.log(`proxy started 🚀`)
  }).catch(err => {
    console.log(`failed to start proxy ${err}`)
  })
