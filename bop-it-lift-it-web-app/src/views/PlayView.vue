<template>
    <main>
        //haven't started yet
        <div v-if="gameState === 'idle'">
          <button @click="startGame"> Start </button>
        </div>

        //playing
        <div v-if="gameState=== 'playing'">
          <h2 v-if="currentPrompt"> {{  currentPrompt.text }} </h2>
        </div>

        <div v-if="gameState=== 'gameOver'">
          <h2>Game Over</h2>
          <h2> Final Score: {{ finalScore }}</h2>
        </div>
    </main>
</template>

<script setup>

  import {
    gameState,
    score,
    command,
    finalScore
  } from '@/services/control'

  import {
    connect,
    startGame,
    stopGame
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