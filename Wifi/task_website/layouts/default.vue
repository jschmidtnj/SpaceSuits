<template>
  <div>
    <notifications />
    <navbar />
    <nuxt />
    <footer-component />
  </div>
</template>

<script lang="ts">
import Vue from 'vue'
import Navbar from '../components/Navbar.vue'
import Notifications from '../components/Notifications.vue'
import FooterComponent from '../components/FooterComponent.vue'

export default Vue.extend({
  components: {
    Navbar,
    Notifications,
    FooterComponent
  }
})
</script>


<style lang="scss">
@import '../assets/styles/main';
html {
  height: 100%;
  box-sizing: border-box;
  font-family: 'Source Sans Pro', -apple-system, BlinkMacSystemFont, 'Segoe UI',
    Roboto, 'Helvetica Neue', Arial, sans-serif;
  font-size: 16px;
  word-spacing: 1px;
  -ms-text-size-adjust: 100%;
  -webkit-text-size-adjust: 100%;
  -moz-osx-font-smoothing: grayscale;
  -webkit-font-smoothing: antialiased;
}

*,
*:before,
*:after {
  box-sizing: inherit;
  margin: 0;
}

body {
  position: relative;
  margin: 0;
  padding-bottom: 6rem;
  min-height: 112%;
}
</style>
