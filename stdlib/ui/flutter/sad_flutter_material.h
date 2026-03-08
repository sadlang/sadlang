// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_material.h — مكونات Material Design المتقدمة
//  Advanced Material Design Components
// ═══════════════════════════════════════════════════════════════════════════════

#ifndef SAD_FLUTTER_MATERIAL_H
#define SAD_FLUTTER_MATERIAL_H

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"
#include "sad_flutter_layout.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Card — بطاقة Material
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) بطاقة Material مع ظل وحواف
 * @brief (EN) Material card with shadow and rounded corners
 */
class Card : public StatelessWidget {
public:
    Card(WidgetPtr child,
         Color color = Color(255, 255, 255),
         float elevation = 1.0f,
         ShapeBorder shape = ShapeBorder{BorderRadius::all(4)},
         EdgeInsets margin = EdgeInsets{},
         bool borderOnForeground = true)
        : child_(child), color_(color), elevation_(elevation),
          shape_(shape), margin_(margin), borderOnForeground_(borderOnForeground) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    WidgetPtr child_;
    Color color_;
    float elevation_;
    ShapeBorder shape_;
    EdgeInsets margin_;
    bool borderOnForeground_;
};

// اسم عربي
using بطاقة = Card;

// ═══════════════════════════════════════════════════════════════════════════════
//  Chip Types — أنواع الشرائح
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) شريحة Material الأساسية
 * @brief (EN) Base Material chip
 */
class Chip : public StatelessWidget {
public:
    Chip(WidgetPtr label,
         WidgetPtr avatar = nullptr,
         WidgetPtr deleteIcon = nullptr,
         std::function<void()> onDeleted = nullptr,
         Color backgroundColor = Color(224, 224, 224),
         EdgeInsets padding = EdgeInsets::symmetric(12, 4),
         EdgeInsets labelPadding = EdgeInsets::symmetric(4, 0),
         float elevation = 0.0f)
        : label_(label), avatar_(avatar), deleteIcon_(deleteIcon),
          onDeleted_(onDeleted), backgroundColor_(backgroundColor),
          padding_(padding), labelPadding_(labelPadding), elevation_(elevation) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
protected:
    WidgetPtr label_;
    WidgetPtr avatar_;
    WidgetPtr deleteIcon_;
    std::function<void()> onDeleted_;
    Color backgroundColor_;
    EdgeInsets padding_;
    EdgeInsets labelPadding_;
    float elevation_;
};

// اسم عربي
using شريحة = Chip;

/**
 * @brief (AR) شريحة إدخال مع حذف
 * @brief (EN) Input chip with delete functionality
 */
class InputChip : public Chip {
public:
    InputChip(WidgetPtr label,
              bool selected = false,
              WidgetPtr avatar = nullptr,
              std::function<void(bool)> onSelected = nullptr,
              std::function<void()> onDeleted = nullptr,
              std::function<void()> onPressed = nullptr,
              bool isEnabled = true,
              Color selectedColor = Color(200, 200, 200))
        : Chip(label, avatar), selected_(selected), onSelected_(onSelected),
          onPressed_(onPressed), isEnabled_(isEnabled), selectedColor_(selectedColor) {
        onDeleted_ = onDeleted;
    }
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    bool selected_;
    std::function<void(bool)> onSelected_;
    std::function<void()> onPressed_;
    bool isEnabled_;
    Color selectedColor_;
};

using شريحة_ادخال = InputChip;

/**
 * @brief (AR) شريحة فلتر قابلة للتحديد
 * @brief (EN) Filter chip for selecting filters
 */
class FilterChip : public StatelessWidget {
public:
    FilterChip(WidgetPtr label,
               bool selected = false,
               std::function<void(bool)> onSelected = nullptr,
               WidgetPtr avatar = nullptr,
               Color selectedColor = Color(200, 200, 200),
               Color backgroundColor = Color(224, 224, 224),
               Color checkmarkColor = Color(0, 0, 0),
               bool showCheckmark = true)
        : label_(label), selected_(selected), onSelected_(onSelected),
          avatar_(avatar), selectedColor_(selectedColor),
          backgroundColor_(backgroundColor), checkmarkColor_(checkmarkColor),
          showCheckmark_(showCheckmark) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    WidgetPtr label_;
    bool selected_;
    std::function<void(bool)> onSelected_;
    WidgetPtr avatar_;
    Color selectedColor_;
    Color backgroundColor_;
    Color checkmarkColor_;
    bool showCheckmark_;
};

