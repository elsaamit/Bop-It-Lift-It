<template>
    <div class="row">
      <button class="link-button" @click="$router.push({ name: 'how-to-play' })" >How to Play</button>
      <button class="link-button" @click="$router.push({ name: 'demo' })" >Video Demos</button>
      <button class="link-button" @click="$router.push({ name: 'home' })" >Home</button>
    </div>
</template>

<script>
export default {
  name: 'AppFooter',
}

</script>

<style scoped>

a {
  color: #424eb9ff;
}

.row{
  display: flex;
  margin: 0 10px;
  gap: 100px;
  justify-content: center;
  width: 100%;
  align-items: center;
}

.link-button{
  margin: 0;
  border: none;
  padding: 10px 25px;
  background-color: #2b5080ff;
  color: white;
  border-radius: 25px;
  font-size: 16px;
  cursor: pointer;
  width: 170px;
}

.link-button:hover{
  background-color: #54729cff
}

</style>
