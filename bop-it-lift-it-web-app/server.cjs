
const WebSocket = require('ws')

const server = new WebSocket.Server({ port: 8080 })

server.on('connection', (socket) => {
  console.log('Vue client connected!')

  socket.on('message', (data) => {
    const message = data.toString()
    console.log('Received from Vue:', message)

    // Echo the message back to Vue.
    socket.send(message)
  })

  socket.on('close', () => {
    console.log('Vue client disconnected.')
  })
})

console.log('WebSocket server listening on port 8080')
