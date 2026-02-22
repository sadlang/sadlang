// ═══════════════════════════════════════════════════════════════════════════════
// تطبيق نظام الذكاء الاصطناعي للألعاب — لغة ص
// Game AI System Implementation — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// يتضمن: إيجاد المسار (A*)، آلة الحالات (FSM)، سلوكيات التوجيه، الأسراب
// ═══════════════════════════════════════════════════════════════════════════════

#include "graphics/sad_ai.h"
#include <SDL.h>
#include <cmath>
#include <algorithm>
#include <queue>
#include <set>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace sad { namespace ai {

// ═══════════════════════════════════════════════════════════════════
// هياكل البيانات الداخلية
// ═══════════════════════════════════════════════════════════════════

// شبكة إيجاد المسار — تحتوي على مصفوفة العبور والتكلفة
struct Grid {
    int width = 0;
    int height = 0;
    std::vector<std::vector<bool>> walkable;   // هل الخلية قابلة للعبور؟
    std::vector<std::vector<float>> cost;      // تكلفة كل خلية
};

// آلة الحالات المحدودة — الحالات والانتقالات
struct FSM {
    std::unordered_map<int, AIState> states;   // قائمة الحالات
    // انتقالات: من حالة → قائمة (محفّز، حالة الهدف)
    std::unordered_map<int, std::vector<std::pair<std::string, int>>> transitions;
    int currentState = -1;                     // الحالة الحالية
};

// سرب — مجموعة من الكيانات تتحرك معاً
struct Flock {
    FlockConfig config;                        // إعدادات السرب
    std::vector<Agent> members;                // أعضاء السرب
    float areaX = 0, areaY = 0;               // حدود المنطقة
    float areaW = 800, areaH = 600;
    bool hasTarget = false;                    // هل يوجد هدف محدد؟
    float targetX = 0, targetY = 0;           // موضع الهدف
};

// ═══════════════════════════════════════════════════════════════════
// الحالة العامة — خرائط لتخزين الكائنات بمعرّفات فريدة
// ═══════════════════════════════════════════════════════════════════

static std::unordered_map<int, Grid> s_grids;       // شبكات المسارات
static std::unordered_map<int, Path> s_paths;        // المسارات المحسوبة
static std::unordered_map<int, FSM> s_fsms;          // آلات الحالات
static std::unordered_map<int, Agent> s_agents;      // الكيانات المتحركة
static std::unordered_map<int, Flock> s_flocks;      // الأسراب

// معرّفات تلقائية متزايدة لكل نوع
static int s_nextGridId = 1;
static int s_nextPathId = 1;
static int s_nextFsmId = 1;
static int s_nextAgentId = 1;
static int s_nextFlockId = 1;

// مولّد أرقام عشوائية عام
static std::mt19937 s_rng(std::random_device{}());

// ═══════════════════════════════════════════════════════════════════
// دوال مساعدة داخلية
// ═══════════════════════════════════════════════════════════════════

