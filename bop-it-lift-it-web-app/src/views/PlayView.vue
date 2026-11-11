<template>
    <main>
      <h1> Lift It! </h1>
        <div v-if="gameState === 'idle'">
          <button @click="startGame" class="play-button"> Start </button>
        </div>

        
        <div v-else-if="gameState === 'playing'">
          <h2>Score: {{ score }} </h2>

          <div class="demo">
            <template v-if="exercise">
              <h2> {{ exercise.text }}</h2>
              <img v-if="exercise.image" :src="exercise.image"/>
            </template>

            <p v-else>Waiting for next exercise</p>
          </div>

          <div v-if="lastUserResult">
            <p v-if="lastUserResult.success">Good job!</p>
            <p v-else>Not quite!</p>
          </div>

          <button @click="stopGame" class="play-button"> Stop </button>
        </div>


        <div v-if="gameState === 'game_over'">
          <h2>Game Over!</h2>
          <h2> Final Score: {{ finalScore }}</h2>
          <button @click="startGame" class="play-button">Play Again</button>
        </div>

    </main>
</template>

<script setup>

  import {
    gameState,
    score,
    exercise,
    finalScore,
    lastUserResult,
  } from '@/services/control'

  import {
    connect,
    disconnect,
    startGame,
    stopGame,
  } from '@/services/websocket'

  import {onMounted, onUnmounted} from 'vue'

  onMounted(() => {
    connect()
  })

  onUnmounted(()=> {
    disconnect()
  })
  

</script>

<style>

.demo {
  margin-top:100px;
  font-weight: bold;
}

</style>