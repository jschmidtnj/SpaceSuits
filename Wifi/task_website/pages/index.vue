<template>
  <section class="container">
    <div>
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
import Tasks from '~/components/Tasks.vue'

export default Vue.extend({
  name: 'Dashboard',
  components: {
    Tasks
  },
  data() {
    return {}
  }
})
</script>
