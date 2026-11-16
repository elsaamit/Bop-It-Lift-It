<template>
    <main class="instruction-page">
      <div class="instruction-content">
        <h1 class="instruction-header">HOW TO PLAY</h1>
        <div class="list-content">
          <ol>
            <li>Power on the Lift It! Dumbbell.</li>
            <li>Press the Start button on the application Home Page.</li>
            <li>When the first exercise prompt appears onscreen, complete the same movement with the dumbbell.</li>
            <li>If you complete the exercise in time, correctly, you will score a point! You can make 3 mistakes before the game ends.</li>
            <li>You can see your score at any time on the app screen, and when the game ends, try again by clicking the Play Again button.</li>
          </ol>
          <p> For exercise demonstrations, click the Exercise Demos button on the bottom of the page to view tutorials. </p>
        </div>
      </div>
    </main>
</template>

<script>
  export default {
    name: 'HowToPlayView'
  }
</script>

<style>

.instruction-page {
  min-height: 0;
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 20px 24px 30px;
  text-align: center;
  color: #f7f7f2;
}

.instruction-content {
  width: 100%;
  max-width: 650px;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 20px;
}

.instruction-content h2 {
  color: #f7f7f2;
  font-size: 30px;
  margin: 0;
}

.list-content {
  width: 100%;
  background: #202b38;
  border: 1px solid #354354;
  border-radius: 20px;
  padding: 24px;
  display: flex;
  flex-direction: column;
  text-align: left;
  gap: 15px;
}

.list-content li {
  margin-bottom: 15px;
}

.list-content p {
  text-align: center;
}

</style>