// حساب المسافة الإقليدية بين نقطتين
static float euclideanDist(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

// تقييد قيمة طول المتجه بحد أقصى
static void limitVector(float& vx, float& vy, float maxMag) {
    float mag = std::sqrt(vx * vx + vy * vy);
    if (mag > maxMag && mag > 0.0001f) {
        float scale = maxMag / mag;
        vx *= scale;
        vy *= scale;
    }
}

// حساب طول المتجه
static float vectorLength(float vx, float vy) {
    return std::sqrt(vx * vx + vy * vy);
}

// ═══════════════════════════════════════════════════════════════════
// تطبيق إيجاد المسار — خوارزمية A*
// ═══════════════════════════════════════════════════════════════════

// عقدة في خوارزمية A* — تمثل خلية على الشبكة
struct AStarNode {
    int x, y;
    float gCost;       // التكلفة من البداية
    float hCost;       // التقدير المتبقي (الحدسي)
    float fCost;       // التكلفة الكلية = g + h
    int parentIndex;   // فهرس الأب لاسترداد المسار

    // مقارنة بالأولوية — أقل تكلفة أولاً
    bool operator>(const AStarNode& other) const {
        return fCost > other.fCost;
    }
};

// إنشاء شبكة جديدة — جميع الخلايا قابلة للعبور بتكلفة 1
int createGrid(int width, int height) {
    int id = s_nextGridId++;
    Grid grid;
    grid.width = width;
    grid.height = height;

    // تهيئة مصفوفة العبور — كل الخلايا مفتوحة
    grid.walkable.resize(height, std::vector<bool>(width, true));

    // تهيئة مصفوفة التكلفة — تكلفة موحدة 1.0
    grid.cost.resize(height, std::vector<float>(width, 1.0f));

    s_grids[id] = std::move(grid);
    return id;
}

// تحرير شبكة من الذاكرة
void destroyGrid(int gridId) {
    s_grids.erase(gridId);
}

// تعيين خلية كممر أو حاجز
void setWalkable(int gridId, int x, int y, bool walkable) {
    auto it = s_grids.find(gridId);
    if (it == s_grids.end()) return;
    Grid& g = it->second;

    // التحقق من صحة الإحداثيات
    if (x < 0 || x >= g.width || y < 0 || y >= g.height) return;
    g.walkable[y][x] = walkable;
}

// تعيين تكلفة خلية — الأرضيات الوعرة أبطأ
void setCost(int gridId, int x, int y, float cost) {
    auto it = s_grids.find(gridId);
    if (it == s_grids.end()) return;
    Grid& g = it->second;

    if (x < 0 || x >= g.width || y < 0 || y >= g.height) return;
    g.cost[y][x] = cost;
}

// إيجاد أقصر مسار باستخدام خوارزمية A*
int findPath(int gridId, int startX, int startY, int endX, int endY) {
    int pathId = s_nextPathId++;
    Path result;
    result.found = false;
    result.totalCost = 0;

    auto it = s_grids.find(gridId);
    if (it == s_grids.end()) {
        // الشبكة غير موجودة — لا مسار
        s_paths[pathId] = result;
        return pathId;
    }

    const Grid& grid = it->second;

    // التحقق من صحة نقطتي البداية والنهاية
    if (startX < 0 || startX >= grid.width || startY < 0 || startY >= grid.height ||
        endX < 0 || endX >= grid.width || endY < 0 || endY >= grid.height) {
        s_paths[pathId] = result;
        return pathId;
    }

    // التحقق من أن نقطتي البداية والنهاية قابلتان للعبور
    if (!grid.walkable[startY][startX] || !grid.walkable[endY][endX]) {
        s_paths[pathId] = result;
        return pathId;
    }

    // إذا كانت البداية هي النهاية
    if (startX == endX && startY == endY) {
        result.found = true;
        result.totalCost = 0;
        result.points.push_back({startX, startY});
        s_paths[pathId] = result;
        return pathId;
    }

    int w = grid.width;
    int h = grid.height;

    // قائمة العقد المفتوحة — ترتيب بالأولوية (أقل تكلفة أولاً)
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openList;

    // مجموعة العقد المغلقة — تم زيارتها
    std::vector<bool> closed(w * h, false);

    // تخزين أفضل تكلفة g لكل خلية
    std::vector<float> bestG(w * h, 1e18f);

    // تخزين الأب لكل خلية لاسترداد المسار
    std::vector<int> parentMap(w * h, -1);

    // دالة تحويل الإحداثيات إلى فهرس أحادي البعد
    auto toIndex = [w](int x, int y) { return y * w + x; };

    // الحدسي — المسافة الإقليدية
    auto heuristic = [endX, endY](int x, int y) -> float {
        float dx = static_cast<float>(endX - x);
        float dy = static_cast<float>(endY - y);
        return std::sqrt(dx * dx + dy * dy);
    };

    // إضافة نقطة البداية
    AStarNode startNode;
    startNode.x = startX;
    startNode.y = startY;
    startNode.gCost = 0;
    startNode.hCost = heuristic(startX, startY);
    startNode.fCost = startNode.gCost + startNode.hCost;
    startNode.parentIndex = -1;

    openList.push(startNode);
    bestG[toIndex(startX, startY)] = 0;

    // الاتجاهات الثمانية — أعلى، أسفل، يمين، يسار + الأقطار
    static const int dx8[] = { 0, 0, 1, -1, 1, -1, 1, -1 };
    static const int dy8[] = { -1, 1, 0, 0, -1, -1, 1, 1 };
    static const float dcost8[] = { 1.0f, 1.0f, 1.0f, 1.0f,
                                     1.414f, 1.414f, 1.414f, 1.414f };

    bool found = false;

    // حلقة البحث الرئيسية
    while (!openList.empty()) {
        AStarNode current = openList.top();
        openList.pop();

        int ci = toIndex(current.x, current.y);

        // تخطي إذا تم زيارة هذه الخلية بتكلفة أقل
        if (closed[ci]) continue;
        closed[ci] = true;

        // هل وصلنا إلى الهدف؟
        if (current.x == endX && current.y == endY) {
            found = true;
            result.totalCost = current.gCost;

            // استرداد المسار — من النهاية إلى البداية
            std::vector<GridPoint> reversePath;
            int idx = ci;
            while (idx != -1) {
                int py = idx / w;
                int px = idx % w;
                reversePath.push_back({px, py});
                idx = parentMap[idx];
            }

            // عكس المسار ليصبح من البداية إلى النهاية
            std::reverse(reversePath.begin(), reversePath.end());
            result.points = std::move(reversePath);
            result.found = true;
            break;
        }

        // استكشاف الجيران الثمانية
        for (int d = 0; d < 8; d++) {
            int nx = current.x + dx8[d];
            int ny = current.y + dy8[d];

            // التحقق من الحدود
            if (nx < 0 || nx >= w || ny < 0 || ny >= h) continue;

            int ni = toIndex(nx, ny);

            // تخطي الخلايا المغلقة وغير القابلة للعبور
            if (closed[ni]) continue;
            if (!grid.walkable[ny][nx]) continue;

            // حساب التكلفة الجديدة — تكلفة الحركة × تكلفة الخلية
            float moveCost = dcost8[d] * grid.cost[ny][nx];
            float newG = current.gCost + moveCost;

            // تحديث فقط إذا وجدنا مساراً أرخص
            if (newG < bestG[ni]) {
                bestG[ni] = newG;
                parentMap[ni] = ci;

                AStarNode neighbor;
                neighbor.x = nx;
                neighbor.y = ny;
                neighbor.gCost = newG;
                neighbor.hCost = heuristic(nx, ny);
                neighbor.fCost = newG + neighbor.hCost;
                neighbor.parentIndex = ci;
                openList.push(neighbor);
            }
        }
    }

    if (!found) {
        result.found = false;
    }

    s_paths[pathId] = std::move(result);
    return pathId;
}

// الحصول على طول المسار (عدد النقاط)
int getPathLength(int pathId) {
    auto it = s_paths.find(pathId);
    if (it == s_paths.end()) return 0;
    return static_cast<int>(it->second.points.size());
}

// الحصول على إحداثي x لنقطة في المسار
int getPathX(int pathId, int index) {
    auto it = s_paths.find(pathId);
    if (it == s_paths.end()) return 0;
    if (index < 0 || index >= static_cast<int>(it->second.points.size())) return 0;
    return it->second.points[index].x;
}

// الحصول على إحداثي y لنقطة في المسار
int getPathY(int pathId, int index) {
    auto it = s_paths.find(pathId);
    if (it == s_paths.end()) return 0;
    if (index < 0 || index >= static_cast<int>(it->second.points.size())) return 0;
    return it->second.points[index].y;
}

// هل تم إيجاد مسار صالح؟
bool isPathFound(int pathId) {
    auto it = s_paths.find(pathId);
    if (it == s_paths.end()) return false;
    return it->second.found;
}

// تحرير مسار من الذاكرة
void destroyPath(int pathId) {
    s_paths.erase(pathId);
}

// ═══════════════════════════════════════════════════════════════════
// تطبيق آلة الحالات المحدودة (FSM)
// ═══════════════════════════════════════════════════════════════════

// إنشاء آلة حالات جديدة
int createFSM() {
    int id = s_nextFsmId++;
    FSM fsm;
    fsm.currentState = -1;
    s_fsms[id] = std::move(fsm);
    return id;
}

// تحرير آلة حالات من الذاكرة
void destroyFSM(int fsmId) {
    s_fsms.erase(fsmId);
}

// إضافة حالة جديدة لآلة الحالات
void addState(int fsmId, int stateId, const std::string& name) {
    auto it = s_fsms.find(fsmId);
    if (it == s_fsms.end()) return;

    AIState state;
    state.id = stateId;
    state.name = name;
    it->second.states[stateId] = state;
}

// تعيين الحالة الابتدائية
void setInitialState(int fsmId, int stateId) {
    auto it = s_fsms.find(fsmId);
    if (it == s_fsms.end()) return;

    // التحقق من وجود الحالة
    if (it->second.states.find(stateId) == it->second.states.end()) return;
    it->second.currentState = stateId;
}

// إضافة انتقال — من حالة إلى أخرى عند تحقق محفّز معين
void addTransition(int fsmId, int fromState, int toState, const std::string& trigger) {
    auto it = s_fsms.find(fsmId);
    if (it == s_fsms.end()) return;

    it->second.transitions[fromState].push_back({trigger, toState});
}

// إطلاق محفّز — البحث عن انتقال مطابق من الحالة الحالية
bool fireTrigger(int fsmId, const std::string& trigger) {
    auto it = s_fsms.find(fsmId);
    if (it == s_fsms.end()) return false;

    FSM& fsm = it->second;
    if (fsm.currentState < 0) return false;

    // البحث في انتقالات الحالة الحالية
    auto trIt = fsm.transitions.find(fsm.currentState);
    if (trIt == fsm.transitions.end()) return false;

    for (const auto& trans : trIt->second) {
        if (trans.first == trigger) {
            // تم العثور على انتقال مطابق — تنفيذه
            fsm.currentState = trans.second;
            return true;
        }
    }

    // لم يُعثر على انتقال مطابق
    return false;
}

// الحصول على معرّف الحالة الحالية
int getCurrentState(int fsmId) {
    auto it = s_fsms.find(fsmId);
    if (it == s_fsms.end()) return -1;
    return it->second.currentState;
}

// الحصول على اسم الحالة الحالية
std::string getCurrentStateName(int fsmId) {
    auto it = s_fsms.find(fsmId);
    if (it == s_fsms.end()) return "";

    FSM& fsm = it->second;
    auto stIt = fsm.states.find(fsm.currentState);
    if (stIt == fsm.states.end()) return "";
    return stIt->second.name;
}

// ═══════════════════════════════════════════════════════════════════
// تطبيق سلوكيات التوجيه (Steering Behaviors)
// ═══════════════════════════════════════════════════════════════════

// إنشاء كيان جديد (عدو، لاعب، NPC)
int createAgent(float x, float y, float maxSpeed) {
    int id = s_nextAgentId++;
    Agent agent;
    agent.x = x;
    agent.y = y;
    agent.maxSpeed = maxSpeed;
    agent.maxForce = maxSpeed * 0.5f;  // القوة القصوى نصف السرعة القصوى افتراضياً
    agent.vx = 0;
    agent.vy = 0;
    agent.sightRange = 200.0f;
    agent.radius = 16.0f;
    agent.state = 0;
    s_agents[id] = agent;
    return id;
}

// تحرير كيان من الذاكرة
void destroyAgent(int agentId) {
    s_agents.erase(agentId);
}

// تعيين موضع الكيان
void setAgentPosition(int agentId, float x, float y) {
    auto it = s_agents.find(agentId);
    if (it == s_agents.end()) return;
    it->second.x = x;
    it->second.y = y;
}

// تعيين سرعة الكيان
void setAgentVelocity(int agentId, float vx, float vy) {
    auto it = s_agents.find(agentId);
    if (it == s_agents.end()) return;
    it->second.vx = vx;
    it->second.vy = vy;
}

// تعيين مدى رؤية الكيان
void setAgentSightRange(int agentId, float range) {
    auto it = s_agents.find(agentId);
    if (it == s_agents.end()) return;
    it->second.sightRange = range;
}

// الحصول على إحداثي x للكيان
float getAgentX(int agentId) {
    auto it = s_agents.find(agentId);
    if (it == s_agents.end()) return 0;
    return it->second.x;
}

// الحصول على إحداثي y للكيان
float getAgentY(int agentId) {
    auto it = s_agents.find(agentId);
    if (it == s_agents.end()) return 0;
    return it->second.y;
}

// ─── سلوك المطاردة (Seek) ───
// حساب قوة التوجيه نحو الهدف وتطبيقها
void seek(int agentId, float targetX, float targetY, float dt) {
    auto it = s_agents.find(agentId);
    if (it == s_agents.end()) return;

    Agent& a = it->second;

    // حساب الاتجاه المطلوب نحو الهدف
    float desiredX = targetX - a.x;
    float desiredY = targetY - a.y;

    // تطبيع وتكبير إلى السرعة القصوى
    float dist = vectorLength(desiredX, desiredY);
    if (dist > 0.0001f) {
        desiredX = (desiredX / dist) * a.maxSpeed;
        desiredY = (desiredY / dist) * a.maxSpeed;
    }

    // قوة التوجيه = الاتجاه المطلوب - السرعة الحالية
    float steerX = desiredX - a.vx;
    float steerY = desiredY - a.vy;

    // تقييد قوة التوجيه بالحد الأقصى
    limitVector(steerX, steerY, a.maxForce);

    // تطبيق القوة على السرعة
    a.vx += steerX * dt;
    a.vy += steerY * dt;

    // تقييد السرعة بالحد الأقصى
    limitVector(a.vx, a.vy, a.maxSpeed);

    // تحديث الموضع
    a.x += a.vx * dt;
    a.y += a.vy * dt;
}

// ─── سلوك الهروب (Flee) ───
// عكس المطاردة — الابتعاد عن التهديد
void flee(int agentId, float threatX, float threatY, float dt) {
    auto it = s_agents.find(agentId);
    if (it == s_agents.end()) return;

    Agent& a = it->second;

    // حساب الاتجاه بعيداً عن التهديد (عكس الاتجاه نحوه)
    float desiredX = a.x - threatX;
    float desiredY = a.y - threatY;

    float dist = vectorLength(desiredX, desiredY);
    if (dist > 0.0001f) {
        desiredX = (desiredX / dist) * a.maxSpeed;
        desiredY = (desiredY / dist) * a.maxSpeed;
    }

    // حساب وتطبيق قوة التوجيه
    float steerX = desiredX - a.vx;
    float steerY = desiredY - a.vy;
    limitVector(steerX, steerY, a.maxForce);

    a.vx += steerX * dt;
    a.vy += steerY * dt;
    limitVector(a.vx, a.vy, a.maxSpeed);

    a.x += a.vx * dt;
    a.y += a.vy * dt;
}

// ─── سلوك التجوال (Wander) ───
// إضافة زاوية عشوائية صغيرة للاتجاه الحالي لمحاكاة حركة طبيعية
void wander(int agentId, float dt) {
    auto it = s_agents.find(agentId);
    if (it == s_agents.end()) return;

    Agent& a = it->second;

    // حساب الزاوية الحالية للحركة
    float heading = std::atan2(a.vy, a.vx);

    // إضافة تغيير عشوائي صغير للزاوية
    std::uniform_real_distribution<float> angleDist(-0.5f, 0.5f);
    float wanderAngle = heading + angleDist(s_rng);

    // حساب نقطة هدف على دائرة أمام الكيان
    float circleDistance = 60.0f;   // بُعد الدائرة عن الكيان
    float circleRadius = 30.0f;    // نصف قطر دائرة التجوال

    // مركز الدائرة أمام الكيان
    float speed = vectorLength(a.vx, a.vy);
    float dirX = (speed > 0.001f) ? a.vx / speed : 1.0f;
    float dirY = (speed > 0.001f) ? a.vy / speed : 0.0f;

    float circleCenterX = a.x + dirX * circleDistance;
    float circleCenterY = a.y + dirY * circleDistance;

    // نقطة على محيط الدائرة بالزاوية العشوائية
    float targetX = circleCenterX + std::cos(wanderAngle) * circleRadius;
    float targetY = circleCenterY + std::sin(wanderAngle) * circleRadius;

    // حساب قوة التوجيه نحو النقطة العشوائية
    float desiredX = targetX - a.x;
    float desiredY = targetY - a.y;
    float dist = vectorLength(desiredX, desiredY);
    if (dist > 0.0001f) {
        desiredX = (desiredX / dist) * a.maxSpeed;
        desiredY = (desiredY / dist) * a.maxSpeed;
    }

    float steerX = desiredX - a.vx;
    float steerY = desiredY - a.vy;
    limitVector(steerX, steerY, a.maxForce);

    a.vx += steerX * dt;
    a.vy += steerY * dt;
    limitVector(a.vx, a.vy, a.maxSpeed);

    a.x += a.vx * dt;
    a.y += a.vy * dt;
}

// ─── سلوك الوصول (Arrive) ───
// مثل المطاردة لكن مع تباطؤ عند الاقتراب من الهدف
void arrive(int agentId, float targetX, float targetY, float slowRadius, float dt) {
    auto it = s_agents.find(agentId);
    if (it == s_agents.end()) return;

    Agent& a = it->second;

    float desiredX = targetX - a.x;
    float desiredY = targetY - a.y;
    float dist = vectorLength(desiredX, desiredY);

    if (dist < 0.0001f) {
        // وصلنا — توقف تماماً
        a.vx = 0;
        a.vy = 0;
        return;
    }

    // حساب السرعة المطلوبة — تتناسب مع المسافة داخل منطقة التباطؤ
    float desiredSpeed = a.maxSpeed;
    if (dist < slowRadius) {
        desiredSpeed = a.maxSpeed * (dist / slowRadius);
    }

    // تطبيع وتكبير الاتجاه إلى السرعة المطلوبة
    desiredX = (desiredX / dist) * desiredSpeed;
    desiredY = (desiredY / dist) * desiredSpeed;

    // حساب وتطبيق قوة التوجيه
    float steerX = desiredX - a.vx;
    float steerY = desiredY - a.vy;
    limitVector(steerX, steerY, a.maxForce);

    a.vx += steerX * dt;
    a.vy += steerY * dt;
    limitVector(a.vx, a.vy, a.maxSpeed);

    a.x += a.vx * dt;
    a.y += a.vy * dt;
}

// هل الكيان يرى النقطة المعطاة؟ (المسافة أقل من مدى الرؤية)
bool canSee(int agentId, float targetX, float targetY) {
    auto it = s_agents.find(agentId);
    if (it == s_agents.end()) return false;

    const Agent& a = it->second;
    float dist = euclideanDist(a.x, a.y, targetX, targetY);
    return dist <= a.sightRange;
}

// ═══════════════════════════════════════════════════════════════════
// تطبيق سلوك الأسراب (Flocking)
// ═══════════════════════════════════════════════════════════════════

// إنشاء سرب — مجموعة من الكيانات في مواضع عشوائية داخل منطقة محددة
int createFlock(int count, float areaX, float areaY, float areaW, float areaH) {
    int id = s_nextFlockId++;
    Flock flock;
    flock.areaX = areaX;
    flock.areaY = areaY;
    flock.areaW = areaW;
    flock.areaH = areaH;
    flock.hasTarget = false;

    // توزيعات عشوائية للمواضع والسرعات الابتدائية
    std::uniform_real_distribution<float> distX(areaX, areaX + areaW);
    std::uniform_real_distribution<float> distY(areaY, areaY + areaH);
    std::uniform_real_distribution<float> distAngle(0.0f, static_cast<float>(2.0 * M_PI));

    for (int i = 0; i < count; i++) {
        Agent member;
        member.x = distX(s_rng);
        member.y = distY(s_rng);
        member.maxSpeed = flock.config.maxSpeed;
        member.maxForce = flock.config.maxSpeed * 0.5f;
        member.radius = 8.0f;

        // سرعة ابتدائية عشوائية
        float angle = distAngle(s_rng);
        float speed = flock.config.maxSpeed * 0.5f;
        member.vx = std::cos(angle) * speed;
        member.vy = std::sin(angle) * speed;

        flock.members.push_back(member);
    }

    s_flocks[id] = std::move(flock);
    return id;
}

// تحرير سرب من الذاكرة
void destroyFlock(int flockId) {
    s_flocks.erase(flockId);
}

// تحديث حركة السرب — تطبيق قواعد الانفصال والمحاذاة والتماسك
void updateFlock(int flockId, float dt) {
    auto it = s_flocks.find(flockId);
    if (it == s_flocks.end()) return;

    Flock& flock = it->second;
    const FlockConfig& cfg = flock.config;
    int n = static_cast<int>(flock.members.size());
    if (n == 0) return;

    // حساب القوى لكل عضو في السرب
    for (int i = 0; i < n; i++) {
        Agent& me = flock.members[i];

        // تراكم قوى الانفصال والمحاذاة والتماسك
        float sepX = 0, sepY = 0;     // الانفصال — الابتعاد عن الجيران القريبين
        float aliX = 0, aliY = 0;     // المحاذاة — متوسط سرعة الجيران
        float cohX = 0, cohY = 0;     // التماسك — الانجذاب لمركز المجموعة

        int sepCount = 0;
        int neighborCount = 0;

        for (int j = 0; j < n; j++) {
            if (i == j) continue;

            const Agent& other = flock.members[j];
            float dist = euclideanDist(me.x, me.y, other.x, other.y);

            // قوة الانفصال — ابتعاد عن الأفراد القريبين جداً
            if (dist < cfg.separationRadius && dist > 0.001f) {
                float diffX = me.x - other.x;
                float diffY = me.y - other.y;
                // كلما كان الجار أقرب، كانت قوة الابتعاد أكبر
                sepX += diffX / dist;
                sepY += diffY / dist;
                sepCount++;
            }

            // قوى المحاذاة والتماسك — من الجيران ضمن مسافة الجوار
            if (dist < cfg.neighborRadius) {
                aliX += other.vx;
                aliY += other.vy;
                cohX += other.x;
                cohY += other.y;
                neighborCount++;
            }
        }

        // حساب متوسط قوة الانفصال
        if (sepCount > 0) {
            sepX /= static_cast<float>(sepCount);
            sepY /= static_cast<float>(sepCount);
        }

        // حساب متوسط المحاذاة (متوسط سرعة الجيران - السرعة الحالية)
        if (neighborCount > 0) {
            aliX = aliX / static_cast<float>(neighborCount) - me.vx;
            aliY = aliY / static_cast<float>(neighborCount) - me.vy;

            // حساب التماسك (التوجه نحو مركز المجموعة)
            cohX = cohX / static_cast<float>(neighborCount) - me.x;
            cohY = cohY / static_cast<float>(neighborCount) - me.y;
        }

        // دمج القوى الثلاث مع الأوزان
        float forceX = sepX * cfg.separationWeight
                      + aliX * cfg.alignmentWeight
                      + cohX * cfg.cohesionWeight;
        float forceY = sepY * cfg.separationWeight
                      + aliY * cfg.alignmentWeight
                      + cohY * cfg.cohesionWeight;

        // إضافة قوة التوجه نحو الهدف إن وُجد
        if (flock.hasTarget) {
            float toTargetX = flock.targetX - me.x;
            float toTargetY = flock.targetY - me.y;
            float tDist = vectorLength(toTargetX, toTargetY);
            if (tDist > 0.001f) {
                toTargetX /= tDist;
                toTargetY /= tDist;
            }
            // وزن الهدف مماثل لوزن التماسك
            forceX += toTargetX * cfg.cohesionWeight;
            forceY += toTargetY * cfg.cohesionWeight;
        }

        // تقييد القوة الإجمالية
        float maxForce = cfg.maxSpeed * 0.5f;
        limitVector(forceX, forceY, maxForce);

        // تحديث السرعة والموضع
        me.vx += forceX * dt;
        me.vy += forceY * dt;
        limitVector(me.vx, me.vy, cfg.maxSpeed);

        me.x += me.vx * dt;
        me.y += me.vy * dt;

        // التفاف عند حدود المنطقة — يظهر من الجانب الآخر
        if (me.x < flock.areaX) me.x += flock.areaW;
        if (me.x > flock.areaX + flock.areaW) me.x -= flock.areaW;
        if (me.y < flock.areaY) me.y += flock.areaH;
        if (me.y > flock.areaY + flock.areaH) me.y -= flock.areaH;
    }
}

// الحصول على إحداثي x لعنصر من السرب
float getFlockMemberX(int flockId, int index) {
    auto it = s_flocks.find(flockId);
    if (it == s_flocks.end()) return 0;
    if (index < 0 || index >= static_cast<int>(it->second.members.size())) return 0;
    return it->second.members[index].x;
}

// الحصول على إحداثي y لعنصر من السرب
float getFlockMemberY(int flockId, int index) {
    auto it = s_flocks.find(flockId);
    if (it == s_flocks.end()) return 0;
    if (index < 0 || index >= static_cast<int>(it->second.members.size())) return 0;
    return it->second.members[index].y;
}

// الحصول على حجم السرب (عدد الأعضاء)
int getFlockSize(int flockId) {
    auto it = s_flocks.find(flockId);
    if (it == s_flocks.end()) return 0;
    return static_cast<int>(it->second.members.size());
}

// تعيين وجهة السرب — نقطة جذب مشتركة
void setFlockTarget(int flockId, float x, float y) {
    auto it = s_flocks.find(flockId);
    if (it == s_flocks.end()) return;
    it->second.hasTarget = true;
    it->second.targetX = x;
    it->second.targetY = y;
}

// ═══════════════════════════════════════════════════════════════════
// رسم التصحيح (Debug Rendering) — يستخدم SDL2
// ═══════════════════════════════════════════════════════════════════

// رسم أعضاء السرب كدوائر صغيرة ملونة
void renderFlock(void* renderer, int flockId, uint8_t r, uint8_t g, uint8_t b) {
    if (!renderer) return;
    SDL_Renderer* sdlRenderer = static_cast<SDL_Renderer*>(renderer);

    auto it = s_flocks.find(flockId);
    if (it == s_flocks.end()) return;

    SDL_SetRenderDrawColor(sdlRenderer, r, g, b, 255);

    const Flock& flock = it->second;
    for (const Agent& member : flock.members) {
        // رسم كل عضو كدائرة صغيرة (تقريب بمربع ٤×٤)
        int cx = static_cast<int>(member.x);
        int cy = static_cast<int>(member.y);
        int radius = 3;

        // رسم الدائرة باستخدام خوارزمية بسيطة
        for (int dy = -radius; dy <= radius; dy++) {
            for (int dx = -radius; dx <= radius; dx++) {
                if (dx * dx + dy * dy <= radius * radius) {
                    SDL_RenderDrawPoint(sdlRenderer, cx + dx, cy + dy);
                }
            }
        }

        // رسم خط قصير يمثل اتجاه الحركة
        float speed = vectorLength(member.vx, member.vy);
        if (speed > 0.1f) {
            int endX = cx + static_cast<int>((member.vx / speed) * 8.0f);
            int endY = cy + static_cast<int>((member.vy / speed) * 8.0f);
            SDL_RenderDrawLine(sdlRenderer, cx, cy, endX, endY);
        }
    }
}

// رسم الشبكة — الخلايا القابلة للعبور والحواجز بألوان مختلفة
void renderGrid(void* renderer, int gridId, int tileSize, float offsetX, float offsetY) {
    if (!renderer) return;
    SDL_Renderer* sdlRenderer = static_cast<SDL_Renderer*>(renderer);

    auto it = s_grids.find(gridId);
    if (it == s_grids.end()) return;

    const Grid& grid = it->second;

    for (int y = 0; y < grid.height; y++) {
        for (int x = 0; x < grid.width; x++) {
            SDL_Rect rect;
            rect.x = static_cast<int>(offsetX) + x * tileSize;
            rect.y = static_cast<int>(offsetY) + y * tileSize;
            rect.w = tileSize - 1;  // ترك فراغ صغير بين الخلايا
            rect.h = tileSize - 1;

            if (grid.walkable[y][x]) {
                // خلية مفتوحة — اللون يعكس التكلفة (أخضر فاتح للتكلفة المنخفضة)
                float c = grid.cost[y][x];
                uint8_t green = static_cast<uint8_t>(std::max(50.0f, 200.0f / c));
                SDL_SetRenderDrawColor(sdlRenderer, 40, green, 40, 180);
            } else {
                // حاجز — لون أحمر داكن
                SDL_SetRenderDrawColor(sdlRenderer, 160, 30, 30, 220);
            }
            SDL_RenderFillRect(sdlRenderer, &rect);
        }
    }
}

// رسم المسار — نقاط ملونة متصلة بخطوط
void renderPath(void* renderer, int pathId, int tileSize,
                float offsetX, float offsetY, uint8_t r, uint8_t g, uint8_t b) {
    if (!renderer) return;
    SDL_Renderer* sdlRenderer = static_cast<SDL_Renderer*>(renderer);

    auto it = s_paths.find(pathId);
    if (it == s_paths.end()) return;

    const Path& path = it->second;
    if (!path.found || path.points.empty()) return;

    SDL_SetRenderDrawColor(sdlRenderer, r, g, b, 255);
    int halfTile = tileSize / 2;

    // رسم خطوط بين النقاط المتتالية
    for (size_t i = 0; i + 1 < path.points.size(); i++) {
        int x1 = static_cast<int>(offsetX) + path.points[i].x * tileSize + halfTile;
        int y1 = static_cast<int>(offsetY) + path.points[i].y * tileSize + halfTile;
        int x2 = static_cast<int>(offsetX) + path.points[i + 1].x * tileSize + halfTile;
        int y2 = static_cast<int>(offsetY) + path.points[i + 1].y * tileSize + halfTile;
        SDL_RenderDrawLine(sdlRenderer, x1, y1, x2, y2);
    }

    // رسم نقطة ملونة في مركز كل خلية على المسار
    for (const GridPoint& pt : path.points) {
        int cx = static_cast<int>(offsetX) + pt.x * tileSize + halfTile;
        int cy = static_cast<int>(offsetY) + pt.y * tileSize + halfTile;

        // دائرة صغيرة بنصف قطر ٣ بكسل
        for (int dy = -3; dy <= 3; dy++) {
            for (int dx = -3; dx <= 3; dx++) {
                if (dx * dx + dy * dy <= 9) {
                    SDL_RenderDrawPoint(sdlRenderer, cx + dx, cy + dy);
                }
            }
        }
    }
}

}} // namespace sad::ai
