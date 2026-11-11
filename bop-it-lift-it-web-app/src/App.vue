<template>
  <div>
    <router-view/>
    <AppFooter class="footer"/>
  </div>
</template>


<script setup>

import { ref, onMounted, onUnmounted } from 'vue'
import AppFooter from './components/AppFooter.vue'

const status = ref('Disconnected')
const messages = ref([])
let socket = null

onMounted(() => {
  // test server
  socket = new WebSocket('ws://localhost:8081')

  socket.addEventListener('open', () => {
    status.value = 'Connected'
  })

  socket.addEventListener('message', (event) => {
    messages.value.push(event.data)
    console.log('Received:', event.data)
  })

  socket.addEventListener('error', (event) => {
    console.error('WebSocket error:', event)
  })

  socket.addEventListener('close', () => {
    status.value = 'Disconnected'
  })

})

function sendMessage(message) {
  if (socket?.readyState === WebSocket.OPEN) {
    socket.send(JSON.stringify(message))
  } else {
    console.warn('WebSocket is not connected')
  }
}

onUnmounted(() => {
  socket?.close()
})
</script>

