import type { Level } from '../types'
import { section1 } from './section1'
import { section2 } from './section2'
import { section3 } from './section3'
import { section4 } from './section4'
import { section5 } from './section5'
import { section6 } from './section6'
import { section7 } from './section7'
import { section8 } from './section8'

/** المستوى الثالث — المتوسط (60 درساً) */
export const level3: Level = {
  id: 3,
  title: 'المتوسط',
  titleEn: 'Intermediate',
  description: 'البرمجة الكائنية، مطابقة الأنماط، الوحدات، والدوال المتقدمة.',
  icon: '🚀',
  color: '#8b5cf6',
  sections: [section1, section2, section3, section4, section5, section6, section7, section8]
}
