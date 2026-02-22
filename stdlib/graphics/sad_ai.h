// ═══════════════════════════════════════════════════════════════════════════════
// نظام الذكاء الاصطناعي للألعاب — لغة ص
// Game AI System — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// نظام ذكاء اصطناعي مدمج للألعاب — Flutter ليس لديه أي نظام مشابه!
// حتى محركات الألعاب المتقدمة تحتاج مكتبات خارجية لهذا.
//
// الميزات:
//   🔍 إيجاد المسار (A* Pathfinding) — أقصر مسار على شبكة ثنائية
//   🤖 آلة الحالات المحدودة (FSM) — حالات + انتقالات + شروط
//   🎯 سلوكيات التوجيه (Steering) — مطاردة، هروب، تجوال، تتبع
//   🧠 القرارات — شجرة قرارات بسيطة مع عتاقيد (Utility AI)
//   👥 سلوك الأسراب (Flocking) — محاكاة حركة مجموعة (طيور/أسماك)
//
// بسطر واحد يمكنك:
//   - إيجاد أقصر مسار لعدو يطارد اللاعب
//   - جعل الأعداء يتجولون ثم يطاردون عند الاقتراب
//   - محاكاة سرب من الطيور أو الأسماك
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <functional>

namespace sad { namespace ai {

// ═══════════════════════════════════════════════════════════════════
// هياكل البيانات الأساسية
// ═══════════════════════════════════════════════════════════════════

// نقطة على الشبكة (للمسارات)
struct GridPoint {
    int x = 0, y = 0;
};

// نتيجة إيجاد المسار — سلسلة من النقاط
struct Path {
    std::vector<GridPoint> points;   // نقاط المسار بالترتيب
    bool found = false;              // هل تم إيجاد مسار؟
    float totalCost = 0;             // التكلفة الإجمالية
};

// ═══════════════════════════════════════════════════════════════════
// آلة الحالات المحدودة (Finite State Machine)
// ═══════════════════════════════════════════════════════════════════

// حالة في FSM
struct AIState {
    int id = 0;
    std::string name;           // اسم الحالة (مثلاً: "خمول"، "مطاردة"، "هجوم")
};

// ═══════════════════════════════════════════════════════════════════
// سلوكيات التوجيه (Steering Behaviors)
// ═══════════════════════════════════════════════════════════════════

// معلومات كيان متحرك (عدو، لاعب، NPC)
struct Agent {
    float x = 0, y = 0;         // الموضع
    float vx = 0, vy = 0;       // السرعة
    float maxSpeed = 100.0f;     // السرعة القصوى
    float maxForce = 50.0f;      // القوة القصوى للتوجيه
    float radius = 16.0f;        // نصف قطر الكيان (للتصادم والرؤية)
    int state = 0;               // الحالة الحالية (في FSM)
    float sightRange = 200.0f;   // مدى الرؤية
};

// ═══════════════════════════════════════════════════════════════════
// سلوك الأسراب (Flocking)
// ═══════════════════════════════════════════════════════════════════

// إعدادات سلوك السرب
struct FlockConfig {
    float separationWeight = 1.5f;   // قوة الابتعاد عن الأفراد القريبين
    float alignmentWeight = 1.0f;    // قوة محاذاة الاتجاه مع الجيران
    float cohesionWeight = 1.0f;     // قوة الانجذاب نحو مركز السرب
    float separationRadius = 30.0f;  // مسافة الابتعاد
    float neighborRadius = 80.0f;    // مسافة الجوار
    float maxSpeed = 80.0f;          // السرعة القصوى لكل فرد
};

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة التطبيقات — إيجاد المسار (A* Pathfinding)
// ═══════════════════════════════════════════════════════════════════

// إنشاء شبكة للمسارات — تُرجع معرّف الشبكة
// الخلايا المحظورة (obstacles) تُشكَّل بـ setWalkable
int createGrid(int width, int height);
void destroyGrid(int gridId);

// تعيين خلية كممر أو حاجز
void setWalkable(int gridId, int x, int y, bool walkable);

// تعيين كلفة خلية (الأرضيات الوعرة أبطأ)
void setCost(int gridId, int x, int y, float cost);

// إيجاد أقصر مسار من (بx، بy) إلى (هx، هy) — خوارزمية A*
// تُرجع معرّف المسار
int findPath(int gridId, int startX, int startY, int endX, int endY);

// الحصول على طول المسار (عدد النقاط)
int getPathLength(int pathId);

// الحصول على إحداثيات نقطة في المسار
int getPathX(int pathId, int index);
int getPathY(int pathId, int index);

// هل تم إيجاد المسار؟
bool isPathFound(int pathId);

// تحرير المسار من الذاكرة
void destroyPath(int pathId);

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة التطبيقات — آلة الحالات (FSM)
// ═══════════════════════════════════════════════════════════════════

// إنشاء آلة حالات — تُرجع معرّف FSM
int createFSM();
void destroyFSM(int fsmId);

// إضافة حالة
void addState(int fsmId, int stateId, const std::string& name);

// تعيين الحالة الأولية
void setInitialState(int fsmId, int stateId);

// إضافة انتقال: من حالة → إلى حالة (عندما يتحقق شرط معين)
void addTransition(int fsmId, int fromState, int toState, const std::string& trigger);

// إطلاق محفّز (trigger) — يتحقق من وجود انتقال ويُنفذه
bool fireTrigger(int fsmId, const std::string& trigger);

// الحصول على الحالة الحالية
int getCurrentState(int fsmId);
std::string getCurrentStateName(int fsmId);

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة التطبيقات — سلوكيات التوجيه
// ═══════════════════════════════════════════════════════════════════

// إنشاء كيان (عدو/NPC) — تُرجع معرّف الكيان
int createAgent(float x, float y, float maxSpeed = 100.0f);
void destroyAgent(int agentId);

// تعيين موضع وسرعة الكيان
void setAgentPosition(int agentId, float x, float y);
void setAgentVelocity(int agentId, float vx, float vy);
void setAgentSightRange(int agentId, float range);

// الحصول على موضع الكيان
float getAgentX(int agentId);
float getAgentY(int agentId);

// ─── سلوكيات التوجيه ───

// مطاردة — التوجه نحو هدف
void seek(int agentId, float targetX, float targetY, float dt);

// هروب — الابتعاد عن تهديد
void flee(int agentId, float threatX, float threatY, float dt);

// تجوال عشوائي
void wander(int agentId, float dt);

// الوصول — مطاردة مع تباطؤ عند الاقتراب
void arrive(int agentId, float targetX, float targetY, float slowRadius, float dt);

// هل الكيان يرى النقطة المعطاة؟
bool canSee(int agentId, float targetX, float targetY);

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة التطبيقات — سلوك الأسراب (Flocking)
// ═══════════════════════════════════════════════════════════════════

// إنشاء سرب — تُرجع معرّف السرب
int createFlock(int count, float areaX, float areaY, float areaW, float areaH);
void destroyFlock(int flockId);

// تحديث حركة السرب
void updateFlock(int flockId, float dt);

// الحصول على موضع عنصر من السرب
float getFlockMemberX(int flockId, int index);
float getFlockMemberY(int flockId, int index);
int getFlockSize(int flockId);

// تعيين وجهة السرب
void setFlockTarget(int flockId, float x, float y);

// رسم السرب كنقاط ملونة
void renderFlock(void* renderer, int flockId, uint8_t r, uint8_t g, uint8_t b);

// رسم الشبكة (للتصحيح)
void renderGrid(void* renderer, int gridId, int tileSize, float offsetX = 0, float offsetY = 0);

// رسم المسار
void renderPath(void* renderer, int pathId, int tileSize,
                float offsetX, float offsetY, uint8_t r, uint8_t g, uint8_t b);

}} // namespace sad::ai
