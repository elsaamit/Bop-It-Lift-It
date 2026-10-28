<template>
  <div class="container">
    <main class="main-content">
      <h1 class="title"> Bop It, Lift It </h1>
      <img alt="enclosure" src="../assets/sketch.png" class="dumbbell-image">
      <button class="play-button" @click="$router.push({name: 'play'})"> Play </button>
    </main>

  </div>
</template>

<script>
  export default {
    name: 'HomeView'
  }
</script>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;;
}

.title {
  font-size: 64px;
  font-weight: bold;
}

.container{
  display: flex;
  flex-direction: column;
  min-height: 100vh;
}

.main-content{
  flex: 1;
  min-height: 0;
}

.dumbbell-image{
  display: block;
  width: 100%;
  max-width: 550px;
  height: auto;
  margin: 20px auto;
  margin-top: 50px;
  object-fix: contain;
}

.play-button{
  border: none;
  padding: 10px 25px;
  background-color: #327adeff;
  color: white;
  border-radius: 25px;
  font-size: 40px;
  cursor: pointer;
  margin-top: 35px;
  text-align: center;
}

.play-button:hover{
  background-color: #9ebeecff
}

</style>