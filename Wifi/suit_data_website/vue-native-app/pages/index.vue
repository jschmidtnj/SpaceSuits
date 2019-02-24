<template>
  <section class="container">
    <div>
      <br>
      <br>
      <h5 class="text-center">
        Suit Metrics
      </h5>
      <br>
      <suit-data />
      <br>
      <br>
      <h5 class="text-center">
        Tasks
      </h5>
      <br>
      <tasks />
    </div>
  </section>
</template>

<script lang="ts">
import Vue from 'vue'
import Tasks from '../components/Tasks.vue'
import SuitData from '../components/SuitData.vue'

export default Vue.extend({
  name: 'Dashboard',
  components: {
    Tasks,
    SuitData
  },
  data() {
    return {}
  }
})
</script>
