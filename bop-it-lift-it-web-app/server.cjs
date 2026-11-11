
const WebSocket = require('ws')

const server = new WebSocket.Server({ port: 8081 })

let gameState = 'ST_IDLE'
let score = 0
let exerciseId = 0
let currentExercise = null
let resultTime = null
let testResult = false

const exercises = [
  {
    curl_type: 'bicep',
    text: 'Bicep Curl',
    time_limit: 5000
  },
  {
    curl_type: 'hammer',
    text: 'Hammer Curl',
    time_limit: 5000
  },
  {
    curl_type: 'preacher',
    text: 'Preacher Curl',
    time_limit: 5000
  }
]

// send message to Vue
function sendMessage(socket, message) {
  if(socket.readyState === WebSocket.OPEN){
    socket.send(JSON.stringify({
      version: 1,
      ...message
    }))

    console.log('Sent:', message)
  } else {
    console.log('Cannot send message due to connection. Socket state:', socket.readyState)
  }
}

// send next exercise (TEST)
function sendExercise(socket){
  exerciseId++

  const selectedExercise = exercises[(exerciseId -1) % exercises.length]
  currentExercise = {
    exercise_id: exerciseId,
    ...selectedExercise
  }

  //test exercise update
  gameState = 'ST_IN_REP'
  sendMessage(socket, {
    type: 'exercise_update',
    ...currentExercise
  })

  //test user result
  resultTime = setTimeout(() => {
    if (gameState !== 'ST_IN_REP') return
    testResult = !testResult

    sendMessage(socket, {
      type: 'user_result',
      exercise_id: exerciseId,
      success: testResult,
      response_time: 2000
    })

    if(testResult){
      score++
    }

    //test score update
    sendMessage(socket, {
      type: 'score_update',
      score: score
    })

    //2 seconds before showing next exercise
    gameState = 'ST_FEEDBACK'

    resultTime = setTimeout(() => {
      if (gameState === 'ST_FEEDBACK') {
        sendExercise(socket)
      } 
    }, 2000)
  }, 2000)
}

//test start
function startGame(socket, message){
  if(gameState !== 'ST_IDLE' && gameState !== 'ST_GAME_OVER'){
    sendMessage(socket, {
      type: 'sent_message_result',
      request_id: message.request_id,
      success: false,
      message: "Game already running."
    })
    return
  }

  clearTimeout(resultTime)

  gameState= 'ST_WAIT_START'
  score = 0
  exerciseId = 0
  currentExercise = null

  sendMessage(socket, {
    type: 'sent_message_result',
    request_id: message.request_id,
    success: true,
    message: 'Game started.'
  })

  sendMessage(socket, {
    type: 'game_started',
    game_id: 'test_game_1',
    score: score
  })

  console.log('Game started')
  sendExercise(socket)
}

//test stop
function stopGame(socket, message){
  clearTimeout(resultTime)
  gameState = 'ST_GAME_OVER'
  sendMessage(socket, {
    type: 'sent_message_result',
    request_id: message.request_id,
    success: true,
    message: 'Game stopped'
  })

  sendMessage(socket, {
    type: 'game_over',
    final_score: score,
    reason: 'user_stopped'
  })
}

//set up
server.on('connection', (socket) => {
  console.log('web app connected')
  socket.on('message', (data) => {
    try {
      const message = JSON.parse(data.toString())
      console.log("received:", message)

      if(message.type === 'start_game'){
        startGame(socket, message);
      }
      else if(message.type === 'stop_game'){
        stopGame(socket, message);
      }
      else if(message.type === 'get_status'){
        sendMessage(socket, {
          type: 'status_update',
          game_state: gameState,
          score: score,
          exercise: currentExercise
        })
      }
    } catch (error) {
      console.error('Invalid message', error)
    }
  })

  socket.on('close', () => {
    console.log('web app disconnected')
    clearTimeout(resultTime)
  })
})

console.log('WebSocket server listening on port 8081')