using شريحة_فلتر = FilterChip;

/**
 * @brief (AR) شريحة اختيار (اختيار واحد)
 * @brief (EN) Choice chip for single selection
 */
class ChoiceChip : public StatelessWidget {
public:
    ChoiceChip(WidgetPtr label,
               bool selected = false,
               std::function<void(bool)> onSelected = nullptr,
               WidgetPtr avatar = nullptr,
               Color selectedColor = Color(100, 149, 237),
               Color disabledColor = Color(200, 200, 200),
               Color backgroundColor = Color(224, 224, 224))
        : label_(label), selected_(selected), onSelected_(onSelected),
          avatar_(avatar), selectedColor_(selectedColor),
          disabledColor_(disabledColor), backgroundColor_(backgroundColor) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    WidgetPtr label_;
    bool selected_;
    std::function<void(bool)> onSelected_;
    WidgetPtr avatar_;
    Color selectedColor_;
    Color disabledColor_;
    Color backgroundColor_;
};

using شريحة_اختيار = ChoiceChip;

/**
 * @brief (AR) شريحة إجراء (زر)
 * @brief (EN) Action chip (button-like)
 */
class ActionChip : public StatelessWidget {
public:
    ActionChip(WidgetPtr label,
               std::function<void()> onPressed,
               WidgetPtr avatar = nullptr,
               Color backgroundColor = Color(224, 224, 224),
               float elevation = 0.0f,
               float pressElevation = 8.0f)
        : label_(label), onPressed_(onPressed), avatar_(avatar),
          backgroundColor_(backgroundColor), elevation_(elevation),
          pressElevation_(pressElevation) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    WidgetPtr label_;
    std::function<void()> onPressed_;
    WidgetPtr avatar_;
    Color backgroundColor_;
    float elevation_;
    float pressElevation_;
};

using شريحة_اجراء = ActionChip;

// ═══════════════════════════════════════════════════════════════════════════════
//  DataTable — جدول البيانات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) عمود في جدول البيانات
 * @brief (EN) Data table column definition
 */
struct DataColumn {
    WidgetPtr label;
    std::string tooltip;
    bool numeric = false;
    std::function<void(int, bool)> onSort = nullptr;
};

using عمود_بيانات = DataColumn;

/**
 * @brief (AR) خلية في جدول البيانات
 * @brief (EN) Data table cell
 */
struct DataCell {
    WidgetPtr child;
    bool placeholder = false;
    bool showEditIcon = false;
    std::function<void()> onTap = nullptr;
    std::function<void()> onLongPress = nullptr;
    std::function<void()> onDoubleTap = nullptr;
    
    DataCell(WidgetPtr c) : child(c) {}
    
    static DataCell empty() {
        return DataCell(std::make_shared<SizedBox>());
    }
};

using خلية_بيانات = DataCell;

/**
 * @brief (AR) صف في جدول البيانات
 * @brief (EN) Data table row
 */
struct DataRow {
    std::vector<DataCell> cells;
    bool selected = false;
    std::function<void(bool?)> onSelectChanged = nullptr;
    std::function<void()> onLongPress = nullptr;
    Color color = Color(0, 0, 0, 0);
    
    DataRow(std::vector<DataCell> c) : cells(std::move(c)) {}
};

using صف_بيانات = DataRow;

/**
 * @brief (AR) جدول بيانات Material
 * @brief (EN) Material data table
 */
