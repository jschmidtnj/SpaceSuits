import express from 'express'
import cors from 'cors'
import minimist from 'minimist'
import bodyParser from 'body-parser'
import mongoose from 'mongoose'
import exitHook from 'exit-hook'
import servestatic from 'serve-static'
import config from './config'

/* eslint-disable */

const app = express()

const setHeaders = (res, path) => {
  res.setHeader('Access-Control-Allow-Origin', '*')
}

app.use(
  servestatic('dist', {
    setHeaders: setHeaders
  })
)

app.use(bodyParser.json())

app.use(cors())

let port = config.port

const args = minimist(process.argv.slice(2), {
  string: ['port']
})

if (args.port) {
  port = args.port
}

if (process.env.PORT) {
  port = process.env.PORT
}

let taskdata = []
let taskdataobject = {}

const createtaskdataobject = taskdataarray => {
  let newobject = {}
  taskdataarray.forEach(task => {
    const majorkey = task.majorkey
    const subkey = task.subkey
    if (!newobject[majorkey]) newobject[majorkey] = {}
    newobject[majorkey][subkey] = task
    delete newobject[majorkey][subkey].majorkey
    delete newobject[majorkey][subkey].subkey
  })
  return newobject
}

const createtaskdata = taskdataobj => {
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

mongoose.connect(
  config.mongouri,
  {
    useNewUrlParser: true
  }
)

const db = mongoose.connection
db.on('error', err => {
  console.error(`mongo connection error: ${err}`)
})

db.once('open', () => {
  console.log('connected to mongo 🚀')
  db.collection('tasks')
    .find()
    .toArray()
    .then(res => {
      taskdata = res
      taskdataobject = createtaskdataobject(taskdata)
    })
    .catch(err => {
      console.log(`got error: ${err}`)
    })
})

const updateDatabase = () => {
  db.collection('tasks')
    .deleteMany()
    .then(() => {
      console.log('database tasks deleted')
    })
    .catch(err => {
      console.log(`got error deleting tasks in db: ${err}`)
    })
  db.collection('tasks')
    .insertMany(taskdata)
    .then(() => {
      console.log('database tasks updated')
    })
    .catch(err => {
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
  taskdataobject = req.body
  taskdata = createtaskdata(taskdataobject)
  updateDatabase()
  res.json({
    message: 'set task data'
  })
})

app.listen(port, () => {
  console.log(`listening on port ${port} 🚀`)
})

exitHook(() => {
  console.log('exiting')
  db.close()
})
