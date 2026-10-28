import { createApp } from 'vue'
import App from './App.vue'
import router from './router'

console.log('Routes', router.getRoutes());
console.log('matched', router.resolve('/'));
createApp(App)
.use(router)
.mount('#app')

