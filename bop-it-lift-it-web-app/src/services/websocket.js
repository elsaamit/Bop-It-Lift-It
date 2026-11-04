import {ref} from 'vue'
import { handleMessage } from './control'


let socket = null

//connects websocket
function connect() {
    
    //if already open
    if (socket && socket.readyState !== WebSocket.CLOSED){
        return
    }

    socket = new WebSocket('ws://localhost:8081')

    socket.addEventListener('message', (event) => {
        try {
            const message = JSON.parse(event.data)
            handleMessage(message)
        } catch (error){
            console.error('Invalid message', error)
        }
    })
}

//send message to ESP32 in JSON format
function sendMessage(message){
    if(socket?.readyState === WebSocket.OPEN){
        socket.send(JSON.stringify(message))
    } else {
        console.warn('WebSocket is disconnected, cannot send message')
    }
}

//start game- send to esp
function startGame(){
    sendMessage({
        version: 1,
        type: 'start_game',
        request_id: crypto.randomUUID()
    })
}

//end game- send to esp
function endGame(){
    sendMessage({
        version: 1,
        type: 'end_game',
        request_id: crypto.randomUUID()
    })
}



export{
    connect,
    sendMessage,
    startGame,
    endGame
}