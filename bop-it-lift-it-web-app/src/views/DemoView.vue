<template>
    <main>
        <h1> Demonstration </h1>
        <div class="oneline">
          <div class="demo-content">
            <h3> Biceps Curl </h3>
            <img class="image" src="../assets/bicep.gif">
          </div>
          <div class="demo-content">
            <h3> Hammer Curl </h3>
            <img class="image" src="../assets/hammer.gif">
          </div>
          <div class="demo-content">
            <h3> Preacher Curl </h3>
            <img class="image" src="../assets/preacher.png">
          </div>
        </div>

    </main>
</template>

<script>
  export default {
    name: 'DemoView'
  }
</script>

<style>

h1 {
  font-size: 48px;
  margin: 0 0 20px;
  color: #c6ff4d;
  letter-spacing: 3px;
  text-align: center;
}


.oneline {
  display: flex;
  margin: 0 10px;
  gap: 150px;
  justify-content: center;
  width: 100%;
  align-items: center;
}

.image {
  height: 400px;
}

.demo-content{
  text-align: center;
}

</style>