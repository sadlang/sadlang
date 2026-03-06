export type { QuizQuestion, SectionQuiz, LevelQuizzes } from './types'
export { level1Quizzes } from './level1'
export { level2Quizzes } from './level2'
export { level3Quizzes } from './level3'
export { level4Quizzes } from './level4'
export { level5Quizzes } from './level5'

import type { SectionQuiz } from './types'
import { level1Quizzes } from './level1'
import { level2Quizzes } from './level2'
import { level3Quizzes } from './level3'
import { level4Quizzes } from './level4'
import { level5Quizzes } from './level5'

/** خريطة كل الاختبارات حسب المستوى */
const allQuizzesByLevel: Record<number, SectionQuiz[]> = {
  1: level1Quizzes,
  2: level2Quizzes,
  3: level3Quizzes,
  4: level4Quizzes,
  5: level5Quizzes,
}

/** جلب أسئلة قسم معين في مستوى معين */
export function getQuizForSection(levelId: number, sectionId: string): SectionQuiz | undefined {
  const levelQuizzes = allQuizzesByLevel[levelId]
  if (!levelQuizzes) return undefined
  return levelQuizzes.find(q => q.sectionId === sectionId)
}

/** جلب كل أسئلة مستوى معين */
export function getQuizzesForLevel(levelId: number): SectionQuiz[] {
  return allQuizzesByLevel[levelId] || []
}

/** عدد الأسئلة الإجمالي */
export function getTotalQuestionCount(): number {
  return Object.values(allQuizzesByLevel)
    .flat()
    .reduce((sum, sq) => sum + sq.questions.length, 0)
}