class DataTable : public StatelessWidget {
public:
    DataTable(std::vector<DataColumn> columns,
              std::vector<DataRow> rows,
              int sortColumnIndex = -1,
              bool sortAscending = true,
              std::function<void(bool?)> onSelectAll = nullptr,
              float dataRowHeight = 48.0f,
              float headingRowHeight = 56.0f,
              float horizontalMargin = 24.0f,
              float columnSpacing = 56.0f,
              bool showCheckboxColumn = true,
              bool showBottomBorder = true,
              Color headingRowColor = Color(250, 250, 250),
              Color dataRowColor = Color(255, 255, 255))
        : columns_(std::move(columns)), rows_(std::move(rows)),
          sortColumnIndex_(sortColumnIndex), sortAscending_(sortAscending),
          onSelectAll_(onSelectAll), dataRowHeight_(dataRowHeight),
          headingRowHeight_(headingRowHeight), horizontalMargin_(horizontalMargin),
          columnSpacing_(columnSpacing), showCheckboxColumn_(showCheckboxColumn),
          showBottomBorder_(showBottomBorder), headingRowColor_(headingRowColor),
          dataRowColor_(dataRowColor) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    std::vector<DataColumn> columns_;
    std::vector<DataRow> rows_;
    int sortColumnIndex_;
    bool sortAscending_;
    std::function<void(bool?)> onSelectAll_;
    float dataRowHeight_;
    float headingRowHeight_;
    float horizontalMargin_;
    float columnSpacing_;
    bool showCheckboxColumn_;
    bool showBottomBorder_;
    Color headingRowColor_;
    Color dataRowColor_;
};

using جدول_بيانات = DataTable;

// ═══════════════════════════════════════════════════════════════════════════════
//  Stepper — خطوات متتابعة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) حالة الخطوة
 * @brief (EN) Step state
 */
enum class StepState {
    Indexed,     // رقم فقط
    Editing,     // تحرير
    Complete,    // مكتمل
    Disabled,    // معطل
    Error        // خطأ
};

using حالة_خطوة = StepState;

/**
 * @brief (AR) خطوة في Stepper
 * @brief (EN) Step in a Stepper
 */
struct Step {
    WidgetPtr title;
    WidgetPtr subtitle = nullptr;
    WidgetPtr content;
    StepState state = StepState::Indexed;
    bool isActive = false;
    WidgetPtr label = nullptr;
};

using خطوة = Step;

/**
 * @brief (AR) نوع Stepper
 * @brief (EN) Stepper type
 */
enum class StepperType {
    Vertical,    // عمودي
    Horizontal   // أفقي
};

using نوع_خطوات = StepperType;

/**
 * @brief (AR) خطوات متتابعة
 * @brief (EN) Material stepper widget
 */
class Stepper : public StatefulWidget {
public:
    Stepper(std::vector<Step> steps,
            StepperType type = StepperType::Vertical,
            int currentStep = 0,
            std::function<void(int)> onStepTapped = nullptr,
            std::function<void()> onStepContinue = nullptr,
            std::function<void()> onStepCancel = nullptr,
            std::function<WidgetPtr(BuildContext*, ControlDetails)> controlsBuilder = nullptr,
            float elevation = 2.0f)
        : steps_(std::move(steps)), type_(type), currentStep_(currentStep),
          onStepTapped_(onStepTapped), onStepContinue_(onStepContinue),
          onStepCancel_(onStepCancel), controlsBuilder_(controlsBuilder),
          elevation_(elevation) {}
    
    std::unique_ptr<State_Base> createState() override;
    
    struct ControlDetails {
        int stepIndex;
        int currentStep;
        std::function<void()> onStepContinue;
        std::function<void()> onStepCancel;
        bool isActive;
    };
    
    std::vector<Step> steps_;
    StepperType type_;
    int currentStep_;
    std::function<void(int)> onStepTapped_;
    std::function<void()> onStepContinue_;
    std::function<void()> onStepCancel_;
    std::function<WidgetPtr(BuildContext*, ControlDetails)> controlsBuilder_;
    float elevation_;
};

using خطوات = Stepper;

// ═══════════════════════════════════════════════════════════════════════════════
//  ExpansionPanel — لوحة قابلة للتوسع
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) لوحة قابلة للتوسع
 * @brief (EN) Expansion panel
 */
