// ═══════════════════════════════════════════════════════════════
// أنواع بيانات الدروس — Lesson Data Types
// ═══════════════════════════════════════════════════════════════

export interface Lesson {
  id: string
  title: string
  desc: string
  explanation?: string  // شرح موسع ومفصل للدرس
  code: string
  exercise: string
  hint: string
  tips: string[]
  concepts: string[]
}

export interface Section {
  id: string
  title: string
  icon: string
  lessons: Lesson[]
}

export interface Level {
  id: number
  title: string
  titleEn: string
  description: string
  icon: string
  color: string
  sections: Section[]
}

export interface LevelMeta {
  id: number
  title: string
  titleEn: string
  description: string
  icon: string
  color: string
  lessonCount: number
  sectionCount: number
}
