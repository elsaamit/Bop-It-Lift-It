<template>
    <main class="game-page">
      <header class="game-header"> 
        <h1>LIFT IT! </h1>
      </header>

        <section v-if="gameState === 'idle'" class="game-content">
          <button @click="startGame" class="play-button"> START </button>
        </section>

        
        <section v-else-if="gameState === 'playing'" class="game-content">
          <div class="score-content">
            <span class="score-label">SCORE</span>
            <span class="score-number">{{ score }}</span>
          </div>

          <div class="exercise-content">
            <template v-if="exercise">
              <h2> {{ exercise.text }}</h2>
              <img v-if="exercise.image" :src="exercise.image" class="exercise-image"/>
            </template>

            <p v-else>Waiting for next exercise</p>

          <div v-if="lastUserResult" class="result-message">
            <p v-if="lastUserResult.success" class="success">NICE!</p>
            <p v-else class="fail">TRY AGAIN!</p>
          </div>
        </div>

        <button @click="stopGame" class="play-button stop-button">STOP</button>
      </section>

      <section v-else-if="gameState === 'game_over'">
        <div class="game-over-content">
          <h2>GAME OVER</h2>
          <div class="final-score"> {{ finalScore }}</div>
          <span class="score-label">FINAL SCORE</span>
          <button @click="startGame" class="play-button">PLAY AGAIN</button>
        </div>
      </section>

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
.game-page {
  min-height: 0;
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 20px 24px 30px;
  text-align: center;
  color: #f7f7f2;
}

.game-header h1 {
  font-size: 48px;
  margin: 0 0 20px;
  color: #c6ff4d;
  letter-spacing: 3px;
}

.game-content {
  width: 100%;
  max-width: 650px;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 20px;
}

.game-content h2 {
  color: #f7f7f2;
  font-size: 30px;
  margin: 0;
}

.score-content {
  display: flex;
  flex-direction: column;
  align-items: center;
  background: #202b38;
  border: 1px solid #354354;
  border-radius: 16px;
  padding: 12px 45px;
}

.score-label {
  color: #aab6c5;
  font-size: 13px;
  font-weight: 700;
  letter-spacing: 2px;
}

.score-number {
  color: #c6ff4d;
  font-size: 48px;
  font-weight: 900;
}

.exercise-content {
  width: 100%;
  background: #202b38;
  border: 1px solid #354354;
  border-radius: 20px;
  padding: 24px;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 15px;
}

.exercise-content h2 {
  color: #c6ff4d;
  font-size: 32px;
  letter-spacing: 1px;
}

.exercise-image {
  width: 100%;
  max-width: 300px;
  max-height: 300px;
  object-fit: contain;
  border-radius: 12px;
  background: white;
}

.result-message p {
  font-size: 20px;
  font-weight: 800;
  margin: 5px 0;
}

.success {
  color: #c6ff4d;
}

.fail {
  color: #ff7138;
}

.play-button {
  background: #c6ff4d;
  color: #111820;
  border: none;
  border-radius: 12px;
  padding: 15px 45px;
  font-size: 18px;
  font-weight: 900;
  letter-spacing: 1px;
  cursor: pointer;
  transition: transform 0.2s, background 0.2s;
}

.play-button:hover {
  background: #d8ff80;
  transform: translateY(-3px);
}

.stop-button {
  background: #ff7138;
  color: white;
}

.stop-button:hover {
  background: #ff895c;
}

.game-over-content {
  background: #202b38;
  border: 1px solid #354354;
  border-radius: 20px;
  padding: 40px;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 15px;
  width: 100%;
}

.game-over-content p {
  color: #aab6c5;
}

.final-score {
  color: #c6ff4d;
  font-size: 72px;
  font-weight: 900;
}

@media (max-width: 600px) {
  .game-header h1 {
    font-size: 36px;
  }

  .exercise-content {
    padding: 16px;
  }

  .exercise-image {
    max-height: 230px;
  }

  .game-content h2 {
    font-size: 24px;
  }
}
</style>