class ExpansionPanel : public StatelessWidget {
public:
    ExpansionPanel(WidgetPtr headerBuilder,
                   WidgetPtr body,
                   bool isExpanded = false,
                   std::function<void(bool)> expansionCallback = nullptr,
                   Color backgroundColor = Color(255, 255, 255),
                   bool canTapOnHeader = true)
        : headerBuilder_(headerBuilder), body_(body), isExpanded_(isExpanded),
          expansionCallback_(expansionCallback), backgroundColor_(backgroundColor),
          canTapOnHeader_(canTapOnHeader) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
    WidgetPtr headerBuilder_;
    WidgetPtr body_;
    bool isExpanded_;
    std::function<void(bool)> expansionCallback_;
    Color backgroundColor_;
    bool canTapOnHeader_;
};

using لوحة_توسع = ExpansionPanel;

/**
 * @brief (AR) قائمة لوحات قابلة للتوسع
 * @brief (EN) List of expansion panels
 */
class ExpansionPanelList : public StatefulWidget {
public:
    ExpansionPanelList(std::vector<std::shared_ptr<ExpansionPanel>> children,
                       std::function<void(int, bool)> expansionCallback = nullptr,
                       Duration animationDuration = Duration::milliseconds(200),
                       float elevation = 2.0f,
                       EdgeInsets expandedHeaderPadding = EdgeInsets::symmetric(0, 16),
                       bool allowMultipleOpen = false)
        : children_(std::move(children)), expansionCallback_(expansionCallback),
          animationDuration_(animationDuration), elevation_(elevation),
          expandedHeaderPadding_(expandedHeaderPadding),
          allowMultipleOpen_(allowMultipleOpen) {}
    
    std::unique_ptr<State_Base> createState() override;
    
    std::vector<std::shared_ptr<ExpansionPanel>> children_;
    std::function<void(int, bool)> expansionCallback_;
    Duration animationDuration_;
    float elevation_;
    EdgeInsets expandedHeaderPadding_;
    bool allowMultipleOpen_;
};

using قائمة_لوحات_توسع = ExpansionPanelList;

// ═══════════════════════════════════════════════════════════════════════════════
//  ListTile — عنصر قائمة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) عنصر قائمة Material
 * @brief (EN) Material list tile
 */
class ListTile : public StatelessWidget {
public:
    ListTile(WidgetPtr title,
             WidgetPtr subtitle = nullptr,
             WidgetPtr leading = nullptr,
             WidgetPtr trailing = nullptr,
             std::function<void()> onTap = nullptr,
             std::function<void()> onLongPress = nullptr,
             bool isThreeLine = false,
             bool dense = false,
             EdgeInsets contentPadding = EdgeInsets::symmetric(16, 0),
             bool enabled = true,
             bool selected = false,
             Color selectedColor = Color(100, 149, 237),
             Color selectedTileColor = Color(240, 248, 255),
             Color tileColor = Color(0, 0, 0, 0))
        : title_(title), subtitle_(subtitle), leading_(leading), trailing_(trailing),
          onTap_(onTap), onLongPress_(onLongPress), isThreeLine_(isThreeLine),
          dense_(dense), contentPadding_(contentPadding), enabled_(enabled),
          selected_(selected), selectedColor_(selectedColor),
          selectedTileColor_(selectedTileColor), tileColor_(tileColor) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    WidgetPtr title_;
    WidgetPtr subtitle_;
    WidgetPtr leading_;
    WidgetPtr trailing_;
    std::function<void()> onTap_;
    std::function<void()> onLongPress_;
    bool isThreeLine_;
    bool dense_;
    EdgeInsets contentPadding_;
    bool enabled_;
    bool selected_;
    Color selectedColor_;
    Color selectedTileColor_;
    Color tileColor_;
};

using عنصر_قائمة = ListTile;

// ═══════════════════════════════════════════════════════════════════════════════
//  CircleAvatar — صورة دائرية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) صورة دائرية للمستخدم
 * @brief (EN) Circular avatar
 */
