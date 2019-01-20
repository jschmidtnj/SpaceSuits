import Vue from 'vue';
import Router from 'vue-router';
import Data from '@/components/Data';

Vue.use(Router);

export default new Router({
  routes: [
    {
      path: '/',
      name: 'Data',
      component: Data,
    },
  ],
  mode: 'history',
});
