import { ref } from 'vue'

import bicepCurl from '@/assets/bicep.gif'
import hammerCurl from '@/assets/hammer.gif'
import preacherCurl from '@/assets/preacher.png'

//images/gifs for each exercise
const exerciseImages = {
    bicep: bicepCurl,
    hammer: hammerCurl,
    preacher: preacherCurl
}

//reactive variables for game
export const gameState = ref('idle')
export const score = ref(0)
export const exercise = ref(null)
export const finalScore = ref(0)
export const sentMessageResult = ref(null) //message from vue to esp
export const lastUserResult = ref(null) //did the user do it right

//only need 3 states on web app
function normalizeGameState(state){ 
    switch(state){ 

        //case 1: idle
        case 'ST_IDLE':
            return 'ST_IDLE'

        //case 2: playing 
        case 'ST_WAIT_START':
        case 'ST_IN_REP':
        case 'ST_FEEDBACK':
            return 'playing'
        
        //case 3: game over
        case 'ST_GAME_OVER':
            return 'ST_GAME_OVER'
        
        //unknown state; do not change
        default:
            return gameState.value

    }
}

//normalize format
export function normalizeExercise(message) {
    if(!message){
        return null
    }

    return {
        id: message.exercise_id,
        curlType: message.curl_type,
        text: message.text,
        image: exerciseImages[message.curl_type] || null,
        timeLimit: message.time_limit
    }
}

//function to handle incoming message. Expected format for each type as follows:
// game_started: score
// exercise_update: exercise_id, curl_type, text, time_limit
// score_update: score
// user_result: exercise_id, success, response_time
// sent_message_result: success, message, request_id
// game_over: final_score, reason
// status_update: game_state, score, exercise

// exercise: object containing the exercise_id, curl_type, text, time_limit
export function handleMessage(message){
    switch(message.type){

        //game is starting
        case 'game_started':                  
            gameState.value = 'playing'
            score.value = 0
            finalScore.value = 0
            exercise.value = null
            sentMessageResult.value = null
            lastUserResult.value = null
            break

        //ESP32 has sent a new exercise
        case 'exercise_update':
            exercise.value = normalizeExercise(message)
            lastUserResult.value = null
            break

        //update score
        case 'score_update':
            score.value = message.score
            break

        //user completed exercise (either successfully or unsuccesfully)
        case 'user_result':
            lastUserResult.value = {
                exerciseId: message.exercise_id,
                success: message.success,
                responseTime: message.response_time
            }
            break
        

        //ESP has acknowledged message from Vue
        case 'sent_message_result':
            sentMessageResult.value = {
                success: message.success,
                message: message.message,
                requestId: message.request_id
            }
            break

        //game has ended
        case 'game_over':
            gameState.value = 'game_over'
            finalScore.value = message.final_score
            score.value = message.final_score
            exercise.value = null
            break

        //ESP has reported game status
        case 'status_update':
            gameState.value = normalizeGameState(message.game_state)
            if(message.score !== undefined){
                score.value = message.score
            }

            exercise.value = normalizeExercise(message.exercise)
            break

        default:
            console.log('Unhandled message type:', message.type)
    }
}