class CircleAvatar : public StatelessWidget {
public:
    CircleAvatar(WidgetPtr child = nullptr,
                 Color backgroundColor = Color(100, 149, 237),
                 Color foregroundColor = Color(255, 255, 255),
                 std::string backgroundImage = "",
                 std::string foregroundImage = "",
                 float radius = 20.0f,
                 float minRadius = 0.0f,
                 float maxRadius = std::numeric_limits<float>::infinity())
        : child_(child), backgroundColor_(backgroundColor),
          foregroundColor_(foregroundColor), backgroundImage_(backgroundImage),
          foregroundImage_(foregroundImage), radius_(radius),
          minRadius_(minRadius), maxRadius_(maxRadius) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    WidgetPtr child_;
    Color backgroundColor_;
    Color foregroundColor_;
    std::string backgroundImage_;
    std::string foregroundImage_;
    float radius_;
    float minRadius_;
    float maxRadius_;
};

using صورة_دائرية = CircleAvatar;

// ═══════════════════════════════════════════════════════════════════════════════
//  Drawer — الدرج الجانبي
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) درج جانبي
 * @brief (EN) Navigation drawer
 */
class Drawer : public StatelessWidget {
public:
    Drawer(WidgetPtr child,
           Color backgroundColor = Color(255, 255, 255),
           float elevation = 16.0f,
           ShapeBorder shape = ShapeBorder{},
           float width = 304.0f)
        : child_(child), backgroundColor_(backgroundColor),
          elevation_(elevation), shape_(shape), width_(width) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    WidgetPtr child_;
    Color backgroundColor_;
    float elevation_;
    ShapeBorder shape_;
    float width_;
};

using درج = Drawer;

/**
 * @brief (AR) رأس الدرج
 * @brief (EN) Drawer header
 */
class DrawerHeader : public StatelessWidget {
public:
    DrawerHeader(WidgetPtr child,
                 EdgeInsets padding = EdgeInsets::fromLTRB(16, 16, 16, 8),
                 EdgeInsets margin = EdgeInsets::only(0, 0, 0, 8),
                 Color decoration_color = Color(100, 149, 237),
                 Duration duration = Duration::milliseconds(250))
        : child_(child), padding_(padding), margin_(margin),
          decorationColor_(decoration_color), duration_(duration) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    WidgetPtr child_;
    EdgeInsets padding_;
    EdgeInsets margin_;
    Color decorationColor_;
    Duration duration_;
};

using رأس_درج = DrawerHeader;

/**
 * @brief (AR) رأس درج حساب المستخدم
 * @brief (EN) User account drawer header
 */
class UserAccountsDrawerHeader : public StatelessWidget {
public:
    UserAccountsDrawerHeader(WidgetPtr accountName,
                             WidgetPtr accountEmail,
                             WidgetPtr currentAccountPicture = nullptr,
                             std::vector<WidgetPtr> otherAccountsPictures = {},
                             std::function<void()> onDetailsPressed = nullptr,
                             Color decoration_color = Color(100, 149, 237),
                             EdgeInsets margin = EdgeInsets::only(0, 0, 0, 8))
        : accountName_(accountName), accountEmail_(accountEmail),
          currentAccountPicture_(currentAccountPicture),
          otherAccountsPictures_(std::move(otherAccountsPictures)),
          onDetailsPressed_(onDetailsPressed), decorationColor_(decoration_color),
          margin_(margin) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    WidgetPtr accountName_;
    WidgetPtr accountEmail_;
    WidgetPtr currentAccountPicture_;
    std::vector<WidgetPtr> otherAccountsPictures_;
    std::function<void()> onDetailsPressed_;
    Color decorationColor_;
    EdgeInsets margin_;
};

using رأس_درج_حساب = UserAccountsDrawerHeader;

// ═══════════════════════════════════════════════════════════════════════════════
//  Progress Indicators — مؤشرات التقدم
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مؤشر تقدم خطي
 * @brief (EN) Linear progress indicator
 */
class LinearProgressIndicator : public StatelessWidget {
public:
    LinearProgressIndicator(std::optional<double> value = std::nullopt,
                            Color backgroundColor = Color(200, 200, 200),
                            Color color = Color(100, 149, 237),
                            float minHeight = 4.0f,
                            std::string semanticsLabel = "",
                            std::string semanticsValue = "")
        : value_(value), backgroundColor_(backgroundColor), color_(color),
          minHeight_(minHeight), semanticsLabel_(semanticsLabel),
          semanticsValue_(semanticsValue) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    std::optional<double> value_;  // nullopt = indeterminate
    Color backgroundColor_;
    Color color_;
    float minHeight_;
    std::string semanticsLabel_;
    std::string semanticsValue_;
};

