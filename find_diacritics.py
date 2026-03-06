import re, sys

with open(r'c:\s_lang\s-programming-language\shared\lexer\src\lexer_keywords.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

pattern = r'keywords_\["([^"]+)"\]\s*=\s*TokenType::(\w+)'
matches = re.findall(pattern, content)

diacritic_range = re.compile('[\u064B-\u065F]')

diacritic_names = {
    '\u064B': 'tanween fathah U+064B',
    '\u064C': 'tanween dammah U+064C',
    '\u064D': 'tanween kasrah U+064D',
    '\u064E': 'fathah U+064E',
    '\u064F': 'dammah U+064F',
    '\u0650': 'kasrah U+0650',
    '\u0651': 'shaddah U+0651',
    '\u0652': 'sukun U+0652',
}

out = open(r'c:\s_lang\s-programming-language\diac_output.txt', 'w', encoding='utf-8')

out.write(f'Total keyword entries found: {len(matches)}\n\n')
out.write('Keywords with Arabic diacritics:\n')
out.write('=' * 80 + '\n')

count = 0
for keyword, token_type in matches:
    found_diacritics = diacritic_range.findall(keyword)
    if found_diacritics:
        count += 1
        stripped = diacritic_range.sub('', keyword)
        diac_list = ', '.join(diacritic_names.get(d, f'U+{ord(d):04X}') for d in set(found_diacritics))
        codepoints = ' '.join(f'U+{ord(c):04X}' for c in keyword)
        out.write(f'{count}. Keyword: "{keyword}"  ->  TokenType::{token_type}\n')
        out.write(f'   Diacritics found: {diac_list}\n')
        out.write(f'   Without diacritics: "{stripped}"\n')
        out.write(f'   Codepoints: {codepoints}\n\n')

out.write(f'Total keywords with diacritics: {count}\n')
out.close()
print("Done")
