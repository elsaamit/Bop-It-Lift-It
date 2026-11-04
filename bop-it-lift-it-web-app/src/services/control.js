import { ref } from 'vue'

import bicepCurl from '@/assets/bicep.gif'
import hammerCurl from '@/assets/hammer.gif'
import doubleCurl from '@/assets/double.png'

//images/gifs for each command
const commandImages = {
    bicep: bicepCurl,
    hammer: hammerCurl,
    double: doubleCurl
}

//reactive variables for game
export const gameState = ref('idle')
export const score = ref(0)
export const command = ref(null)
export const finalScore = ref(0)
export const lastCommandResult = ref(null)
export const lastUserResult = ref(null)

export function handleMessage(message) {

    switch(message.type){

        //game is starting
        case 'game_started':                  
            gameState.value = 'playing'
            score.value = 0
            finalScore.value = 0
            command.value = null
            break

        //new command
        case 'command_update':
            command.value = {
                id: message.command_id,
                action: message.action,
                text: message.text,
                image: commandImages[message.action] || null,
                timeLimit: message.time_limit
            }
            break

        //update score
        case 'score_update':
            score.value = message.score
            break

        //get whether or not user succeeded in last command
        case 'user_result':
            lastUserResult.value = message.last_user_result
            break
        
        //game has ended
        case 'game_over':
            gameState.value = 'game_over'
            finalScore.value = message.final_score
            command.value = null
            break

        //update status
        case 'status_update':
            gameState.value = message.game_state
            score.value = message.score
            if(message.command){
                command.value = message.command
                image: commandImages[message.command.action] || null
            } else {
                command.value = null
            }
            break

        default:
            console.log('Unhandled message type:', message.type)
    }
}