using مؤشر_تقدم_خطي = LinearProgressIndicator;

/**
 * @brief (AR) مؤشر تقدم دائري
 * @brief (EN) Circular progress indicator
 */
class CircularProgressIndicator : public StatelessWidget {
public:
    CircularProgressIndicator(std::optional<double> value = std::nullopt,
                              Color backgroundColor = Color(0, 0, 0, 0),
                              Color color = Color(100, 149, 237),
                              float strokeWidth = 4.0f,
                              std::string semanticsLabel = "",
                              std::string semanticsValue = "")
        : value_(value), backgroundColor_(backgroundColor), color_(color),
          strokeWidth_(strokeWidth), semanticsLabel_(semanticsLabel),
          semanticsValue_(semanticsValue) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    std::optional<double> value_;  // nullopt = indeterminate
    Color backgroundColor_;
    Color color_;
    float strokeWidth_;
    std::string semanticsLabel_;
    std::string semanticsValue_;
};

using مؤشر_تقدم_دائري = CircularProgressIndicator;

// ═══════════════════════════════════════════════════════════════════════════════
//  Divider — فاصل
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) فاصل أفقي
 * @brief (EN) Horizontal divider
 */
class Divider : public StatelessWidget {
public:
    Divider(float height = 16.0f,
            float thickness = 1.0f,
            float indent = 0.0f,
            float endIndent = 0.0f,
            Color color = Color(0, 0, 0, 31))
        : height_(height), thickness_(thickness), indent_(indent),
          endIndent_(endIndent), color_(color) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    float height_;
    float thickness_;
    float indent_;
    float endIndent_;
    Color color_;
};

using فاصل = Divider;

/**
 * @brief (AR) فاصل عمودي
 * @brief (EN) Vertical divider
 */
class VerticalDivider : public StatelessWidget {
public:
    VerticalDivider(float width = 16.0f,
                    float thickness = 1.0f,
                    float indent = 0.0f,
                    float endIndent = 0.0f,
                    Color color = Color(0, 0, 0, 31))
        : width_(width), thickness_(thickness), indent_(indent),
          endIndent_(endIndent), color_(color) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    float width_;
    float thickness_;
    float indent_;
    float endIndent_;
    Color color_;
};

using فاصل_عمودي = VerticalDivider;

// ═══════════════════════════════════════════════════════════════════════════════
//  Badge — شارة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) شارة إشعار
 * @brief (EN) Notification badge
 */
class Badge : public StatelessWidget {
public:
    Badge(WidgetPtr child = nullptr,
          WidgetPtr label = nullptr,
          Color backgroundColor = Color(244, 67, 54),
          Color textColor = Color(255, 255, 255),
          float smallSize = 6.0f,
          float largeSize = 16.0f,
          bool isLabelVisible = true,
          AlignmentGeometry alignment = AlignmentGeometry{1.0f, -1.0f},
          EdgeInsets padding = EdgeInsets::symmetric(4, 0))
        : child_(child), label_(label), backgroundColor_(backgroundColor),
          textColor_(textColor), smallSize_(smallSize), largeSize_(largeSize),
          isLabelVisible_(isLabelVisible), alignment_(alignment), padding_(padding) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    WidgetPtr child_;
    WidgetPtr label_;
    Color backgroundColor_;
    Color textColor_;
    float smallSize_;
    float largeSize_;
    bool isLabelVisible_;
    AlignmentGeometry alignment_;
    EdgeInsets padding_;
};

using شارة = Badge;

// ═══════════════════════════════════════════════════════════════════════════════
//  Banner — لافتة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) موقع المحتوى في اللافتة
 * @brief (EN) Banner content position
 */
enum class BannerContentPosition {
    Top,
    Bottom
};

/**
 * @brief (AR) لافتة Material
 * @brief (EN) Material banner
 */
