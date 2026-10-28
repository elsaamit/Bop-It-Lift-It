import HomeView from '../views/HomeView.vue'
import DemoView from '../views/DemoView.vue'
import PlayView from '../views/PlayView.vue'
import HowToPlayView from '../views/HowToPlayView.vue'
import { createRouter, createWebHistory } from 'vue-router'

const routes = [
    {
        path: '/',
        name: 'home',
        component: HomeView
    },
    {
        path: '/play',
        name: 'play',
        component: PlayView
    },
    {
        path: '/how-to-play',
        name: 'how-to-play',
        component: HowToPlayView
    },
    {
        path:'/demo',
        name: 'demo',
        component: DemoView
    },
]

const router = createRouter({
    history: createWebHistory(),
    routes
})

export default router