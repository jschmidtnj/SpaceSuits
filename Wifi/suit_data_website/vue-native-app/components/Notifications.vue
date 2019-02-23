<template>
  <div>
    <b-alert
      :show="dismissCountDown"
      dismissible
      :variant="alertvariant"
      @dismissed="dismissCountDown=0"
      @dismiss-count-down="countDownChanged"
    >
      <p>{{ message }}</p>
      <p>This alert will dismiss after {{ dismissCountDown }} seconds...</p>
      <b-progress
        :variant="alertvariant"
        :max="dismissSecs"
        :value="dismissCountDown"
        height="4px"
      />
    </b-alert>
  </div>
</template>

<script lang="ts">
import Vue from 'vue'
import { mapState } from 'vuex'
import config from '../assets/config'
export default Vue.extend({
  name: 'Notifications',
  data() {
    return {
      dismissSecs: 10,
      dismissCountDown: 0,
      alertvariant: 'warning',
      message: ''
    }
  },
  computed: {
    ...mapState(['notifications'])
  },
  mounted() {
    this.$store.subscribe((mutation, state) => {
      if (mutation.type === 'notifications/addNotification') {
        /* eslint-disable */
        const notification = this.notifications.notifications[
          this.notifications.notifications.length - 1
        ]
        /* eslint-enable */
        this.message = notification.message
        switch (notification.code) {
          case config.errorcode:
            this.alertvariant = 'danger'
            break
          case config.warningcode:
            this.alertvariant = 'warning'
            break
          case config.successcode:
            this.alertvariant = 'success'
            break
          default:
            this.alertvariant = 'warning'
            break
        }
        this.dismissCountDown = this.dismissSecs
      }
    })
  },
  methods: {
    countDownChanged(dismissCountDown) {
      this.dismissCountDown = dismissCountDown
    }
  }
})
</script>