class MaterialBanner : public StatelessWidget {
public:
    MaterialBanner(WidgetPtr content,
                   std::vector<WidgetPtr> actions,
                   WidgetPtr leading = nullptr,
                   EdgeInsets padding = EdgeInsets::all(16),
                   EdgeInsets leadingPadding = EdgeInsets::only(0, 0, 16, 0),
                   bool forceActionsBelow = false,
                   Color backgroundColor = Color(255, 255, 255),
                   float elevation = 0.0f)
        : content_(content), actions_(std::move(actions)), leading_(leading),
          padding_(padding), leadingPadding_(leadingPadding),
          forceActionsBelow_(forceActionsBelow), backgroundColor_(backgroundColor),
          elevation_(elevation) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    WidgetPtr content_;
    std::vector<WidgetPtr> actions_;
    WidgetPtr leading_;
    EdgeInsets padding_;
    EdgeInsets leadingPadding_;
    bool forceActionsBelow_;
    Color backgroundColor_;
    float elevation_;
};

using لافتة = MaterialBanner;

// ═══════════════════════════════════════════════════════════════════════════════
//  RefreshIndicator — مؤشر التحديث
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مؤشر تحديث بالسحب
 * @brief (EN) Pull-to-refresh indicator
 */
class RefreshIndicator : public StatefulWidget {
public:
    RefreshIndicator(WidgetPtr child,
                     std::function<void()> onRefresh,
                     Color color = Color(100, 149, 237),
                     Color backgroundColor = Color(255, 255, 255),
                     float displacement = 40.0f,
                     float edgeOffset = 0.0f,
                     float strokeWidth = 2.5f)
        : child_(child), onRefresh_(onRefresh), color_(color),
          backgroundColor_(backgroundColor), displacement_(displacement),
          edgeOffset_(edgeOffset), strokeWidth_(strokeWidth) {}
    
    std::unique_ptr<State_Base> createState() override;
    
    WidgetPtr child_;
    std::function<void()> onRefresh_;
    Color color_;
    Color backgroundColor_;
    float displacement_;
    float edgeOffset_;
    float strokeWidth_;
};

using مؤشر_تحديث = RefreshIndicator;

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إنشاء بطاقة معلومات
 * @brief (EN) Create info card
 */
inline WidgetPtr انشاء_بطاقة_معلومات(const std::string& عنوان,
                                     const std::string& محتوى,
                                     WidgetPtr ايقونة = nullptr) {
    return std::make_shared<Card>(
        std::make_shared<Padding>(
            std::make_shared<Column>(
                std::vector<WidgetPtr>{
                    std::make_shared<Row>(
                        std::vector<WidgetPtr>{
                            ايقونة ? ايقونة : std::make_shared<SizedBox>(),
                            std::make_shared<SizedBox>(8.0f, std::nullopt),
                            std::make_shared<Text>(عنوان, TextStyle{.fontSize = 18, .fontWeight = FontWeight::Bold})
                        }
                    ),
                    std::make_shared<SizedBox>(std::nullopt, 8.0f),
                    std::make_shared<Text>(محتوى)
                },
                MainAxisAlignment::Start,
                MainAxisSize::Min,
                CrossAxisAlignment::Start
            ),
            EdgeInsets::all(16)
        ),
        Color(255, 255, 255),
        2.0f
    );
}

/**
 * @brief (AR) إنشاء قائمة خيارات
 * @brief (EN) Create options list
 */
inline WidgetPtr انشاء_قائمة_خيارات(
    const std::vector<std::tuple<std::string, WidgetPtr, std::function<void()>>>& خيارات) {
    
    std::vector<WidgetPtr> عناصر;
    
    for (const auto& [عنوان, ايقونة, معالج] : خيارات) {
        عناصر.push_back(
            std::make_shared<ListTile>(
                std::make_shared<Text>(عنوان),
                nullptr,
                ايقونة,
                std::make_shared<Icon>(IconData{"chevron_right", 0xE5CC}),
                معالج
            )
        );
        عناصر.push_back(std::make_shared<Divider>());
    }
    
    if (!عناصر.empty()) {
        عناصر.pop_back();  // إزالة آخر فاصل
    }
    
    return std::make_shared<Card>(
        std::make_shared<Column>(std::move(عناصر))
    );
}

} // namespace flutter
} // namespace sad

#endif // SAD_FLUTTER_MATERIAL_H
