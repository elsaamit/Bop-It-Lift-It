
const WebSocket = require('ws')

const server = new WebSocket.Server({ port: 8081 })

let gameState = 'idle'
let score = 0

server.on('connection', (socket) => {
  console.log('Vue client connected!')

  socket.on('message', (data) => {
    const message = JSON.parse(data.toString())

    console.log('Received:', message)

    if (message.type === 'start_game') {

      if (gameState === 'playing') {
        socket.send(JSON.stringify({
          version: 1,
          type: 'command_result',
          request_id: message.request_id,
          success: false,
          message: 'A game is already running.'
        }))
        return
      }

      gameState = 'playing'
      score = 0

      // Acknowledge the command.
      socket.send(JSON.stringify({
        version: 1,
        type: 'command_result',
        request_id: message.request_id,
        success: true,
        message: 'Game started successfully.'
      }))

      // Notify Vue that the game has started.
      socket.send(JSON.stringify({
        version: 1,
        type: 'game_started',
        game_id: 'test-game-001',
        score: score
      }))

      console.log('Game started!')
    }

    else if (message.type === 'get_status') {
      socket.send(JSON.stringify({
        version: 1,
        type: 'status_update',
        game_state: gameState,
        score: score,
        game_id: null,
        current_prompt: null
      }))
    }
  })
})

console.log('WebSocket server listening on port 8081')
