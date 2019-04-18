import express from "express"
import cors from "cors"
import minimist from "minimist"
import bodyParser from 'body-parser'
import mongoose from 'mongoose'
import exitHook from 'exit-hook'
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

const createtaskdataobject = (taskdataarray) => {
  let newitems = []
  for (const majorkey in taskdataarray) {
    for (const subkey in taskdataarray[majorkey]) {
      const singletaskdata = taskdataarray[majorkey][subkey]
      const tasktimeutc = parseInt(singletaskdata.time)
      const tasktimeutcdate = new Date(tasktimeutc)
      const singletasktime = tasktimeutcdate.toString()
      const singletaskdatavalue = singletaskdata.data
      const tasknum = majorkey + '.' + subkey
      newitems.push({
        id: tasknum,
        data: singletaskdatavalue,
        time: singletasktime
      })
    }
  }
  return newitems
}

const createtaskdata = (taskdataobj) => {
  const newtaskdata = []
  for (const majorkey in taskdataobj) {
    for (const subkey in taskdataobj[majorkey]) {
      const newtask = taskdataobj[majorkey][subkey]
      newtask.majorkey = majorkey
      newtask.subkey = subkey
      newtaskdata.push(newtask)
    }
  }
  return newtaskdata
}

mongoose.connect(config.mongouri, {
  useNewUrlParser: true
})

const db = mongoose.connection
db.on('error', err => {
  console.error(`mongo connection error: ${err}`)
})

db.once('open', () => {
  console.log('connected to mongo 🚀')
  db.collection('tasks').find().toArray().then(res => {
    taskdata = res
    taskdataobject = createtaskdataobject(taskdata)
  }).catch(err => {
    console.log(`got error: ${err}`)
  })
})

const updateDatabase = () => {
  db.collection('tasks').deleteMany().then(() => {
    console.log('database tasks deleted')
  }).catch(err => {
    console.log(`got error deleting tasks in db: ${err}`)
  })
  db.collection('tasks').insertMany(taskdata).then(() => {
    console.log('database tasks updated')
  }).catch(err => {
    console.log(`got error updating tasks in db: ${err}`)
  })
}

app.get('/gettaskdataobject', (req, res) => {
  res.send(taskdataobject)
})

app.get('/gettaskdata', (req, res) => {
  res.send(taskdata)
})

app.put('/settaskdataobject', (req, res) => {
  taskdata = req.body
  taskdata = createtaskdata(req.body)
  updateDatabase()
  res.json({
    message: 'set task data'
  })
})

app.listen(port, () => {
  console.log(`listening on port ${port} for requests 🚀`)
})

exitHook(() => {
  console.log('exiting')
  db.close()
})
