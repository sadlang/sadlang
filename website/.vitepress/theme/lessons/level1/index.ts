import type { Level } from '../types'
import { section1 } from './section1'
import { section2 } from './section2'
import { section3 } from './section3'
import { section4 } from './section4'
import { section5 } from './section5'

export const level1: Level = {
  id: 1,
  title: 'من الصفر',
  titleEn: 'From Zero',
  description: 'ابدأ رحلتك البرمجية — لا خبرة سابقة مطلوبة',
  icon: '🌱',
  color: '#10b981',
  sections: [section1, section2, section3, section4, section5],
}
