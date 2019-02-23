<template>
  <section class="container">
    <div>
      <br>
      <br>
      <br>
      <task-table />
    </div>
  </section>
</template>

<script lang="ts">
import Vue from 'vue'
import TaskTable from '../components/TaskTable.vue'

export default Vue.extend({
  name: 'Tasks',
  data() {
    return {}
  },
  components: {
    TaskTable
  }
})
</script>
