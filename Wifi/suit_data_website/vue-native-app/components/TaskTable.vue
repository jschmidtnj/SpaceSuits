<template>
  <b-container class="bv-example-row">
    <b-row>
      <b-col>
        <b-container fluid>
          <!-- User Interface controls -->
          <b-row>
            <b-col md="6" class="my-1">
              <b-form-group label-cols-sm="3" label="Filter" class="mb-0">
                <b-input-group>
                  <b-form-input v-model="filter" placeholder="Type to Search"/>
                  <b-input-group-append>
                    <b-button :disabled="!filter" @click="filter = ''">Clear</b-button>
                  </b-input-group-append>
                </b-input-group>
              </b-form-group>
            </b-col>

            <b-col md="6" class="my-1">
              <b-form-group label-cols-sm="3" label="Per page" class="mb-0">
                <b-form-select v-model="perPage" :options="pageOptions"/>
              </b-form-group>
            </b-col>
          </b-row>

          <!-- Main table element -->
          <b-table
            show-empty
            stacked="md"
            :items="items"
            :fields="fields"
            :current-page="currentPage"
            :per-page="perPage"
            :filter="filter"
            :sort-by.sync="sortBy"
            :sort-desc.sync="sortDesc"
            :sort-direction="sortDirection"
            @filtered="onFiltered"
          ></b-table>

          <b-row>
            <b-col md="6" class="my-1">
              <b-pagination
                v-model="currentPage"
                :total-rows="totalRows"
                :per-page="perPage"
                class="my-0"
              />
            </b-col>
          </b-row>
        </b-container>
      </b-col>
    </b-row>
    <b-row>
      <b-col>
        <b-form @submit="onTaskSubmit" @reset="onTaskReset">
          <b-form-group
            id="taskinputgroup"
            label="Enter task:"
            label-for="taskinput"
            description="task must be valid."
          >
            <b-form-input
              id="taskinput"
              v-model="form.taskdata"
              type="text"
              required
              placeholder="task"
            />
          </b-form-group>
          <b-button type="submit" variant="primary">Submit</b-button>
        </b-form>
      </b-col>
    </b-row>
  </b-container>
</template>

<script lang="ts">
import Vue from 'vue'
import axios from 'axios'
import _ from 'lodash'
import config from '../assets/config'

export default Vue.extend({
  name: 'tasktable',
  data() {
    return {
      form: {
        taskdata: ''
      },
      items: [],
      fields: [
        {
          key: 'id',
          label: 'Id Number',
          sortable: true
        },
        { key: 'data', label: 'Task', sortable: true, class: 'text-center' },
        {
          key: 'time',
          label: 'Submission Time',
          sortable: true,
          class: 'text-center'
        }
      ],
      currentPage: 1,
      perPage: 5,
      totalRows: 0,
      pageOptions: [5, 10, 15],
      sortBy: null,
      sortDesc: false,
      sortDirection: 'asc',
      filter: null,
      taskdata: {}
    }
  },
  created() {
    const getTasks = () => {
      axios
        .get(config.gettaskdataurl)
        .then(resp => {
          if (!_.isEqual(this.taskdata, resp.data)) {
            this.taskdata = resp.data
            const newitems = []
            for (const majorkey in this.taskdata) {
              for (const subkey in this.taskdata[majorkey]) {
                const singletaskdata = this.taskdata[majorkey][subkey]
                const tasktimeutc = parseInt(singletaskdata.time)
                // console.log("time: " + tasktimeutc.toString());
                const tasktimeutcdate = new Date(tasktimeutc)
                const singletasktime = tasktimeutcdate.toString()
                // console.log("date: " + singletasktime);
                const singletaskdatavalue = singletaskdata.data
                // console.log(majorkey + " -> " + subkey + " -> " + singletaskdatavalue);
                const tasknum = majorkey + '.' + subkey
                newitems.push({
                  id: tasknum,
                  data: singletaskdatavalue,
                  time: singletasktime
                })
              }
            }
            this.items = newitems
            this.totalRows = this.items.length
          }
        })
        .catch(err => {
          this.$store.commit('notifications/addNotification', {
            code: config.errorcode,
            message: err.toString()
          })
          // console.log(err)
        })
    }
    getTasks()
    setInterval(getTasks, config.taskpoll)
  },
  methods: {
    onFiltered(filteredItems) {
      // Trigger pagination to update the number of buttons/pages due to filtering
      this.totalRows = filteredItems.length
      this.currentPage = 1
    },
    onTaskSubmit(evt) {
      evt.preventDefault()
      const formdata = this.form.taskdata
      const regexp = new RegExp(config.validtaskregex, 'i')
      if (!regexp.test(formdata)) {
        this.$store.commit('notifications/addNotification', {
          code: config.errorcode,
          message: 'invalid task input. must be in form 1.1 task message'
        })
        return
      }
      const tasksplit = formdata.split(' ')
      const tasknum = tasksplit[0]
      const tasknumsplit = tasknum.split('.')
      const majortasknum = tasknumsplit[0]
      const subtasknum = tasknumsplit[1]
      delete tasksplit[0]
      const taskdata = tasksplit.join(' ').trim()
      const tasktime = Date.now().toString()
      const newitem = {
        data: taskdata,
        time: tasktime
      }
      if (this.taskdata[majortasknum] === undefined)
        this.taskdata[majortasknum] = {}
      this.taskdata[majortasknum][subtasknum] = newitem
      // update table data
      const newitemtable = {
        data: taskdata,
        time: new Date(parseInt(newitem.time)).toString(),
        id: tasknum
      }
      let founditem: boolean = false
      for (let i = 0; i < this.items.length; i++) {
        if (this.items[i].id === tasknum) {
          this.items[i] = newitemtable
          founditem = true
          break
        }
      }
      if (!founditem) this.items.push(newitemtable)
      /* eslint-disable */
      console.log('starting request')
      console.log(JSON.stringify(this.taskdata))
      axios
        .put(config.settaskdataurl, JSON.stringify(this.taskdata))
        .then(resp => {
          // console.log(resp)
        })
        .catch(err => {
          this.$store.commit('notifications/addNotification', {
            code: config.errorcode,
            message: err.toString()
          })
          // console.log(err)
        })
    },
    onTaskReset(evt) {
      evt.preventDefault()
      this.form.taskdata = ''
    }
  }
})
</script>
