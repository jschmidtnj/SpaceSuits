import express from "express"
import cors from "cors"
import minimist from "minimist"
import bodyParser from 'body-parser'
import config from "./config"

const app = express()

app.use(bodyParser.json())

app.use(cors())

let port = config.port

const args = minimist(process.argv.slice(2), {
  string: ["port"]
})

if (args.port) {
  port = args.port
}

let taskdata = []
let taskdataobject = {}

app.get('/gettaskdataobject', (req, res) => {
  res.send(taskdataobject)
})

app.get('/gettaskdata', (req, res) => {
  res.send(taskdata)
})

app.put('/settaskdata', (req, res) => {
  const newtaskdata = []
  taskdataobject = req.body
  for (const majorkey in taskdataobject) {
    for (const subkey in taskdataobject[majorkey]) {
      const newtask = taskdataobject[majorkey][subkey]
      newtask.majorkey = majorkey
      newtask.subkey = subkey
      newtaskdata.push(newtask)
    }
  }
  taskdata = newtaskdata
  res.json({
    message: 'set task data'
  })
})

app.listen(port, () => {
  console.log(`listening on port ${port} for requests 🚀`)
})
