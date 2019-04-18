<template>
  <footer>
    <div class="container">
      <p>&copy; Joshua Schmidt 2019. All Rights Reserved.</p>
      <ul class="list-inline">
        <li class="list-inline-item">
          <a id="privacypolicylink" target="_blank" :href="privacylink">Privacy</a>
        </li>
        <li class="list-inline-item">
          <a id="toslink" target="_blank" :href="toslink">Terms</a>
        </li>
        <li class="list-inline-item">
          <a id="helplink" target="_blank" :href="helplink">Help</a>
        </li>
      </ul>
    </div>
  </footer>
</template>

<script lang="ts">
import Vue from 'vue'
import config from '../assets/config'
export default Vue.extend({
  name: 'Footer',
  data() {
    return {
      privacylink: config.privacypolicyurl,
      toslink: config.termsofserviceurl,
      helplink: config.helpurl
    }
  }
})
</script>

<style>
footer {
  position: absolute;
  right: 0;
  bottom: 0;
  left: 0;
  padding: 1rem;
  text-align: center;
  color: rgba(255, 255, 255, 0.3);
  background-color: #222222;
}

footer p {
  font-size: 12px;
  margin: 0;
}

footer ul {
  margin-bottom: 0;
}

footer ul li a {
  font-size: 12px;
  color: rgba(255, 255, 255, 0.3);
}

footer ul li a:hover,
footer ul li a:focus,
footer ul li a:active,
footer ul li a.active {
  text-decoration: none;
}

footer a {
  -webkit-transition: all 0.35s;
  -moz-transition: all 0.35s;
  transition: all 0.35s;
}

footer a:hover,
footer a:focus {
  color: #fcbd20;
}
</style>
