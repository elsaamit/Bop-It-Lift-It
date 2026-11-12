<template>
  <div class="app-shell">
    <router-view/>
    <AppFooter/>
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
  socket = new WebSocket('ws://192.168.4.1:8081')

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

<style>
@import url('https://fonts.googleapis.com/css2?family=Barlow+Condensed:wght@600;700;800;900&family=Inter:wght@400;500;600;700;800&display=swap');

:root {
  --bg: #111318;
  --surface: #1d2129;
  --accent: #c6ff4d;
  --orange: #ff7138;
  --text: #f7f7f2;
  --muted: #9298a3;
}

body {
  margin: 0;
  background: var(--bg);
  color: var(--text);
  font-family: 'Inter', sans-serif;
}

h1, h2, h3 {
  font-family: 'Barlow Condensed', sans-serif;
  text-transform: uppercase;
  font-weight: 800;
  letter-spacing: 1px;
}

button {
  background: var(--accent);
  color: #111318;
  border: none;
  border-radius: 12px;
  padding: 14px 28px;
  font-size: 16px;
  font-weight: 800;
  text-transform: uppercase;
  cursor: pointer;
  transition: transform 0.15s ease;
}

button:hover {
  transform: translateY(-2px);
}

.game-content {
  background: var(--surface);
  border: 1px solid #343a45;
  border-radius: 20px;
  padding: 28px;
}

.score {
  color: var(--accent);
  font-size: 64px;
  font-weight: 900;
  font-family: 'Barlow Condensed', sans-serif;
}

.muted {
  color: var(--muted);
}

.app-shell {
  min-height: 100vh;
  display: flex;
  flex-direction: column;
}

.app-footer {
  margin-top: auto;
}

</style>
