import express from 'express'
import minimist from 'minimist'
import axios from 'axios'
import bodyParser from 'body-parser'
import cors from 'cors'
import config from './config'

/* eslint-disable */

const app = express()

app.use(bodyParser.json())

app.use(cors())

let requestaddress = config.requestaddress
let requestport = config.requestport
let receiveport = config.receiveport

const args = minimist(process.argv.slice(2), {
  string: ["requestaddress", "requestport", "receiveport"]
})

if (
  args.requestaddress !== undefined &&
  args.requestport !== undefined &&
  args.receiveport !== undefined
) {
  requestaddress = args.requestaddress
  requestport = args.requestport
  receiveport = args.receiveport
}

const handleError = (error, res) => {
  if (error.response) {
    // The request was made and the server responded with a status code
    // that falls out of the range of 2xx
    if (config.debug)
      console.log(
        `data: ${error.response.data}, status: ${
        error.response.status
        }, headers: ${error.response.headers}`
      )
    res.status(error.response.status).send(error.response.data)
  } else if (error.request) {
    // The request was made but no response was received
    // `error.request` is an instance of XMLHttpRequest in the browser and an instance of
    // http.ClientRequest in node.js
    const message = `no response received from ${requestaddress}:${requestport}`
    if (config.debug) console.log(message)
    res.status(404).send(
      JSON.stringify({
        message: message
      })
    )
  } else {
    // Something happened in setting up the request that triggered an Error
    console.log(`Error: ${error.message}`)
    res.status(404).send(
      JSON.stringify({
        message: `no response received from ${requestaddress}:${requestport}`
      })
    )
  }
}

app.get(config.requestpath, (req, res) => {
  const fullurl = `${req.protocol}://${requestaddress}:${requestport}${
    req.originalUrl
    }`
  // if (config.debug) console.log(`get request ${fullurl}`)
  axios
    .get(fullurl, {
      headers: req.headers
    })
    .then(theresp => {
      const data = theresp.data
      // if (config.debug) console.log(data)
      res.send(data)
    })
    .catch(err => {
      handleError(err, res)
    })
    .then(() => {
      // if (config.debug) console.log(`finished get request`)
    })
})

app.post(config.requestpath, (req, res) => {
  const fullurl = `${req.protocol}://${requestaddress}:${requestport}${
    req.originalUrl
    }`
  if (config.debug) console.log(`put request ${fullurl}, data: ${JSON.stringify(req.body)}`)
  axios
    .post(fullurl, req.body, {
      headers: req.headers
    })
    .then(theresp => {
      const data = theresp.data
      if (config.debug) console.log(data)
      res.send(data)
    })
    .catch(err => {
      handleError(err, res)
    })
    .then(() => {
      if (config.debug) console.log(`finished post request`)
    })
})

app.put(config.requestpath, (req, res) => {
  const fullurl = `${req.protocol}://${requestaddress}:${requestport}${
    req.originalUrl
    }`
  if (config.debug) console.log(`put request ${fullurl}, data: ${JSON.stringify(req.body)}`)
  axios
    .put(fullurl, req.body, {
      headers: req.headers
    })
    .then(theresp => {
      const data = theresp.data
      if (config.debug) console.log(data)
      res.send(data)
    })
    .catch(err => {
      handleError(err, res)
    })
    .then(() => {
      if (config.debug) console.log(`finished put request`)
    })
})

app.delete(config.requestpath, (req, res) => {
  const fullurl = `${req.protocol}://${requestaddress}:${requestport}${
    req.originalUrl
    }`
  if (config.debug) console.log(`delete request ${fullurl}`)
  axios
    .delete(fullurl, {
      headers: req.headers
    })
    .then(theresp => {
      const data = theresp.data
      if (config.debug) console.log(data)
      res.send(data)
    })
    .catch(err => {
      handleError(err, res)
    })
    .then(() => {
      if (config.debug) console.log(`finished delete request`)
    })
})

app.listen(receiveport, () => {
  console.log(`listening on port ${receiveport} for requests 🚀`)
})