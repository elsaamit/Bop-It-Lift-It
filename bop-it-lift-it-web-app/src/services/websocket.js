import {ref} from 'vue'
import { handleMessage } from './control'


let socket = null

 //reactive value to track connection status
export const connectionStatus = ref('disconnected')


//connect to ESP32
function connect() {
    
    //if socket already open 
    if (socket && socket.readyState !== WebSocket.CLOSED){
        return
    }

    //need to add ESP32 IP address & port
    socket = new WebSocket('ws://localhost:8081')

    //on opening of socket
    socket.addEventListener('open', () => {
        connectionStatus.value = 'connected'
    })

    //receive messages
    socket.addEventListener('message', (event) => {
        try {
            const message = JSON.parse(event.data)
            handleMessage(message)
        } catch (error){
            console.error('Invalid message', error)
        }
    })

    //on close of socket
    socket.addEventListener('close', () => {
        connectionStatus.value = 'disconnected'
        socket = null
    })

    //graceful error handling
    socket.addEventListener('error', (e) => {
        connectionStatus.value = 'error'
    })
}


//send message to ESP32
function sendMessage(message){
    if(socket?.readyState === WebSocket.OPEN){
        socket.send(JSON.stringify(message))
    } else {
        console.warn('WebSocket is disconnected, cannot send message')
    }
}

//start game- send to ESP32
function startGame(){
    console.log('startGame called')
    sendMessage({
        version: 1,
        type: 'start_game',
        request_id: crypto.randomUUID()
    })
}

//stop game- send to ESP32
function stopGame(){
    sendMessage({
        version: 1,
        type: 'stop_game',
        request_id: crypto.randomUUID()
    })
}

//disconnect from ESP32
function disconnect() {
    if (socket){
        socket.close();
    }
}


export{
    connect,
    disconnect,
    sendMessage,
    startGame,
    stopGame
}