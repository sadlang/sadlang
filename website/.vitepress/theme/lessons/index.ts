// ═══════════════════════════════════════════════════════════════
// تصدير جميع المستويات — Export all levels
// ═══════════════════════════════════════════════════════════════
import { level1 } from './level1'
import { level2 } from './level2'
import { level3 } from './level3'
import { level4 } from './level4'
import { level5 } from './level5'
import type { Level } from './types'
export type { Level, Section, Lesson, LevelMeta } from './types'

export const allLevels: Level[] = [level1, level2, level3, level4, level5]

export function getLevelMeta(level: Level) {
  return {
    id: level.id,
    title: level.title,
    titleEn: level.titleEn,
    description: level.description,
    icon: level.icon,
    color: level.color,
    lessonCount: level.sections.reduce((sum, s) => sum + s.lessons.length, 0),
    sectionCount: level.sections.length,
  }
}
