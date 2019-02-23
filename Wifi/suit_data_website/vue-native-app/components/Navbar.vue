<template>
  <div>
    <b-navbar toggleable="lg" type="dark" variant="info">
      <b-navbar-brand href="/">
        Space Suit
      </b-navbar-brand>
      <b-navbar-toggle target="nav_collapse" />
      <b-collapse id="nav_collapse" is-nav>
        <b-navbar-nav>
          <b-nav-item href="/tasks">
            Tasks
          </b-nav-item>
          <b-nav-item href="/about">
            About
          </b-nav-item>
        </b-navbar-nav>
      </b-collapse>
    </b-navbar>
  </div>
</template>

<script lang="ts">
import Vue from 'vue'
export default Vue.extend({
  name: 'Navbar',
  data() {
    return {}
  }
})
</script>

<style scoped>
</style>
