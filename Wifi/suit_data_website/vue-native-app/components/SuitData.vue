<template>
  <b-container class="suit-data text-center">
    <form id="suitMetricsForm">
      <b-container>
        <b-row class="justify-content-center">
          <b-col col sm="3">
            <b-form-group>
              <label class="control-label" for="primaryo2">
                O
                <sub>2</sub> (primary)
              </label>
              <b-form-input
                id="primaryo2"
                type="text"
                name="primaryo2"
                class="text-center"
                :value="suitdata.primaryo2"
                disabled
              />psi
            </b-form-group>
          </b-col>
          <b-col col sm="3">
            <b-form-group>
              <label class="control-label" for="secondaryo2">
                O
                <sub>2</sub> (secondary)
              </label>
              <b-form-input
                id="secondaryo2"
                type="text"
                name="secondaryo2"
                class="text-center"
                :value="suitdata.secondaryo2"
                disabled
              />psi
            </b-form-group>
          </b-col>
          <b-col col sm="3">
            <b-form-group>
              <label class="control-label" for="battery">Battery</label>
              <b-form-input
                id="battery"
                type="text"
                name="battery"
                class="text-center"
                :value="suitdata.battery"
                disabled
              />%
            </b-form-group>
          </b-col>
          <b-col col sm="3">
            <b-form-group>
              <label class="control-label" for="heartrate">Heart Rate</label>
              <b-form-input
                id="heartrate"
                type="text"
                name="heartrate"
                class="text-center"
                :value="suitdata.heartrate"
                disabled
              />bpm
            </b-form-group>
          </b-col>
          <b-col col sm="3">
            <b-form-group>
              <label class="control-label" for="moisture">Moisture</label>
              <b-form-input
                id="moisture"
                type="text"
                name="moisture"
                class="text-center"
                :value="suitdata.moisture"
                disabled
              />%
            </b-form-group>
          </b-col>
        </b-row>
      </b-container>
    </form>
    <hr>
    <form id="suitPositioningForm">
      <b-container>
        <b-row class="justify-content-center">
          <b-col col sm="3">
            <b-form-group>
              <label class="control-label" for="accelx">Accel X</label>
              <b-form-input
                id="accelx"
                type="text"
                name="accelx"
                class="text-center"
                :value="suitdata.inertialdata.accelx"
                disabled
              />m/s
              <sup>2</sup>
            </b-form-group>
          </b-col>
          <b-col col sm="3">
            <b-form-group>
              <label class="control-label" for="accely">Accel Y</label>
              <b-form-input
                id="accely"
                type="text"
                name="accely"
                class="text-center"
                :value="suitdata.inertialdata.accely"
                disabled
              />m/s
              <sup>2</sup>
            </b-form-group>
          </b-col>
          <b-col col sm="3">
            <b-form-group>
              <label class="control-label" for="accelz">Accel Z</label>
              <b-form-input
                id="accelz"
                type="text"
                name="accelz"
                class="text-center"
                :value="suitdata.inertialdata.accelz"
                disabled
              />m/s
              <sup>2</sup>
            </b-form-group>
          </b-col>
          <b-col col sm="3">
            <b-form-group>
              <label class="control-label" for="roll">Roll</label>
              <b-form-input
                id="roll"
                type="text"
                name="roll"
                class="text-center"
                :value="suitdata.inertialdata.roll"
                disabled
              />&deg;
            </b-form-group>
          </b-col>
          <b-col col sm="3">
            <b-form-group>
              <label class="control-label" for="pitch">Pitch</label>
              <b-form-input
                id="pitch"
                type="text"
                name="pitch"
                class="text-center"
                :value="suitdata.inertialdata.pitch"
                disabled
              />&deg;
            </b-form-group>
          </b-col>
          <b-col col sm="3">
            <b-form-group>
              <label class="control-label" for="yaw">Yaw</label>
              <b-form-input
                id="yaw"
                type="text"
                name="yaw"
                class="text-center"
                :value="suitdata.inertialdata.yaw"
                disabled
              />&deg;
            </b-form-group>
          </b-col>
        </b-row>
      </b-container>
    </form>
    <hr>
  </b-container>
</template>

<script lang="ts">
import Vue from 'vue'
import axios from 'axios'
import config from '../assets/config'
export default Vue.extend({
  name: 'SuitData',
  data() {
    return {
      suitdata: {
        primaryo2: 100,
        secondaryo2: 100,
        battery: 100,
        heartrate: 70,
        moisture: 20,
        inertialdata: {
          accelx: 0,
          accely: 0,
          accelz: 0,
          roll: 0,
          pitch: 0,
          yaw: 0
        }
      }
    }
  },
  created() {
    const getData = () => {
      axios
        .get(config.getsuitdataurl)
        .then(res => {
          this.suitdata = res.data
        })
        .catch(err => {
          this.$store.commit('notifications/addNotification', {
            code: config.errorcode,
            message: err.toString()
          })
          // console.log(err)
        })
    }
    getData()
    setInterval(getData, config.datapoll)
  }
})
</script>

<style scoped>
</style>
