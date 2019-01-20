<template>
  <div class="data">
    <h1>{{ msg }}</h1>
    <p>{{ data }}</p>
  </div>
</template>

<script>
import axios from 'axios';

export default {
  name: 'Data',
  data() {
    return {
      msg: 'data',
      data: '',
      url: 'https://dog.ceo/api/breeds/list/all',
      interval: undefined,
    };
  },
  mounted() {
    // url for testing: https://dog.ceo/api/breeds/list/all
    // response.data.message;
    // http://192.168.1.1/getdata
    this.interval = setInterval(() => {
      console.log(this.url);
      axios.get(this.url).then(response => {
        const message = response.data;
        const messageStr = JSON.stringify(message);
        console.log(messageStr);
        this.data = messageStr;
      });
    }, 1000);
  },
};
</script>

<style>
</style>