import { ref } from 'vue'

export interface Achievement {
  id: string
  title: string
  description: string
  icon: string
  category: 'lesson' | 'level' | 'streak' | 'speed'
  unlocked: boolean
  unlockedAt: number | null
}

const STORAGE_KEY = 'sad-learn-achievements'

const defaultAchievements: Omit<Achievement, 'unlocked' | 'unlockedAt'>[] = [
  // Lesson milestones
  { id: 'lesson_1', title: 'أول درس', description: 'أكملت أول درس لك', icon: '🌱', category: 'lesson' },
  { id: 'lesson_10', title: 'عشرة دروس', description: 'أكملت 10 دروس', icon: '📘', category: 'lesson' },
  { id: 'lesson_50', title: 'خمسون درساً', description: 'أكملت 50 درساً', icon: '🔥', category: 'lesson' },
  { id: 'lesson_100', title: 'مئة درس', description: 'أكملت 100 درس', icon: '💯', category: 'lesson' },
  { id: 'lesson_200', title: 'مئتا درس', description: 'أكملت 200 درس', icon: '🏅', category: 'lesson' },
  { id: 'lesson_300', title: 'أكملت الكل', description: 'أكملت جميع الدروس الـ 300', icon: '👑', category: 'lesson' },
  // Level milestones
  { id: 'level_1', title: 'إتمام المستوى ١', description: 'أنهيت المستوى الأول بالكامل', icon: '⭐', category: 'level' },
  { id: 'level_2', title: 'إتمام المستوى ٢', description: 'أنهيت المستوى الثاني بالكامل', icon: '⭐', category: 'level' },
  { id: 'level_3', title: 'إتمام المستوى ٣', description: 'أنهيت المستوى الثالث بالكامل', icon: '🌟', category: 'level' },
  { id: 'level_4', title: 'إتمام المستوى ٤', description: 'أنهيت المستوى الرابع بالكامل', icon: '🌟', category: 'level' },
  { id: 'level_5', title: 'إتمام المستوى ٥', description: 'أنهيت المستوى الخامس بالكامل', icon: '💎', category: 'level' },
  // Streak
  { id: 'streak_1', title: 'يوم واحد', description: 'تعلّمت يوماً متواصلاً', icon: '✨', category: 'streak' },
  { id: 'streak_7', title: 'أسبوع كامل', description: 'تعلّمت 7 أيام متواصلة', icon: '🔥', category: 'streak' },
  { id: 'streak_30', title: 'شهر كامل', description: 'تعلّمت 30 يوماً متواصلاً', icon: '🏆', category: 'streak' },
  // Speed
  { id: 'speed_5', title: 'سريع البرق', description: 'أكملت 5 دروس في جلسة واحدة', icon: '⚡', category: 'speed' },
]

const lessonThresholds: Record<string, number> = {
  lesson_1: 1, lesson_10: 10, lesson_50: 50,
  lesson_100: 100, lesson_200: 200, lesson_300: 300,
}
const streakThresholds: Record<string, number> = {
  streak_1: 1, streak_7: 7, streak_30: 30,
}

export const hasNewAchievement = ref(false)

function loadAchievements(): Achievement[] {
  const stored = typeof localStorage !== 'undefined' ? localStorage.getItem(STORAGE_KEY) : null
  const map: Record<string, { unlocked: boolean; unlockedAt: number | null }> = {}
  if (stored) {
    try {
      const parsed: Achievement[] = JSON.parse(stored)
      for (const a of parsed) map[a.id] = { unlocked: a.unlocked, unlockedAt: a.unlockedAt }
    } catch { /* ignore */ }
  }
  return defaultAchievements.map(d => ({
    ...d,
    unlocked: map[d.id]?.unlocked ?? false,
    unlockedAt: map[d.id]?.unlockedAt ?? null,
  }))
}

function saveAchievements(achievements: Achievement[]) {
  if (typeof localStorage !== 'undefined')
    localStorage.setItem(STORAGE_KEY, JSON.stringify(achievements))
}

export function useAchievements() {
  function getAchievements(): Achievement[] {
    return loadAchievements()
  }

  function getUnlockedCount(): number {
    return loadAchievements().filter(a => a.unlocked).length
  }

  function getLatestAchievement(): Achievement | null {
    const unlocked = loadAchievements().filter(a => a.unlocked && a.unlockedAt)
    if (!unlocked.length) return null
    return unlocked.reduce((latest, a) => (a.unlockedAt! > latest.unlockedAt! ? a : latest))
  }

  function checkAchievements(
    completedCount: number,
    levelCompletions: boolean[],
    streak: number,
    sessionCount: number,
  ): Achievement[] {
    const achievements = loadAchievements()
    const newlyUnlocked: Achievement[] = []
    const now = Date.now()

    for (const a of achievements) {
      if (a.unlocked) continue
      let earned = false

      if (a.category === 'lesson' && lessonThresholds[a.id] != null)
        earned = completedCount >= lessonThresholds[a.id]
      else if (a.category === 'level') {
        const lvl = parseInt(a.id.replace('level_', ''), 10)
        earned = !!levelCompletions[lvl - 1]
      } else if (a.category === 'streak' && streakThresholds[a.id] != null)
        earned = streak >= streakThresholds[a.id]
      else if (a.id === 'speed_5')
        earned = sessionCount >= 5

      if (earned) {
        a.unlocked = true
        a.unlockedAt = now
        newlyUnlocked.push(a)
      }
    }

    if (newlyUnlocked.length) {
      saveAchievements(achievements)
      hasNewAchievement.value = true
    }
    return newlyUnlocked
  }

  return { getAchievements, getUnlockedCount, getLatestAchievement, checkAchievements, hasNewAchievement }
}
