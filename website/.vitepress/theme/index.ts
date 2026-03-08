// ═══════════════════════════════════════════════════════════════
// Custom Theme — يوسّع السمة الافتراضية بدعم RTL عربي كامل
// ═══════════════════════════════════════════════════════════════
import { h } from 'vue'
import DefaultTheme from 'vitepress/theme'
import type { Theme } from 'vitepress'

// مكونات مخصصة
import HomeHero from './components/HomeHero.vue'
import DownloadCard from './components/DownloadCard.vue'
import CodePlayground from './components/CodePlayground.vue'
import CodePlaygroundV2 from './components/CodePlaygroundV2.vue'
import FeatureGrid from './components/FeatureGrid.vue'
import SadVersion from './components/SadVersion.vue'
import InteractiveTutorial from './components/InteractiveTutorial.vue'
import LearningPath from './components/LearningPath.vue'
import LevelTabs from './components/LevelTabs.vue'
import SectionList from './components/SectionList.vue'
import LessonWizard from './components/LessonWizard.vue'
import SadCodeRunner from './components/SadCodeRunner.vue'
import LearningStats from './components/LearningStats.vue'
import AchievementBadges from './components/AchievementBadges.vue'
import QuizModal from './components/QuizModal.vue'

import './style.css'

export default {
  extends: DefaultTheme,

  Layout() {
    return h(DefaultTheme.Layout, null, {
      // فتحة فوق محتوى المستندات
      'doc-before': () => null,
    })
  },

  enhanceApp({ app, router }) {
    // تسجيل المكونات العامة
    app.component('HomeHero',           HomeHero)
    app.component('DownloadCard',       DownloadCard)
    app.component('CodePlayground',     CodePlayground)
    app.component('CodePlaygroundV2',   CodePlaygroundV2)
    app.component('FeatureGrid',        FeatureGrid)
    app.component('SadVersion',         SadVersion)
    app.component('InteractiveTutorial', InteractiveTutorial)
    app.component('LearningPath',       LearningPath)
    app.component('LevelTabs',          LevelTabs)
    app.component('SectionList',        SectionList)
    app.component('LessonWizard',       LessonWizard)
    app.component('SadCodeRunner',      SadCodeRunner)
    app.component('LearningStats',      LearningStats)
    app.component('AchievementBadges',  AchievementBadges)
    app.component('QuizModal',          QuizModal)

    // تطبيق اتجاه RTL عند تحميل الصفحة
    if (typeof window !== 'undefined') {
      router.onAfterRouteChanged = () => {
        document.documentElement.dir  = 'rtl'
        document.documentElement.lang = 'ar'
      }
    }
  },
} satisfies Theme
