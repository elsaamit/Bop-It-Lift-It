<template>
    <main>
        <p> Main page for gameplay </p>
        <button @click="startGame"> Start </button>
        <p> Latest message: {{ lastMessage }}</p>
    </main>
</template>

<script setup>
  import {
    lastMessage,
    connect,
    startGame
  } from '@/services/websocket'

  import {onMounted} from 'vue'

  onMounted(() => {
    connect()
  })
  
</script>
<script>
  export default {
    name: 'PlayView'
  }

</script>