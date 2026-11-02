import {ref} from 'vue'
const status = ref('Disconnected')
const lastMessage = ref(null)

let socket = null

//connects websocket
function connect() {
    if (socket && socket.readyState !== WebSocket.CLOSED){
        return
    }
}

//initializes websocket
socket = new WebSocket('ws://localhost:8081')

socket.addEventListener('open', () => {
    status.value = 'Connected'
    
    sendMessage({
        version: 1,
        type: 'get_game_status',
        request_id: 'status-01'
    })
})

//receive messages from ESP32
socket.addEventListener('message', (event) => {
    try{
        const message = JSON.parse(event.data)
        lastMessage.value = message

        console.log('Received:', message)
    } catch (error){
        console.error('Invalid JSON:', error)
    }
})

//send message to ESP32
function sendMessage(message){
    if(socket?.readyState === WebSocket.OPEN){
        socket.send(JSON.stringify(message))
    } else {
        console.warn('WebSocket is disconnected, cannot send message')
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//GAME FUNCTIONS//
function startGame(){
    sendMessage({
        version: 1,
        type: 'start_game',
        request_id: crypto.randomUUID()
    })
}

export{
    status,
    lastMessage,
    connect,
    sendMessage,
    startGame
}