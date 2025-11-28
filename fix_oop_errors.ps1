# حذف جميع دوال getType() من class_nodes.h
# Remove all getType() functions from class_nodes.h

$file = "C:\s\s_language\include\parser\ast\class_nodes.h"
$content = Get-Content $file -Raw

# حذف سطور getType مع التعليقات
$content = $content -replace '(?ms)\s+/\*\*\s+\* @brief \(AR\) الحصول على نوع العقدة\s+\* @brief \(EN\) Get node type\s+\*/\s+NodeType getType\(\) const override \{ return NodeType::\w+; \}\s+', "`n    "

Set-Content $file -Value $content -Encoding UTF8

Write-Host "Fixed class_nodes.h - removed getType() functions"
