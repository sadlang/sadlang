// ═══════════════════════════════════════════════════════════════
// أنواع بيانات الاختبارات — Quiz Data Types
// ═══════════════════════════════════════════════════════════════

export interface QuizQuestion {
  question: string
  options: string[]
  correct: number  // 0-based index
  explanation: string
}

export interface SectionQuiz {
  sectionId: string
  questions: QuizQuestion[]
}

export interface LevelQuizzes {
  levelId: number
  sections: SectionQuiz[]
}
