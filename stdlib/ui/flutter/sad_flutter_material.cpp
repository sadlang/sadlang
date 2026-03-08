// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_material.cpp — تطبيق مكونات Material Design
//  Material Design Components Implementation
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_flutter_material.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Card
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr Card::build(BuildContext* ctx) {
    BoxDecoration decoration;
    decoration.setColor(color_);
    decoration.setBorderRadius(shape_.borderRadius);
    // TODO: تطبيق الظل حسب elevation_
    
    return std::make_shared<Padding>(
        std::make_shared<Container>(
            child_,
            std::nullopt, std::nullopt,
            EdgeInsets{}, EdgeInsets{},
            decoration
        ),
        margin_
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Chip
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr Chip::build(BuildContext* ctx) {
    std::vector<WidgetPtr> rowChildren;
    
    if (avatar_) {
        rowChildren.push_back(avatar_);
        rowChildren.push_back(std::make_shared<SizedBox>(8.0f, std::nullopt));
    }
    
    rowChildren.push_back(
        std::make_shared<Padding>(label_, labelPadding_)
    );
    
    if (deleteIcon_ && onDeleted_) {
        rowChildren.push_back(std::make_shared<SizedBox>(4.0f, std::nullopt));
        rowChildren.push_back(
            std::make_shared<GestureDetector>(
                deleteIcon_,
                onDeleted_
            )
        );
    }
    
    BoxDecoration decoration;
    decoration.setColor(backgroundColor_);
    decoration.setBorderRadius(BorderRadius::all(16));
    
    return std::make_shared<Container>(
        std::make_shared<Row>(
            std::move(rowChildren),
            MainAxisAlignment::Center,
            MainAxisSize::Min
        ),
        std::nullopt, 32.0f,
        padding_,
        EdgeInsets{},
        decoration
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  InputChip
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr InputChip::build(BuildContext* ctx) {
    Color bgColor = selected_ ? selectedColor_ : backgroundColor_;
    
    std::vector<WidgetPtr> rowChildren;
    
    if (avatar_) {
        rowChildren.push_back(avatar_);
        rowChildren.push_back(std::make_shared<SizedBox>(8.0f, std::nullopt));
    }
    
    rowChildren.push_back(
        std::make_shared<Padding>(label_, labelPadding_)
    );
    
    if (onDeleted_) {
        rowChildren.push_back(std::make_shared<SizedBox>(4.0f, std::nullopt));
        rowChildren.push_back(
            std::make_shared<GestureDetector>(
                std::make_shared<Icon>(IconData{"cancel", 0xE5C9}, 18.0f),
                onDeleted_
            )
        );
    }
    
    BoxDecoration decoration;
    decoration.setColor(bgColor);
    decoration.setBorderRadius(BorderRadius::all(16));
    
    return std::make_shared<GestureDetector>(
        std::make_shared<Container>(
            std::make_shared<Row>(
                std::move(rowChildren),
                MainAxisAlignment::Center,
                MainAxisSize::Min
            ),
            std::nullopt, 32.0f,
            padding_,
            EdgeInsets{},
            decoration
        ),
        [this]{
            if (onPressed_) onPressed_();
            if (onSelected_) onSelected_(!selected_);
        }
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  FilterChip
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr FilterChip::build(BuildContext* ctx) {
    Color bgColor = selected_ ? selectedColor_ : backgroundColor_;
    
    std::vector<WidgetPtr> rowChildren;
    
    if (selected_ && showCheckmark_) {
        rowChildren.push_back(
            std::make_shared<Icon>(IconData{"check", 0xE5CA}, 18.0f, checkmarkColor_)
        );
        rowChildren.push_back(std::make_shared<SizedBox>(4.0f, std::nullopt));
    } else if (avatar_) {
        rowChildren.push_back(avatar_);
        rowChildren.push_back(std::make_shared<SizedBox>(4.0f, std::nullopt));
    }
    
    rowChildren.push_back(label_);
    
    BoxDecoration decoration;
    decoration.setColor(bgColor);
    decoration.setBorderRadius(BorderRadius::all(16));
    
    return std::make_shared<GestureDetector>(
        std::make_shared<Container>(
            std::make_shared<Row>(
                std::move(rowChildren),
                MainAxisAlignment::Center,
                MainAxisSize::Min
            ),
            std::nullopt, 32.0f,
            EdgeInsets::symmetric(12, 4),
            EdgeInsets{},
            decoration
        ),
        [this]{ if (onSelected_) onSelected_(!selected_); }
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ChoiceChip
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr ChoiceChip::build(BuildContext* ctx) {
    Color bgColor = selected_ ? selectedColor_ : backgroundColor_;
    
    std::vector<WidgetPtr> rowChildren;
    
    if (avatar_) {
        rowChildren.push_back(avatar_);
        rowChildren.push_back(std::make_shared<SizedBox>(8.0f, std::nullopt));
    }
    
    rowChildren.push_back(label_);
    
    BoxDecoration decoration;
    decoration.setColor(bgColor);
    decoration.setBorderRadius(BorderRadius::all(16));
    
    return std::make_shared<GestureDetector>(
        std::make_shared<Container>(
            std::make_shared<Row>(
                std::move(rowChildren),
                MainAxisAlignment::Center,
                MainAxisSize::Min
            ),
            std::nullopt, 32.0f,
            EdgeInsets::symmetric(12, 4),
            EdgeInsets{},
            decoration
        ),
        [this]{ if (onSelected_) onSelected_(!selected_); }
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ActionChip
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr ActionChip::build(BuildContext* ctx) {
    std::vector<WidgetPtr> rowChildren;
    
    if (avatar_) {
        rowChildren.push_back(avatar_);
        rowChildren.push_back(std::make_shared<SizedBox>(8.0f, std::nullopt));
    }
    
    rowChildren.push_back(label_);
    
    BoxDecoration decoration;
    decoration.setColor(backgroundColor_);
    decoration.setBorderRadius(BorderRadius::all(16));
    
    return std::make_shared<GestureDetector>(
        std::make_shared<Container>(
            std::make_shared<Row>(
                std::move(rowChildren),
                MainAxisAlignment::Center,
                MainAxisSize::Min
            ),
            std::nullopt, 32.0f,
            EdgeInsets::symmetric(12, 4),
            EdgeInsets{},
            decoration
        ),
        onPressed_
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  DataTable
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr DataTable::build(BuildContext* ctx) {
    std::vector<WidgetPtr> tableRows;
    
    // صف العناوين
    std::vector<WidgetPtr> headerCells;
    
    if (showCheckboxColumn_) {
        // TODO: Checkbox للتحديد الكلي
        headerCells.push_back(
            std::make_shared<Container>(
                std::make_shared<Center>(
                    std::make_shared<Checkbox>(false, [this](bool? v){
                        if (onSelectAll_) onSelectAll_(v);
                    })
                ),
                48.0f, headingRowHeight_
            )
        );
    }
    
    for (size_t i = 0; i < columns_.size(); ++i) {
        // TODO: تطبيق الترتيب
        headerCells.push_back(
            std::make_shared<Container>(
                std::make_shared<Padding>(
                    columns_[i].label,
                    EdgeInsets::symmetric(columnSpacing_ / 2, 0)
                ),
                std::nullopt, headingRowHeight_
            )
        );
    }
    
    BoxDecoration headerDecoration;
    headerDecoration.setColor(headingRowColor_);
    
    tableRows.push_back(
        std::make_shared<Container>(
            std::make_shared<Row>(std::move(headerCells)),
            std::nullopt, headingRowHeight_,
            EdgeInsets::symmetric(horizontalMargin_, 0),
            EdgeInsets{},
            headerDecoration
        )
    );
    
    // صفوف البيانات
    for (size_t rowIndex = 0; rowIndex < rows_.size(); ++rowIndex) {
        const auto& row = rows_[rowIndex];
        std::vector<WidgetPtr> rowCells;
        
        if (showCheckboxColumn_) {
            size_t idx = rowIndex;
            rowCells.push_back(
                std::make_shared<Container>(
                    std::make_shared<Center>(
                        std::make_shared<Checkbox>(row.selected, [&row](bool? v){
                            if (row.onSelectChanged) row.onSelectChanged(v);
                        })
                    ),
                    48.0f, dataRowHeight_
                )
            );
        }
        
        for (const auto& cell : row.cells) {
            rowCells.push_back(
                std::make_shared<GestureDetector>(
                    std::make_shared<Container>(
                        std::make_shared<Padding>(
                            cell.child,
                            EdgeInsets::symmetric(columnSpacing_ / 2, 0)
                        ),
                        std::nullopt, dataRowHeight_
                    ),
                    cell.onTap
                )
            );
        }
        
        Color rowColor = row.color.a > 0 ? row.color : dataRowColor_;
        BoxDecoration rowDecoration;
        rowDecoration.setColor(rowColor);
        
        std::vector<WidgetPtr> rowWithBorder;
        rowWithBorder.push_back(std::make_shared<Row>(std::move(rowCells)));
        
        if (showBottomBorder_) {
            rowWithBorder.push_back(
                std::make_shared<Container>(
                    nullptr,
                    std::nullopt, 1.0f,
                    EdgeInsets{}, EdgeInsets{},
                    BoxDecoration().setColor(Color(0, 0, 0, 31))
                )
            );
        }
        
        tableRows.push_back(
            std::make_shared<Container>(
                std::make_shared<Column>(std::move(rowWithBorder)),
                std::nullopt, std::nullopt,
                EdgeInsets::symmetric(horizontalMargin_, 0),
                EdgeInsets{},
                rowDecoration
            )
        );
    }
    
    return std::make_shared<Column>(std::move(tableRows));
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Stepper State
// ═══════════════════════════════════════════════════════════════════════════════

class StepperState : public State<Stepper> {
public:
    void initState() override {
        currentStep_ = widget()->currentStep_;
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        std::vector<WidgetPtr> stepWidgets;
        
        for (size_t i = 0; i < w->steps_.size(); ++i) {
            const auto& step = w->steps_[i];
            bool isActive = (static_cast<int>(i) == currentStep_);
            
            // دائرة الرقم أو الأيقونة
            WidgetPtr stepCircle;
            if (step.state == StepState::Complete) {
                stepCircle = std::make_shared<CircleAvatar>(
                    std::make_shared<Icon>(IconData{"check", 0xE5CA}, 16.0f, Color(255, 255, 255)),
                    Color(76, 175, 80),
                    Color(255, 255, 255),
                    "", "", 12.0f
                );
            } else if (step.state == StepState::Error) {
                stepCircle = std::make_shared<CircleAvatar>(
                    std::make_shared<Text>("!", TextStyle{.fontSize = 14, .color = Color(255, 255, 255)}),
                    Color(244, 67, 54),
                    Color(255, 255, 255),
                    "", "", 12.0f
                );
            } else {
                Color circleColor = isActive ? Color(100, 149, 237) : Color(158, 158, 158);
                stepCircle = std::make_shared<CircleAvatar>(
                    std::make_shared<Text>(std::to_string(i + 1), TextStyle{.fontSize = 12, .color = Color(255, 255, 255)}),
                    circleColor,
                    Color(255, 255, 255),
                    "", "", 12.0f
                );
            }
            
            int index = static_cast<int>(i);
            
            if (w->type_ == StepperType::Vertical) {
                std::vector<WidgetPtr> stepColumn;
                
                // رأس الخطوة
                stepColumn.push_back(
                    std::make_shared<GestureDetector>(
                        std::make_shared<Row>(
                            std::vector<WidgetPtr>{
                                stepCircle,
                                std::make_shared<SizedBox>(12.0f, std::nullopt),
                                std::make_shared<Column>(
                                    std::vector<WidgetPtr>{
                                        step.title,
                                        step.subtitle ? step.subtitle : std::make_shared<SizedBox>()
                                    },
                                    MainAxisAlignment::Center,
                                    MainAxisSize::Min,
                                    CrossAxisAlignment::Start
                                )
                            }
                        ),
                        [this, index]{ 
                            if (widget()->onStepTapped_) {
                                widget()->onStepTapped_(index);
                            }
                            currentStep_ = index;
                            setState([]{});
                        }
                    )
                );
                
                // محتوى الخطوة (إذا كانت نشطة)
                if (isActive) {
                    stepColumn.push_back(
                        std::make_shared<Padding>(
                            step.content,
                            EdgeInsets::only(36, 16, 0, 0)
                        )
                    );
                    
                    // أزرار التحكم
                    if (!w->controlsBuilder_) {
                        stepColumn.push_back(
                            std::make_shared<Padding>(
                                std::make_shared<Row>(
                                    std::vector<WidgetPtr>{
                                        std::make_shared<ElevatedButton>(
                                            std::make_shared<Text>("متابعة"),
                                            [this]{
                                                if (widget()->onStepContinue_) {
                                                    widget()->onStepContinue_();
                                                }
                                                if (currentStep_ < static_cast<int>(widget()->steps_.size()) - 1) {
                                                    currentStep_++;
                                                    setState([]{});
                                                }
                                            }
                                        ),
                                        std::make_shared<SizedBox>(8.0f, std::nullopt),
                                        std::make_shared<TextButton>(
                                            std::make_shared<Text>("إلغاء"),
                                            [this]{
                                                if (widget()->onStepCancel_) {
                                                    widget()->onStepCancel_();
                                                }
                                                if (currentStep_ > 0) {
                                                    currentStep_--;
                                                    setState([]{});
                                                }
                                            }
                                        )
                                    }
                                ),
                                EdgeInsets::only(36, 16, 0, 16)
                            )
                        );
                    }
                }
                
                // خط الاتصال
                if (i < w->steps_.size() - 1) {
                    stepColumn.push_back(
                        std::make_shared<Padding>(
                            std::make_shared<Container>(
                                nullptr,
                                2.0f, 24.0f,
                                EdgeInsets{}, EdgeInsets{},
                                BoxDecoration().setColor(Color(189, 189, 189))
                            ),
                            EdgeInsets::only(11, 0, 0, 0)
                        )
                    );
                }
                
                stepWidgets.push_back(std::make_shared<Column>(std::move(stepColumn)));
            } else {
                // Horizontal stepper
                stepWidgets.push_back(
                    std::make_shared<GestureDetector>(
                        std::make_shared<Column>(
                            std::vector<WidgetPtr>{
                                stepCircle,
                                std::make_shared<SizedBox>(std::nullopt, 4.0f),
                                step.title
                            },
                            MainAxisAlignment::Center,
                            MainAxisSize::Min,
                            CrossAxisAlignment::Center
                        ),
                        [this, index]{ 
                            if (widget()->onStepTapped_) {
                                widget()->onStepTapped_(index);
                            }
                            currentStep_ = index;
                            setState([]{});
                        }
                    )
                );
                
                // خط الاتصال الأفقي
                if (i < w->steps_.size() - 1) {
                    stepWidgets.push_back(
                        std::make_shared<Expanded>(
                            std::make_shared<Container>(
                                nullptr,
                                std::nullopt, 2.0f,
                                EdgeInsets::symmetric(8, 0),
                                EdgeInsets{},
                                BoxDecoration().setColor(Color(189, 189, 189))
                            )
                        )
                    );
                }
            }
        }
        
        if (w->type_ == StepperType::Vertical) {
            return std::make_shared<Card>(
                std::make_shared<Padding>(
                    std::make_shared<Column>(std::move(stepWidgets)),
                    EdgeInsets::all(24)
                ),
                Color(255, 255, 255),
                w->elevation_
            );
        } else {
            return std::make_shared<Card>(
                std::make_shared<Row>(std::move(stepWidgets)),
                Color(255, 255, 255),
                w->elevation_
            );
        }
    }
    
private:
    int currentStep_ = 0;
    
    Stepper* widget() const {
        return static_cast<Stepper*>(widget_);
    }
};

std::unique_ptr<State_Base> Stepper::createState() {
    return std::make_unique<StepperState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ExpansionPanel
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr ExpansionPanel::build(BuildContext* ctx) {
    std::vector<WidgetPtr> panelChildren;
    
    // الرأس
    WidgetPtr header = canTapOnHeader_
        ? std::static_pointer_cast<Widget>(std::make_shared<GestureDetector>(
              headerBuilder_,
              [this]{ if (expansionCallback_) expansionCallback_(!isExpanded_); }
          ))
        : headerBuilder_;
    
    panelChildren.push_back(
        std::make_shared<Row>(
            std::vector<WidgetPtr>{
                std::make_shared<Expanded>(header),
                std::make_shared<Icon>(
                    isExpanded_ 
                        ? IconData{"expand_less", 0xE5CE}
                        : IconData{"expand_more", 0xE5CF},
                    24.0f
                )
            }
        )
    );
    
    // المحتوى (إذا كان موسعاً)
    if (isExpanded_) {
        panelChildren.push_back(body_);
    }
    
    BoxDecoration decoration;
    decoration.setColor(backgroundColor_);
    
    return std::make_shared<Container>(
        std::make_shared<Column>(std::move(panelChildren)),
        std::nullopt, std::nullopt,
        EdgeInsets::all(16),
        EdgeInsets{},
        decoration
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ExpansionPanelList State
// ═══════════════════════════════════════════════════════════════════════════════

class ExpansionPanelListState : public State<ExpansionPanelList> {
public:
    void initState() override {
        auto* w = widget();
        expandedPanels_.resize(w->children_.size(), false);
        for (size_t i = 0; i < w->children_.size(); ++i) {
            expandedPanels_[i] = w->children_[i]->isExpanded_;
        }
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        std::vector<WidgetPtr> panels;
        
        for (size_t i = 0; i < w->children_.size(); ++i) {
            auto panel = w->children_[i];
            int index = static_cast<int>(i);
            
            auto modifiedPanel = std::make_shared<ExpansionPanel>(
                panel->headerBuilder_,
                panel->body_,
                expandedPanels_[i],
                [this, index](bool expanded){
                    if (!widget()->allowMultipleOpen_ && expanded) {
                        // إغلاق جميع اللوحات الأخرى
                        for (size_t j = 0; j < expandedPanels_.size(); ++j) {
                            if (static_cast<int>(j) != index) {
                                expandedPanels_[j] = false;
                            }
                        }
                    }
                    expandedPanels_[index] = expanded;
                    if (widget()->expansionCallback_) {
                        widget()->expansionCallback_(index, expanded);
                    }
                    setState([]{});
                },
                panel->backgroundColor_,
                panel->canTapOnHeader_
            );
            
            panels.push_back(modifiedPanel);
        }
        
        return std::make_shared<Card>(
            std::make_shared<Column>(std::move(panels)),
            Color(255, 255, 255),
            w->elevation_
        );
    }
    
private:
    std::vector<bool> expandedPanels_;
    
    ExpansionPanelList* widget() const {
        return static_cast<ExpansionPanelList*>(widget_);
    }
};

std::unique_ptr<State_Base> ExpansionPanelList::createState() {
    return std::make_unique<ExpansionPanelListState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ListTile
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr ListTile::build(BuildContext* ctx) {
    std::vector<WidgetPtr> rowChildren;
    
    if (leading_) {
        rowChildren.push_back(leading_);
        rowChildren.push_back(std::make_shared<SizedBox>(16.0f, std::nullopt));
    }
    
    // المحتوى الرئيسي
    std::vector<WidgetPtr> titleContent;
    titleContent.push_back(title_);
    if (subtitle_) {
        titleContent.push_back(std::make_shared<SizedBox>(std::nullopt, 4.0f));
        titleContent.push_back(subtitle_);
    }
    
    rowChildren.push_back(
        std::make_shared<Expanded>(
            std::make_shared<Column>(
                std::move(titleContent),
                MainAxisAlignment::Center,
                MainAxisSize::Min,
                CrossAxisAlignment::Start
            )
        )
    );
    
    if (trailing_) {
        rowChildren.push_back(std::make_shared<SizedBox>(16.0f, std::nullopt));
        rowChildren.push_back(trailing_);
    }
    
    Color bgColor = selected_ ? selectedTileColor_ : tileColor_;
    BoxDecoration decoration;
    if (bgColor.a > 0) {
        decoration.setColor(bgColor);
    }
    
    float height = dense_ ? 48.0f : (isThreeLine_ ? 88.0f : (subtitle_ ? 72.0f : 56.0f));
    
    WidgetPtr tile = std::make_shared<Container>(
        std::make_shared<Row>(std::move(rowChildren)),
        std::nullopt, height,
        contentPadding_,
        EdgeInsets{},
        decoration
    );
    
    if (enabled_ && (onTap_ || onLongPress_)) {
        return std::make_shared<GestureDetector>(
            tile,
            onTap_,
            nullptr, nullptr, // onTapDown, onTapUp
            onLongPress_
        );
    }
    
    return tile;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CircleAvatar
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr CircleAvatar::build(BuildContext* ctx) {
    float actualRadius = std::clamp(radius_, minRadius_, maxRadius_);
    
    BoxDecoration decoration;
    decoration.setColor(backgroundColor_);
    decoration.setBorderRadius(BorderRadius::all(actualRadius));
    // TODO: backgroundImage
    
    return std::make_shared<Container>(
        child_ ? std::make_shared<Center>(child_) : nullptr,
        actualRadius * 2, actualRadius * 2,
        EdgeInsets{}, EdgeInsets{},
        decoration
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Drawer
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr Drawer::build(BuildContext* ctx) {
    BoxDecoration decoration;
    decoration.setColor(backgroundColor_);
    decoration.setBorderRadius(shape_.borderRadius);
    // TODO: elevation shadow
    
    return std::make_shared<Container>(
        child_,
        width_, std::nullopt,
        EdgeInsets{}, EdgeInsets{},
        decoration
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  DrawerHeader
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr DrawerHeader::build(BuildContext* ctx) {
    BoxDecoration decoration;
    decoration.setColor(decorationColor_);
    
    return std::make_shared<Padding>(
        std::make_shared<Container>(
            std::make_shared<Padding>(child_, padding_),
            std::nullopt, 160.0f,
            EdgeInsets{}, EdgeInsets{},
            decoration
        ),
        margin_
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  UserAccountsDrawerHeader
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr UserAccountsDrawerHeader::build(BuildContext* ctx) {
    BoxDecoration decoration;
    decoration.setColor(decorationColor_);
    
    std::vector<WidgetPtr> headerContent;
    
    // صورة الحساب الحالي
    if (currentAccountPicture_) {
        headerContent.push_back(currentAccountPicture_);
    }
    
    // صور الحسابات الأخرى
    if (!otherAccountsPictures_.empty()) {
        std::vector<WidgetPtr> otherPics;
        for (size_t i = 0; i < std::min(otherAccountsPictures_.size(), size_t(3)); ++i) {
            otherPics.push_back(otherAccountsPictures_[i]);
            if (i < 2) {
                otherPics.push_back(std::make_shared<SizedBox>(8.0f, std::nullopt));
            }
        }
        headerContent.push_back(
            std::make_shared<Row>(std::move(otherPics))
        );
    }
    
    headerContent.push_back(std::make_shared<Spacer>());
    
    // معلومات الحساب
    headerContent.push_back(accountName_);
    headerContent.push_back(accountEmail_);
    
    return std::make_shared<Padding>(
        std::make_shared<Container>(
            std::make_shared<Padding>(
                std::make_shared<Column>(
                    std::move(headerContent),
                    MainAxisAlignment::End,
                    MainAxisSize::Max,
                    CrossAxisAlignment::Start
                ),
                EdgeInsets::all(16)
            ),
            std::nullopt, 160.0f,
            EdgeInsets{}, EdgeInsets{},
            decoration
        ),
        margin_
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  LinearProgressIndicator
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr LinearProgressIndicator::build(BuildContext* ctx) {
    BoxDecoration bgDecoration;
    bgDecoration.setColor(backgroundColor_);
    bgDecoration.setBorderRadius(BorderRadius::all(minHeight_ / 2));
    
    BoxDecoration fgDecoration;
    fgDecoration.setColor(color_);
    fgDecoration.setBorderRadius(BorderRadius::all(minHeight_ / 2));
    
    if (value_.has_value()) {
        // Determinate
        return std::make_shared<Container>(
            std::make_shared<Stack>(
                std::vector<WidgetPtr>{
                    // الخلفية
                    std::make_shared<Positioned>(
                        std::make_shared<Container>(
                            nullptr,
                            std::nullopt, minHeight_,
                            EdgeInsets{}, EdgeInsets{},
                            bgDecoration
                        ),
                        0.0f, 0.0f, 0.0f, 0.0f
                    ),
                    // التقدم
                    std::make_shared<FractionallySizedBox>(
                        std::make_shared<Container>(
                            nullptr,
                            std::nullopt, minHeight_,
                            EdgeInsets{}, EdgeInsets{},
                            fgDecoration
                        ),
                        value_.value(),
                        1.0
                    )
                }
            ),
            std::nullopt, minHeight_
        );
    } else {
        // Indeterminate - TODO: animation
        return std::make_shared<Container>(
            nullptr,
            std::nullopt, minHeight_,
            EdgeInsets{}, EdgeInsets{},
            bgDecoration
        );
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CircularProgressIndicator
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr CircularProgressIndicator::build(BuildContext* ctx) {
    // TODO: تطبيق الرسم الدائري الفعلي
    float size = 40.0f;
    
    if (value_.has_value()) {
        // Determinate - عرض النسبة
        int percent = static_cast<int>(value_.value() * 100);
        return std::make_shared<Container>(
            std::make_shared<Center>(
                std::make_shared<Text>(std::to_string(percent) + "%", TextStyle{.fontSize = 12, .color = color_})
            ),
            size, size
        );
    } else {
        // Indeterminate
        return std::make_shared<Container>(
            std::make_shared<Center>(
                std::make_shared<Text>("⟳", TextStyle{.fontSize = 24, .color = color_})
            ),
            size, size
        );
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Divider
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr Divider::build(BuildContext* ctx) {
    BoxDecoration decoration;
    decoration.setColor(color_);
    
    return std::make_shared<Container>(
        std::make_shared<Padding>(
            std::make_shared<Container>(
                nullptr,
                std::nullopt, thickness_,
                EdgeInsets{}, EdgeInsets{},
                decoration
            ),
            EdgeInsets::fromLTRB(indent_, 0, endIndent_, 0)
        ),
        std::nullopt, height_
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  VerticalDivider
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr VerticalDivider::build(BuildContext* ctx) {
    BoxDecoration decoration;
    decoration.setColor(color_);
    
    return std::make_shared<Container>(
        std::make_shared<Padding>(
            std::make_shared<Container>(
                nullptr,
                thickness_, std::nullopt,
                EdgeInsets{}, EdgeInsets{},
                decoration
            ),
            EdgeInsets::fromLTRB(0, indent_, 0, endIndent_)
        ),
        width_, std::nullopt
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Badge
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr Badge::build(BuildContext* ctx) {
    if (!child_ && !label_) {
        return std::make_shared<SizedBox>();
    }
    
    BoxDecoration badgeDecoration;
    badgeDecoration.setColor(backgroundColor_);
    
    float badgeSize = label_ ? largeSize_ : smallSize_;
    badgeDecoration.setBorderRadius(BorderRadius::all(badgeSize / 2));
    
    WidgetPtr badge = std::make_shared<Container>(
        label_ ? std::make_shared<Padding>(label_, padding_) : nullptr,
        label_ ? std::nullopt : std::optional(badgeSize),
        badgeSize,
        EdgeInsets{}, EdgeInsets{},
        badgeDecoration
    );
    
    if (!child_) {
        return badge;
    }
    
    if (!isLabelVisible_) {
        return child_;
    }
    
    return std::make_shared<Stack>(
        std::vector<WidgetPtr>{
            child_,
            std::make_shared<Positioned>(
                badge,
                alignment_.x > 0 ? std::nullopt : std::optional(0.0f),
                alignment_.y < 0 ? std::optional(0.0f) : std::nullopt,
                alignment_.x > 0 ? std::optional(0.0f) : std::nullopt,
                alignment_.y < 0 ? std::nullopt : std::optional(0.0f)
            )
        }
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  MaterialBanner
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr MaterialBanner::build(BuildContext* ctx) {
    std::vector<WidgetPtr> bannerChildren;
    
    std::vector<WidgetPtr> contentRow;
    
    if (leading_) {
        contentRow.push_back(
            std::make_shared<Padding>(leading_, leadingPadding_)
        );
    }
    
    contentRow.push_back(std::make_shared<Expanded>(content_));
    
    bannerChildren.push_back(
        std::make_shared<Row>(std::move(contentRow))
    );
    
    // الأزرار
    if (!actions_.empty()) {
        if (forceActionsBelow_) {
            std::vector<WidgetPtr> actionRow;
            for (const auto& action : actions_) {
                actionRow.push_back(action);
                actionRow.push_back(std::make_shared<SizedBox>(8.0f, std::nullopt));
            }
            if (!actionRow.empty()) actionRow.pop_back();
            
            bannerChildren.push_back(std::make_shared<SizedBox>(std::nullopt, 8.0f));
            bannerChildren.push_back(
                std::make_shared<Row>(
                    std::move(actionRow),
                    MainAxisAlignment::End
                )
            );
        } else {
            // في نفس السطر
            // TODO: تطبيق التخطيط المرن
        }
    }
    
    BoxDecoration decoration;
    decoration.setColor(backgroundColor_);
    
    return std::make_shared<Container>(
        std::make_shared<Column>(std::move(bannerChildren)),
        std::nullopt, std::nullopt,
        padding_,
        EdgeInsets{},
        decoration
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  RefreshIndicator State
// ═══════════════════════════════════════════════════════════════════════════════

class RefreshIndicatorState : public State<RefreshIndicator> {
public:
    void initState() override {
        isRefreshing_ = false;
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        std::vector<WidgetPtr> stackChildren;
        
        // المؤشر (مخفي عادة)
        if (isRefreshing_) {
            BoxDecoration indicatorBg;
            indicatorBg.setColor(w->backgroundColor_);
            indicatorBg.setBorderRadius(BorderRadius::all(20));
            
            stackChildren.push_back(
                std::make_shared<Positioned>(
                    std::make_shared<Container>(
                        std::make_shared<CircularProgressIndicator>(
                            std::nullopt,
                            Color(0, 0, 0, 0),
                            w->color_,
                            w->strokeWidth_
                        ),
                        40.0f, 40.0f,
                        EdgeInsets{}, EdgeInsets{},
                        indicatorBg
                    ),
                    std::nullopt,
                    w->displacement_
                )
            );
        }
        
        // المحتوى
        // TODO: تطبيق السحب لأسفل
        stackChildren.push_back(w->child_);
        
        return std::make_shared<Stack>(std::move(stackChildren));
    }
    
private:
    bool isRefreshing_ = false;
    
    RefreshIndicator* widget() const {
        return static_cast<RefreshIndicator*>(widget_);
    }
};

std::unique_ptr<State_Base> RefreshIndicator::createState() {
    return std::make_unique<RefreshIndicatorState>();
}

} // namespace flutter
} // namespace sad
