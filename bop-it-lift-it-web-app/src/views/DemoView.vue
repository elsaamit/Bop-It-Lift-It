<template>
    <main>
        <h1> Demonstration </h1>
        <div class="oneline">
          <div>
            <h3> Biceps Curl </h3>
            <img class="image" src="../assets/bicep.gif">
          </div>
          <div>
            <h3> Hammer Curl </h3>
            <img class="image" src="../assets/hammer.gif">
          </div>
          <div>
            <h3> Double Arm Curl </h3>
            <img class="image" src="../assets/double.png">
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

</style>