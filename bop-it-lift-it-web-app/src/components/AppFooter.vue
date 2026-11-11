<template>
    <footer class="app-footer">
      <button class="link-button" @click="$router.push({ name: 'how-to-play' })" >HOW TO PLAY</button>
      <button class="link-button" @click="$router.push({ name: 'demo' })" >DEMOS</button>
      <button class="link-button" @click="$router.push({ name: 'home' })" >HOME</button>
    </footer>
</template>

<script>
export default {
  name: 'AppFooter',
}

</script>

<style scoped>

.app-footer {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: clamp(15px, 8vw, 120px);
  width: 100%;
  padding: 20px 24px;
  box-sizing: border-box;
}

.link-button {
  background: #294f7a;
  color: white;
  border: 1px solid #42658c;
  border-radius: 12px;
  padding: 13px 20px;
  width: 170px;
  font-size: 15px;
  font-weight: 800;
  cursor: pointer;
  transition: background 0.2s, transform 0.2s;
}

.link-button:hover {
  background: #3d6897;
  transform: translateY(-2px);
}

@media (max-width: 600px) {
  .app-footer {
    gap: 8px;
    padding: 12px 8px;
  }

  .link-button {
    width: auto;
    flex: 1;
    padding: 12px 5px;
    font-size: 11px;
  }
}
</style>