import type { Level } from '../types'
import { section1 } from './section1'
import { section2 } from './section2'
import { section3 } from './section3'
import { section4 } from './section4'
import { section5 } from './section5'
import { section6 } from './section6'

export const level2: Level = {
  id: 2,
  title: 'المبتدئ',
  titleEn: 'Beginner',
  description: 'أتقن الأدوات الأساسية: الدوال والمصفوفات والأخطاء',
  icon: '🌿',
  color: '#3b82f6',
  sections: [section1, section2, section3, section4, section5, section6],
}
