/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * SadViewFactory.java
 * مصنع عناصر واجهة أصلية بتصميم عربي مخصص
 * Native View Factory with Custom Arabic Design
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * يُستدعى من C++ عبر JNI مباشرة لإنشاء Android Views أصلية
 * بدون WebView وبدون وسيط JSON
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

package com.sad.app;

import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.drawable.GradientDrawable;
import android.graphics.drawable.RippleDrawable;
import android.graphics.drawable.ColorDrawable;
import android.content.res.ColorStateList;
import android.os.Build;
import android.text.InputType;
import android.text.TextUtils;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.*;

public class SadViewFactory {

    // ═══════════════════════════════════════════════════════════════════════════
    // ألوان التصميم العربي المخصص
    // ═══════════════════════════════════════════════════════════════════════════

    private static final int PRIMARY       = 0xFF00897B;  // Teal 600
    private static final int PRIMARY_DARK  = 0xFF00695C;  // Teal 800
    private static final int PRIMARY_LIGHT = 0xFF4DB6AC;  // Teal 300
    private static final int ACCENT        = 0xFFFFB300;  // Amber 600
    private static final int ACCENT_DARK   = 0xFFF57F17;  // Yellow 900
    private static final int BACKGROUND    = 0xFFF5F5F0;  // Warm off-white
    private static final int SURFACE       = 0xFFFFFFFF;  // White
    private static final int SURFACE_DIM   = 0xFFF0EDE8;  // Warm surface
    private static final int TEXT_PRIMARY  = 0xFF1A1A2E;  // Deep blue-black
    private static final int TEXT_SECONDARY = 0xFF6D6D80; // Muted gray
    private static final int BORDER_COLOR  = 0xFFD5D1CA;  // Warm border
    private static final int DIVIDER_COLOR = 0xFFE0DDD8;  // Warm divider
    private static final int ERROR_COLOR   = 0xFFD32F2F;  // Red 700
    private static final int SUCCESS_COLOR = 0xFF2E7D32;  // Green 800

    // أحجام (dp)
    private static final int CORNER_RADIUS = 14;
    private static final int CARD_ELEVATION = 4;
    private static final int PADDING_SM = 8;
    private static final int PADDING_MD = 16;
    private static final int PADDING_LG = 24;
    private static final int SPACING_DEFAULT = 10;

    // ═══════════════════════════════════════════════════════════════════════════
    // أكواد الأنواع — مُتطابقة مع UINodeType enum في C++
    // ═══════════════════════════════════════════════════════════════════════════

    // §1 عَرض
    private static final int TYPE_TEXT = 0;
    private static final int TYPE_IMAGE = 1;
    private static final int TYPE_ICON = 2;
    // §2 إدخال
    private static final int TYPE_BUTTON = 3;
    private static final int TYPE_TEXT_FIELD = 4;
    private static final int TYPE_TEXT_AREA = 5;
    private static final int TYPE_TOGGLE = 6;
    private static final int TYPE_SLIDER = 7;
    private static final int TYPE_PICKER = 8;
    private static final int TYPE_DATE_PICKER = 9;
    private static final int TYPE_COLOR_PICKER = 10;
    private static final int TYPE_CHECKBOX = 11;
    private static final int TYPE_RADIO = 12;
    // §3 تخطيط
    private static final int TYPE_COLUMN = 13;
    private static final int TYPE_ROW = 14;
    private static final int TYPE_STACK = 15;
    private static final int TYPE_GRID = 16;
    private static final int TYPE_LAZY_COLUMN = 17;
    private static final int TYPE_LAZY_ROW = 18;
    private static final int TYPE_LAZY_GRID = 19;
    private static final int TYPE_SPACER = 20;
    private static final int TYPE_DIVIDER = 21;
    private static final int TYPE_WRAP = 22;
    // §4 حاويات
    private static final int TYPE_CONTAINER = 23;
    private static final int TYPE_CARD = 24;
    private static final int TYPE_SURFACE = 25;
    private static final int TYPE_SCAFFOLD = 26;
    private static final int TYPE_BOX = 27;
    // §5 تنقل
    private static final int TYPE_NAV_VIEW = 28;
    private static final int TYPE_NAV_LINK = 29;
    private static final int TYPE_TAB_VIEW = 30;
    private static final int TYPE_TAB_ITEM = 31;
    // §6 قوائم
    private static final int TYPE_LIST = 32;
    private static final int TYPE_SECTION = 33;
    private static final int TYPE_FOR_EACH = 34;
    // §7 حوارات
    private static final int TYPE_ALERT = 35;
    private static final int TYPE_SHEET = 36;
    private static final int TYPE_DIALOG = 37;
    private static final int TYPE_MENU = 38;
    // §8 متقدم
    private static final int TYPE_CANVAS = 39;
    private static final int TYPE_MAP_VIEW = 40;
    private static final int TYPE_WEB_VIEW = 41;
    private static final int TYPE_VIDEO_PLAYER = 42;
    private static final int TYPE_SCROLL_VIEW = 43;
    // §9 خاص
    private static final int TYPE_CUSTOM = 44;
    private static final int TYPE_CONDITIONAL = 45;
    private static final int TYPE_GROUP = 46;
    // §10 جديد v2
    private static final int TYPE_PROGRESS_BAR = 47;
    private static final int TYPE_BADGE = 48;
    private static final int TYPE_CHIP = 49;
    private static final int TYPE_AVATAR = 50;
    private static final int TYPE_SEARCH_BAR = 51;
    private static final int TYPE_BOTTOM_SHEET = 52;
    private static final int TYPE_FAB = 53;
    private static final int TYPE_SNACK_BAR = 54;
    private static final int TYPE_DRAWER = 55;
    private static final int TYPE_APP_BAR = 56;
    // §11 بيانات v3
    private static final int TYPE_TOOLTIP = 57;
    private static final int TYPE_DATA_TABLE = 58;
    private static final int TYPE_TREE_VIEW = 59;
    private static final int TYPE_BREADCRUMB = 60;
    private static final int TYPE_PAGINATION = 61;
    private static final int TYPE_TIMELINE = 62;
    // §12 وسائط v3
    private static final int TYPE_CAROUSEL = 63;
    private static final int TYPE_RICH_TEXT = 64;
    private static final int TYPE_MARKDOWN = 65;
    private static final int TYPE_CODE_BLOCK = 66;
    private static final int TYPE_AUDIO_PLAYER = 67;
    private static final int TYPE_IMAGE_GALLERY = 68;
    // §13 حركة v3
    private static final int TYPE_SKELETON = 69;
    private static final int TYPE_SHIMMER = 70;
    private static final int TYPE_ANIMATED_LIST = 71;
    private static final int TYPE_EXPANDABLE = 72;
    private static final int TYPE_COLLAPSIBLE = 73;
    private static final int TYPE_SWIPEABLE = 74;
    // §14 تحكم متقدم v3
    private static final int TYPE_STEPPER = 75;
    private static final int TYPE_SEGMENTED = 76;
    private static final int TYPE_RATING_BAR = 77;
    private static final int TYPE_TIME_PICKER = 78;
    private static final int TYPE_CALENDAR = 79;
    private static final int TYPE_COLOR_WHEEL = 80;
    // §15 تنقل متقدم v3
    private static final int TYPE_BOTTOM_NAV = 81;
    private static final int TYPE_SIDE_NAV = 82;
    private static final int TYPE_TOOLBAR = 83;
    private static final int TYPE_SPLIT_VIEW = 84;
    private static final int TYPE_TABS = 85;
    private static final int TYPE_MEGA_MENU = 86;

    // ═══════════════════════════════════════════════════════════════════════════
    // createView — إنشاء عنصر أصلي حسب النوع
    // ═══════════════════════════════════════════════════════════════════════════

    public static View createView(Context ctx, int typeCode) {
        switch (typeCode) {
            // ── §1 عَرض ──────────────────────────────
            case TYPE_TEXT:      return createText(ctx);
            case TYPE_IMAGE:     return createImage(ctx);
            case TYPE_ICON:      return createIcon(ctx);

            // ── §2 إدخال ──────────────────────────────
            case TYPE_BUTTON:    return createButton(ctx);
            case TYPE_TEXT_FIELD: return createTextField(ctx);
            case TYPE_TEXT_AREA: return createTextArea(ctx);
            case TYPE_TOGGLE:    return createToggle(ctx);
            case TYPE_SLIDER:    return createSlider(ctx);
            case TYPE_PICKER:    return createPicker(ctx);
            case TYPE_DATE_PICKER: return createPlaceholder(ctx, "📅 منتقي تاريخ");
            case TYPE_COLOR_PICKER: return createPlaceholder(ctx, "🎨 منتقي لون");
            case TYPE_CHECKBOX:  return createCheckbox(ctx);
            case TYPE_RADIO:     return createRadio(ctx);

            // ── §3 تخطيط ──────────────────────────────
            case TYPE_COLUMN:    return createColumn(ctx);
            case TYPE_ROW:       return createRow(ctx);
            case TYPE_STACK:     return createStack(ctx);
            case TYPE_GRID:      return createGrid(ctx);
            case TYPE_LAZY_COLUMN: return createScrollColumn(ctx);
            case TYPE_LAZY_ROW:  return createScrollRow(ctx);
            case TYPE_LAZY_GRID: return createScrollColumn(ctx);
            case TYPE_SPACER:    return createSpacer(ctx);
            case TYPE_DIVIDER:   return createDivider(ctx);
            case TYPE_WRAP:      return createColumn(ctx); // Fallback to column

            // ── §4 حاويات ──────────────────────────────
            case TYPE_CONTAINER: return createContainer(ctx);
            case TYPE_CARD:      return createCard(ctx);
            case TYPE_SURFACE:   return createSurface(ctx);
            case TYPE_SCAFFOLD:  return createScaffold(ctx);
            case TYPE_BOX:       return createStack(ctx);

            // ── §5 تنقل ──────────────────────────────
            case TYPE_NAV_VIEW:  return createColumn(ctx);
            case TYPE_NAV_LINK:  return createNavLink(ctx);
            case TYPE_TAB_VIEW:  return createTabView(ctx);
            case TYPE_TAB_ITEM:  return createTabItem(ctx);

            // ── §6 قوائم ──────────────────────────────
            case TYPE_LIST:      return createScrollColumn(ctx);
            case TYPE_SECTION:   return createSection(ctx);
            case TYPE_FOR_EACH:  return createColumn(ctx);

            // ── §7 حوارات ──────────────────────────────
            case TYPE_ALERT:     return createAlertBox(ctx);
            case TYPE_SHEET:
            case TYPE_BOTTOM_SHEET: return createBottomSheet(ctx);
            case TYPE_DIALOG:    return createDialogBox(ctx);
            case TYPE_MENU:      return createColumn(ctx);

            // ── §8 متقدم ──────────────────────────────
            case TYPE_CANVAS:    return createPlaceholder(ctx, "🎨 لوحة رسم");
            case TYPE_MAP_VIEW:  return createPlaceholder(ctx, "🗺 خريطة");
            case TYPE_WEB_VIEW:  return createPlaceholder(ctx, "🌐 عرض ويب");
            case TYPE_VIDEO_PLAYER: return createPlaceholder(ctx, "🎬 مشغل فيديو");
            case TYPE_SCROLL_VIEW: return createScrollColumn(ctx);

            // ── §9 خاص ──────────────────────────────
            case TYPE_CUSTOM:
            case TYPE_CONDITIONAL:
            case TYPE_GROUP:     return createColumn(ctx);

            // ── §10 جديد v2 ──────────────────────────────
            case TYPE_PROGRESS_BAR: return createProgressBar(ctx);
            case TYPE_BADGE:     return createBadge(ctx);
            case TYPE_CHIP:      return createChip(ctx);
            case TYPE_AVATAR:    return createAvatar(ctx);
            case TYPE_SEARCH_BAR: return createSearchBar(ctx);
            case TYPE_FAB:       return createFAB(ctx);
            case TYPE_SNACK_BAR: return createSnackBar(ctx);
            case TYPE_DRAWER:    return createColumn(ctx);
            case TYPE_APP_BAR:   return createAppBar(ctx);

            // ── §11 بيانات v3 ──────────────────────────────
            case TYPE_TOOLTIP:   return createTooltip(ctx);
            case TYPE_DATA_TABLE: return createDataTable(ctx);
            case TYPE_TREE_VIEW: return createScrollColumn(ctx);
            case TYPE_BREADCRUMB: return createBreadcrumb(ctx);
            case TYPE_PAGINATION: return createPagination(ctx);
            case TYPE_TIMELINE:  return createTimeline(ctx);

            // ── §12 وسائط v3 ──────────────────────────────
            case TYPE_CAROUSEL:  return createCarousel(ctx);
            case TYPE_RICH_TEXT: return createText(ctx);
            case TYPE_MARKDOWN:  return createText(ctx);
            case TYPE_CODE_BLOCK: return createCodeBlock(ctx);
            case TYPE_AUDIO_PLAYER: return createPlaceholder(ctx, "🎵 مشغل صوت");
            case TYPE_IMAGE_GALLERY: return createScrollRow(ctx);

            // ── §13 حركة v3 ──────────────────────────────
            case TYPE_SKELETON:  return createSkeleton(ctx);
            case TYPE_SHIMMER:   return createSkeleton(ctx);
            case TYPE_ANIMATED_LIST: return createScrollColumn(ctx);
            case TYPE_EXPANDABLE:
            case TYPE_COLLAPSIBLE: return createExpandable(ctx);
            case TYPE_SWIPEABLE: return createStack(ctx);

            // ── §14 تحكم متقدم v3 ──────────────────────────────
            case TYPE_STEPPER:   return createStepper(ctx);
            case TYPE_SEGMENTED: return createSegmented(ctx);
            case TYPE_RATING_BAR: return createRatingBar(ctx);
            case TYPE_TIME_PICKER: return createPlaceholder(ctx, "⏰ منتقي وقت");
            case TYPE_CALENDAR:  return createPlaceholder(ctx, "📅 تقويم");
            case TYPE_COLOR_WHEEL: return createPlaceholder(ctx, "🎨 عجلة ألوان");

            // ── §15 تنقل متقدم v3 ──────────────────────────────
            case TYPE_BOTTOM_NAV: return createBottomNav(ctx);
            case TYPE_SIDE_NAV:  return createColumn(ctx);
            case TYPE_TOOLBAR:   return createToolbar(ctx);
            case TYPE_SPLIT_VIEW: return createRow(ctx);
            case TYPE_TABS:      return createRow(ctx);
            case TYPE_MEGA_MENU: return createColumn(ctx);

            default:
                return createPlaceholder(ctx, "عنصر #" + typeCode);
        }
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // setProperty — تعيين خاصية على عنصر
    // ═══════════════════════════════════════════════════════════════════════════

    public static void setProperty(View view, String key, String value) {
        if (key == null || value == null || value.isEmpty()) return;

        switch (key) {
            // — نص ومحتوى —
            case "text": case "نص": case "محتوى": case "عنوان":
                if (view instanceof TextView) {
                    ((TextView) view).setText(value);
                } else if (view instanceof ViewGroup) {
                    // AppBar: set title on first TextView child
                    setFirstChildText((ViewGroup) view, value);
                }
                break;

            // — حجم الخط —
            case "fontSize": case "حجم_خط":
                if (view instanceof TextView) {
                    try {
                        float size = Float.parseFloat(value);
                        ((TextView) view).setTextSize(TypedValue.COMPLEX_UNIT_SP, size);
                    } catch (NumberFormatException ignored) {}
                }
                break;

            // — لون النص —
            case "color": case "لون": case "لون_نص":
                if (view instanceof TextView) {
                    ((TextView) view).setTextColor(parseColor(value));
                }
                break;

            // — لون الخلفية —
            case "backgroundColor": case "لون_خلفية":
                applyBackgroundColor(view, parseColor(value));
                break;

            // — عرض —
            case "width": case "عرض":
                try {
                    int w = (int) (Float.parseFloat(value) * view.getResources().getDisplayMetrics().density);
                    ViewGroup.LayoutParams lp = view.getLayoutParams();
                    if (lp == null) lp = new ViewGroup.LayoutParams(w, ViewGroup.LayoutParams.WRAP_CONTENT);
                    else lp.width = w;
                    view.setLayoutParams(lp);
                } catch (NumberFormatException ignored) {}
                break;

            // — ارتفاع —
            case "height": case "ارتفاع":
                try {
                    int h = (int) (Float.parseFloat(value) * view.getResources().getDisplayMetrics().density);
                    ViewGroup.LayoutParams lp = view.getLayoutParams();
                    if (lp == null) lp = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, h);
                    else lp.height = h;
                    view.setLayoutParams(lp);
                } catch (NumberFormatException ignored) {}
                break;

            // — حشوة —
            case "padding": case "حشوة":
                try {
                    int p = dp(view.getContext(), (int) Float.parseFloat(value));
                    view.setPadding(p, p, p, p);
                } catch (NumberFormatException ignored) {}
                break;

            // — محاذاة —
            case "alignment": case "محاذاة":
                applyAlignment(view, value);
                break;

            // — تباعد —
            case "spacing": case "تباعد":
                // Handled in addChild via tag
                view.setTag(0x7f0a0001, value);
                break;

            // — شفافية —
            case "opacity": case "شفافية":
                try {
                    view.setAlpha(Float.parseFloat(value));
                } catch (NumberFormatException ignored) {}
                break;

            // — ظل / ارتفاع —
            case "elevation": case "ظل":
                try {
                    view.setElevation(dp(view.getContext(), (int) Float.parseFloat(value)));
                } catch (NumberFormatException ignored) {}
                break;

            // — مرئي —
            case "visible": case "مرئي":
                view.setVisibility("false".equals(value) ? View.GONE : View.VISIBLE);
                break;

            // — قيمة (للشرائط والمنزلقات) —
            case "value": case "قيمة":
                if (view instanceof ProgressBar) {
                    try { ((ProgressBar) view).setProgress((int)(Float.parseFloat(value) * 100)); }
                    catch (NumberFormatException ignored) {}
                } else if (view instanceof SeekBar) {
                    try { ((SeekBar) view).setProgress((int)(Float.parseFloat(value) * 100)); }
                    catch (NumberFormatException ignored) {}
                }
                break;

            // — تلميح (placeholder) —
            case "hint": case "تلميح":
                if (view instanceof EditText) {
                    ((EditText) view).setHint(value);
                }
                break;

            // — مُحدد (لـ Toggle/Checkbox) —
            case "checked": case "محدد":
                if (view instanceof CompoundButton) {
                    ((CompoundButton) view).setChecked("true".equals(value));
                }
                break;

            // — bold/italic —
            case "bold": case "عريض":
                if (view instanceof TextView && "true".equals(value)) {
                    ((TextView) view).setTypeface(((TextView) view).getTypeface(), Typeface.BOLD);
                }
                break;

            case "italic": case "مائل":
                if (view instanceof TextView && "true".equals(value)) {
                    ((TextView) view).setTypeface(((TextView) view).getTypeface(), Typeface.ITALIC);
                }
                break;
        }
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // addChild — إضافة ابن إلى حاوية
    // ═══════════════════════════════════════════════════════════════════════════

    public static void addChild(View parent, View child) {
        if (parent instanceof ScrollView) {
            // ScrollView accepts only one child, wrap in LinearLayout if needed
            if (((ScrollView) parent).getChildCount() == 0) {
                LinearLayout inner = new LinearLayout(parent.getContext());
                inner.setOrientation(LinearLayout.VERTICAL);
                inner.setLayoutParams(new ViewGroup.LayoutParams(
                    ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));
                ((ScrollView) parent).addView(inner);
                inner.addView(child);
            } else {
                View existing = ((ScrollView) parent).getChildAt(0);
                if (existing instanceof ViewGroup) {
                    ((ViewGroup) existing).addView(child);
                }
            }
        } else if (parent instanceof HorizontalScrollView) {
            if (((HorizontalScrollView) parent).getChildCount() == 0) {
                LinearLayout inner = new LinearLayout(parent.getContext());
                inner.setOrientation(LinearLayout.HORIZONTAL);
                inner.setLayoutParams(new ViewGroup.LayoutParams(
                    ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));
                ((HorizontalScrollView) parent).addView(inner);
                inner.addView(child);
            } else {
                View existing = ((HorizontalScrollView) parent).getChildAt(0);
                if (existing instanceof ViewGroup) {
                    ((ViewGroup) existing).addView(child);
                }
            }
        } else if (parent instanceof ViewGroup) {
            ViewGroup vg = (ViewGroup) parent;

            // تطبيق تباعد إذا موجود
            Object spacingTag = vg.getTag(0x7f0a0001);
            if (spacingTag != null && vg.getChildCount() > 0) {
                try {
                    int spacing = dp(parent.getContext(), (int) Float.parseFloat(spacingTag.toString()));
                    LinearLayout.LayoutParams lp;
                    if (child.getLayoutParams() instanceof LinearLayout.LayoutParams) {
                        lp = (LinearLayout.LayoutParams) child.getLayoutParams();
                    } else {
                        lp = new LinearLayout.LayoutParams(
                            ViewGroup.LayoutParams.MATCH_PARENT,
                            ViewGroup.LayoutParams.WRAP_CONTENT);
                    }
                    if (vg instanceof LinearLayout) {
                        int orient = ((LinearLayout) vg).getOrientation();
                        if (orient == LinearLayout.VERTICAL) {
                            lp.topMargin = spacing;
                        } else {
                            lp.setMarginStart(spacing);
                        }
                    }
                    child.setLayoutParams(lp);
                } catch (NumberFormatException ignored) {}
            }

            vg.addView(child);
        }
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // wrapRoot — لف الجذر في حاوية RTL مع تمرير وتصميم عربي
    // ═══════════════════════════════════════════════════════════════════════════

    public static View wrapRoot(Context ctx, View root) {
        ScrollView scroll = new ScrollView(ctx);
        scroll.setLayoutParams(new ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        scroll.setBackgroundColor(BACKGROUND);
        scroll.setLayoutDirection(View.LAYOUT_DIRECTION_RTL);
        scroll.setFillViewport(true);
        scroll.setOverScrollMode(View.OVER_SCROLL_NEVER);

        LinearLayout wrapper = new LinearLayout(ctx);
        wrapper.setOrientation(LinearLayout.VERTICAL);
        wrapper.setLayoutParams(new ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));
        int p = dp(ctx, PADDING_MD);
        wrapper.setPadding(p, p, p, p);

        wrapper.addView(root);
        scroll.addView(wrapper);
        return scroll;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // §1 — مكونات العَرض
    // ═══════════════════════════════════════════════════════════════════════════

    private static TextView createText(Context ctx) {
        TextView tv = new TextView(ctx);
        tv.setTextColor(TEXT_PRIMARY);
        tv.setTextSize(TypedValue.COMPLEX_UNIT_SP, 16);
        tv.setLineSpacing(0, 1.4f);
        tv.setTextDirection(View.TEXT_DIRECTION_ANY_RTL);
        tv.setLayoutParams(matchWrap());
        return tv;
    }

    private static ImageView createImage(Context ctx) {
        ImageView iv = new ImageView(ctx);
        iv.setScaleType(ImageView.ScaleType.FIT_CENTER);
        iv.setLayoutParams(new ViewGroup.LayoutParams(dp(ctx, 200), dp(ctx, 150)));
        iv.setBackgroundColor(SURFACE_DIM);
        return iv;
    }

    private static TextView createIcon(Context ctx) {
        TextView tv = new TextView(ctx);
        tv.setTextSize(TypedValue.COMPLEX_UNIT_SP, 24);
        tv.setTextColor(PRIMARY);
        tv.setGravity(Gravity.CENTER);
        tv.setLayoutParams(wrapWrap());
        return tv;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // §2 — مكونات الإدخال
    // ═══════════════════════════════════════════════════════════════════════════

    private static Button createButton(Context ctx) {
        Button btn = new Button(ctx);
        btn.setAllCaps(false);
        btn.setTextColor(Color.WHITE);
        btn.setTextSize(TypedValue.COMPLEX_UNIT_SP, 16);

        GradientDrawable bg = new GradientDrawable();
        bg.setColor(PRIMARY);
        bg.setCornerRadius(dp(ctx, CORNER_RADIUS));

        RippleDrawable ripple = new RippleDrawable(
            ColorStateList.valueOf(PRIMARY_DARK), bg, null);
        btn.setBackground(ripple);

        int ph = dp(ctx, PADDING_MD);
        int pv = dp(ctx, PADDING_SM + 4);
        btn.setPadding(ph, pv, ph, pv);
        btn.setElevation(dp(ctx, 2));
        btn.setLayoutParams(wrapWrap());
        return btn;
    }

    private static EditText createTextField(Context ctx) {
        EditText et = new EditText(ctx);
        et.setTextColor(TEXT_PRIMARY);
        et.setHintTextColor(TEXT_SECONDARY);
        et.setTextSize(TypedValue.COMPLEX_UNIT_SP, 15);
        et.setSingleLine(true);
        et.setTextDirection(View.TEXT_DIRECTION_ANY_RTL);

        GradientDrawable bg = new GradientDrawable();
        bg.setColor(SURFACE);
        bg.setCornerRadius(dp(ctx, CORNER_RADIUS));
        bg.setStroke(dp(ctx, 1), BORDER_COLOR);
        et.setBackground(bg);

        int p = dp(ctx, PADDING_MD);
        et.setPadding(p, dp(ctx, 12), p, dp(ctx, 12));
        et.setLayoutParams(matchWrap());
        return et;
    }

    private static EditText createTextArea(Context ctx) {
        EditText et = createTextField(ctx);
        et.setSingleLine(false);
        et.setMinLines(3);
        et.setMaxLines(8);
        et.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_FLAG_MULTI_LINE);
        return et;
    }

    private static Switch createToggle(Context ctx) {
        Switch sw = new Switch(ctx);
        sw.setTextColor(TEXT_PRIMARY);
        sw.setLayoutParams(wrapWrap());
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            sw.setTrackTintList(ColorStateList.valueOf(PRIMARY_LIGHT));
            sw.setThumbTintList(new ColorStateList(
                new int[][] {{android.R.attr.state_checked}, {}},
                new int[] {PRIMARY, BORDER_COLOR}));
        }
        return sw;
    }

    private static SeekBar createSlider(Context ctx) {
        SeekBar sb = new SeekBar(ctx);
        sb.setMax(100);
        sb.setProgress(50);
        sb.setLayoutParams(matchWrap());
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            sb.setProgressTintList(ColorStateList.valueOf(PRIMARY));
            sb.setThumbTintList(ColorStateList.valueOf(PRIMARY_DARK));
        }
        return sb;
    }

    private static Spinner createPicker(Context ctx) {
        Spinner sp = new Spinner(ctx);
        sp.setLayoutParams(wrapWrap());
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(SURFACE);
        bg.setCornerRadius(dp(ctx, CORNER_RADIUS));
        bg.setStroke(dp(ctx, 1), BORDER_COLOR);
        sp.setBackground(bg);
        sp.setPadding(dp(ctx, 12), dp(ctx, 8), dp(ctx, 12), dp(ctx, 8));
        return sp;
    }

    private static CheckBox createCheckbox(Context ctx) {
        CheckBox cb = new CheckBox(ctx);
        cb.setTextColor(TEXT_PRIMARY);
        cb.setLayoutParams(wrapWrap());
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            cb.setButtonTintList(new ColorStateList(
                new int[][] {{android.R.attr.state_checked}, {}},
                new int[] {PRIMARY, TEXT_SECONDARY}));
        }
        return cb;
    }

    private static RadioButton createRadio(Context ctx) {
        RadioButton rb = new RadioButton(ctx);
        rb.setTextColor(TEXT_PRIMARY);
        rb.setLayoutParams(wrapWrap());
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            rb.setButtonTintList(new ColorStateList(
                new int[][] {{android.R.attr.state_checked}, {}},
                new int[] {PRIMARY, TEXT_SECONDARY}));
        }
        return rb;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // §3 — مكونات التخطيط
    // ═══════════════════════════════════════════════════════════════════════════

    private static LinearLayout createColumn(Context ctx) {
        LinearLayout ll = new LinearLayout(ctx);
        ll.setOrientation(LinearLayout.VERTICAL);
        ll.setLayoutParams(matchWrap());
        ll.setLayoutDirection(View.LAYOUT_DIRECTION_RTL);
        return ll;
    }

    private static LinearLayout createRow(Context ctx) {
        LinearLayout ll = new LinearLayout(ctx);
        ll.setOrientation(LinearLayout.HORIZONTAL);
        ll.setGravity(Gravity.CENTER_VERTICAL);
        ll.setLayoutParams(matchWrap());
        ll.setLayoutDirection(View.LAYOUT_DIRECTION_RTL);
        return ll;
    }

    private static FrameLayout createStack(Context ctx) {
        FrameLayout fl = new FrameLayout(ctx);
        fl.setLayoutParams(matchWrap());
        return fl;
    }

    private static GridLayout createGrid(Context ctx) {
        GridLayout gl = new GridLayout(ctx);
        gl.setColumnCount(2);
        gl.setLayoutParams(matchWrap());
        gl.setLayoutDirection(View.LAYOUT_DIRECTION_RTL);
        return gl;
    }

    private static ScrollView createScrollColumn(Context ctx) {
        ScrollView sv = new ScrollView(ctx);
        sv.setLayoutParams(matchWrap());
        sv.setOverScrollMode(View.OVER_SCROLL_NEVER);
        return sv;
    }

    private static HorizontalScrollView createScrollRow(Context ctx) {
        HorizontalScrollView hsv = new HorizontalScrollView(ctx);
        hsv.setLayoutParams(matchWrap());
        hsv.setHorizontalScrollBarEnabled(false);
        hsv.setOverScrollMode(View.OVER_SCROLL_NEVER);
        return hsv;
    }

    private static Space createSpacer(Context ctx) {
        Space sp = new Space(ctx);
        sp.setLayoutParams(new LinearLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, dp(ctx, 12)));
        return sp;
    }

    private static View createDivider(Context ctx) {
        View v = new View(ctx);
        v.setBackgroundColor(DIVIDER_COLOR);
        v.setLayoutParams(new LinearLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, dp(ctx, 1)));
        LinearLayout.LayoutParams lp = (LinearLayout.LayoutParams) v.getLayoutParams();
        lp.topMargin = dp(ctx, 6);
        lp.bottomMargin = dp(ctx, 6);
        return v;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // §4 — حاويات
    // ═══════════════════════════════════════════════════════════════════════════

    private static FrameLayout createContainer(Context ctx) {
        FrameLayout fl = new FrameLayout(ctx);
        fl.setLayoutParams(matchWrap());
        int p = dp(ctx, PADDING_MD);
        fl.setPadding(p, p, p, p);
        return fl;
    }

    private static LinearLayout createCard(Context ctx) {
        LinearLayout card = new LinearLayout(ctx);
        card.setOrientation(LinearLayout.VERTICAL);

        GradientDrawable bg = new GradientDrawable();
        bg.setColor(SURFACE);
        bg.setCornerRadius(dp(ctx, CORNER_RADIUS + 2));
        card.setBackground(bg);

        card.setElevation(dp(ctx, CARD_ELEVATION));
        int p = dp(ctx, PADDING_MD);
        card.setPadding(p, p, p, p);
        card.setLayoutParams(matchWrap());
        card.setLayoutDirection(View.LAYOUT_DIRECTION_RTL);

        // هامش للبطاقة
        LinearLayout.LayoutParams lp = (LinearLayout.LayoutParams) card.getLayoutParams();
        int m = dp(ctx, 4);
        lp.setMargins(m, m, m, m);
        return card;
    }

    private static FrameLayout createSurface(Context ctx) {
        FrameLayout fl = new FrameLayout(ctx);
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(SURFACE);
        bg.setCornerRadius(dp(ctx, CORNER_RADIUS));
        fl.setBackground(bg);
        fl.setElevation(dp(ctx, 2));
        fl.setLayoutParams(matchWrap());
        int p = dp(ctx, PADDING_MD);
        fl.setPadding(p, p, p, p);
        return fl;
    }

    private static LinearLayout createScaffold(Context ctx) {
        LinearLayout ll = new LinearLayout(ctx);
        ll.setOrientation(LinearLayout.VERTICAL);
        ll.setBackgroundColor(BACKGROUND);
        ll.setLayoutParams(new ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        ll.setLayoutDirection(View.LAYOUT_DIRECTION_RTL);
        return ll;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // §5+6 — تنقل وقوائم
    // ═══════════════════════════════════════════════════════════════════════════

    private static TextView createNavLink(Context ctx) {
        TextView tv = new TextView(ctx);
        tv.setTextColor(PRIMARY);
        tv.setTextSize(TypedValue.COMPLEX_UNIT_SP, 16);
        tv.setPadding(dp(ctx, 12), dp(ctx, 10), dp(ctx, 12), dp(ctx, 10));
        tv.setLayoutParams(matchWrap());
        tv.setClickable(true);
        tv.setFocusable(true);
        return tv;
    }

    private static LinearLayout createTabView(Context ctx) {
        LinearLayout ll = new LinearLayout(ctx);
        ll.setOrientation(LinearLayout.VERTICAL);
        ll.setLayoutParams(matchWrap());
        return ll;
    }

    private static Button createTabItem(Context ctx) {
        Button btn = new Button(ctx);
        btn.setAllCaps(false);
        btn.setTextColor(PRIMARY);
        btn.setBackgroundColor(Color.TRANSPARENT);
        btn.setTextSize(TypedValue.COMPLEX_UNIT_SP, 14);
        btn.setLayoutParams(new LinearLayout.LayoutParams(
            0, ViewGroup.LayoutParams.WRAP_CONTENT, 1f));
        return btn;
    }

    private static LinearLayout createSection(Context ctx) {
        LinearLayout ll = new LinearLayout(ctx);
        ll.setOrientation(LinearLayout.VERTICAL);
        ll.setLayoutParams(matchWrap());
        int p = dp(ctx, PADDING_SM);
        ll.setPadding(0, p, 0, p);
        return ll;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // §7 — حوارات
    // ═══════════════════════════════════════════════════════════════════════════

    private static LinearLayout createAlertBox(Context ctx) {
        LinearLayout box = new LinearLayout(ctx);
        box.setOrientation(LinearLayout.VERTICAL);
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(0xFFFFF3E0); // Orange 50
        bg.setCornerRadius(dp(ctx, CORNER_RADIUS));
        bg.setStroke(dp(ctx, 1), ACCENT);
        box.setBackground(bg);
        int p = dp(ctx, PADDING_MD);
        box.setPadding(p, p, p, p);
        box.setLayoutParams(matchWrap());
        return box;
    }

    private static LinearLayout createBottomSheet(Context ctx) {
        LinearLayout sheet = new LinearLayout(ctx);
        sheet.setOrientation(LinearLayout.VERTICAL);
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(SURFACE);
        float[] radii = {dp(ctx,20),dp(ctx,20),dp(ctx,20),dp(ctx,20),0,0,0,0};
        bg.setCornerRadii(radii);
        sheet.setBackground(bg);
        sheet.setElevation(dp(ctx, 8));
        int p = dp(ctx, PADDING_LG);
        sheet.setPadding(p, p, p, p);
        sheet.setLayoutParams(matchWrap());
        return sheet;
    }

    private static LinearLayout createDialogBox(Context ctx) {
        LinearLayout dlg = new LinearLayout(ctx);
        dlg.setOrientation(LinearLayout.VERTICAL);
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(SURFACE);
        bg.setCornerRadius(dp(ctx, CORNER_RADIUS + 4));
        dlg.setBackground(bg);
        dlg.setElevation(dp(ctx, 12));
        int p = dp(ctx, PADDING_LG);
        dlg.setPadding(p, p, p, p);
        dlg.setLayoutParams(matchWrap());
        return dlg;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // §10 — مكونات v2
    // ═══════════════════════════════════════════════════════════════════════════

    private static ProgressBar createProgressBar(Context ctx) {
        ProgressBar pb = new ProgressBar(ctx, null, android.R.attr.progressBarStyleHorizontal);
        pb.setMax(100);
        pb.setProgress(50);
        pb.setLayoutParams(matchWrap());
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            pb.setProgressTintList(ColorStateList.valueOf(PRIMARY));
            pb.setProgressBackgroundTintList(ColorStateList.valueOf(SURFACE_DIM));
        }
        return pb;
    }

    private static TextView createBadge(Context ctx) {
        TextView badge = new TextView(ctx);
        badge.setTextColor(Color.WHITE);
        badge.setTextSize(TypedValue.COMPLEX_UNIT_SP, 12);
        badge.setGravity(Gravity.CENTER);
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(ERROR_COLOR);
        bg.setCornerRadius(dp(ctx, 12));
        badge.setBackground(bg);
        int p = dp(ctx, 6);
        badge.setPadding(p, dp(ctx, 2), p, dp(ctx, 2));
        badge.setMinWidth(dp(ctx, 24));
        badge.setLayoutParams(wrapWrap());
        return badge;
    }

    private static Button createChip(Context ctx) {
        Button chip = new Button(ctx);
        chip.setAllCaps(false);
        chip.setTextColor(PRIMARY);
        chip.setTextSize(TypedValue.COMPLEX_UNIT_SP, 13);
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(Color.TRANSPARENT);
        bg.setCornerRadius(dp(ctx, 20));
        bg.setStroke(dp(ctx, 1), PRIMARY);
        chip.setBackground(bg);
        int ph = dp(ctx, PADDING_MD);
        int pv = dp(ctx, 4);
        chip.setPadding(ph, pv, ph, pv);
        chip.setLayoutParams(wrapWrap());
        return chip;
    }

    private static FrameLayout createAvatar(Context ctx) {
        FrameLayout fl = new FrameLayout(ctx);
        int size = dp(ctx, 48);
        fl.setLayoutParams(new ViewGroup.LayoutParams(size, size));
        GradientDrawable bg = new GradientDrawable();
        bg.setShape(GradientDrawable.OVAL);
        bg.setColor(PRIMARY_LIGHT);
        fl.setBackground(bg);

        TextView initial = new TextView(ctx);
        initial.setTextColor(Color.WHITE);
        initial.setTextSize(TypedValue.COMPLEX_UNIT_SP, 20);
        initial.setGravity(Gravity.CENTER);
        initial.setTypeface(Typeface.DEFAULT_BOLD);
        initial.setLayoutParams(new FrameLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        initial.setText("👤");
        fl.addView(initial);
        return fl;
    }

    private static LinearLayout createSearchBar(Context ctx) {
        LinearLayout row = new LinearLayout(ctx);
        row.setOrientation(LinearLayout.HORIZONTAL);
        row.setGravity(Gravity.CENTER_VERTICAL);
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(SURFACE);
        bg.setCornerRadius(dp(ctx, 24));
        bg.setStroke(dp(ctx, 1), BORDER_COLOR);
        row.setBackground(bg);
        int p = dp(ctx, PADDING_SM);
        row.setPadding(dp(ctx, PADDING_MD), p, dp(ctx, PADDING_MD), p);
        row.setLayoutParams(matchWrap());

        TextView icon = new TextView(ctx);
        icon.setText("🔍");
        icon.setTextSize(TypedValue.COMPLEX_UNIT_SP, 18);
        icon.setPadding(0, 0, dp(ctx, 8), 0);
        row.addView(icon, wrapWrap());

        EditText et = new EditText(ctx);
        et.setTextColor(TEXT_PRIMARY);
        et.setHintTextColor(TEXT_SECONDARY);
        et.setHint("بحث...");
        et.setBackground(null);
        et.setSingleLine(true);
        et.setTextSize(TypedValue.COMPLEX_UNIT_SP, 15);
        row.addView(et, new LinearLayout.LayoutParams(0, ViewGroup.LayoutParams.WRAP_CONTENT, 1f));
        return row;
    }

    private static Button createFAB(Context ctx) {
        Button fab = new Button(ctx);
        fab.setAllCaps(false);
        fab.setTextColor(Color.WHITE);
        fab.setTextSize(TypedValue.COMPLEX_UNIT_SP, 24);
        int size = dp(ctx, 56);
        fab.setLayoutParams(new ViewGroup.LayoutParams(size, size));
        GradientDrawable bg = new GradientDrawable();
        bg.setShape(GradientDrawable.OVAL);
        bg.setColor(ACCENT);
        RippleDrawable ripple = new RippleDrawable(
            ColorStateList.valueOf(ACCENT_DARK), bg, null);
        fab.setBackground(ripple);
        fab.setElevation(dp(ctx, 6));
        fab.setText("+");
        return fab;
    }

    private static LinearLayout createSnackBar(Context ctx) {
        LinearLayout snack = new LinearLayout(ctx);
        snack.setOrientation(LinearLayout.HORIZONTAL);
        snack.setGravity(Gravity.CENTER_VERTICAL);
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(0xFF323232); // Dark
        bg.setCornerRadius(dp(ctx, 8));
        snack.setBackground(bg);
        int p = dp(ctx, PADDING_MD);
        snack.setPadding(p, dp(ctx, 12), p, dp(ctx, 12));
        snack.setLayoutParams(matchWrap());
        return snack;
    }

    private static LinearLayout createAppBar(Context ctx) {
        LinearLayout bar = new LinearLayout(ctx);
        bar.setOrientation(LinearLayout.HORIZONTAL);
        bar.setGravity(Gravity.CENTER_VERTICAL);
        bar.setLayoutDirection(View.LAYOUT_DIRECTION_RTL);

        GradientDrawable bg = new GradientDrawable(
            GradientDrawable.Orientation.LEFT_RIGHT,
            new int[] {PRIMARY, PRIMARY_DARK});
        bg.setCornerRadius(0);
        bar.setBackground(bg);

        bar.setElevation(dp(ctx, 4));
        int ph = dp(ctx, PADDING_MD);
        bar.setPadding(ph, dp(ctx, 14), ph, dp(ctx, 14));
        bar.setLayoutParams(new ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, dp(ctx, 56)));

        // عنوان
        TextView title = new TextView(ctx);
        title.setTextColor(Color.WHITE);
        title.setTextSize(TypedValue.COMPLEX_UNIT_SP, 20);
        title.setTypeface(Typeface.DEFAULT_BOLD);
        title.setText("تطبيق ص");
        bar.addView(title, new LinearLayout.LayoutParams(
            0, ViewGroup.LayoutParams.WRAP_CONTENT, 1f));

        return bar;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // §11 — بيانات v3
    // ═══════════════════════════════════════════════════════════════════════════

    private static FrameLayout createTooltip(Context ctx) {
        FrameLayout fl = new FrameLayout(ctx);
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(0xFF616161);
        bg.setCornerRadius(dp(ctx, 6));
        fl.setBackground(bg);
        int p = dp(ctx, 8);
        fl.setPadding(p, dp(ctx, 4), p, dp(ctx, 4));
        fl.setLayoutParams(wrapWrap());
        return fl;
    }

    private static LinearLayout createDataTable(Context ctx) {
        LinearLayout table = new LinearLayout(ctx);
        table.setOrientation(LinearLayout.VERTICAL);
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(SURFACE);
        bg.setCornerRadius(dp(ctx, CORNER_RADIUS));
        bg.setStroke(dp(ctx, 1), BORDER_COLOR);
        table.setBackground(bg);
        table.setLayoutParams(matchWrap());
        return table;
    }

    private static LinearLayout createBreadcrumb(Context ctx) {
        LinearLayout row = new LinearLayout(ctx);
        row.setOrientation(LinearLayout.HORIZONTAL);
        row.setGravity(Gravity.CENTER_VERTICAL);
        row.setLayoutParams(matchWrap());
        row.setLayoutDirection(View.LAYOUT_DIRECTION_RTL);
        return row;
    }

    private static LinearLayout createPagination(Context ctx) {
        LinearLayout row = new LinearLayout(ctx);
        row.setOrientation(LinearLayout.HORIZONTAL);
        row.setGravity(Gravity.CENTER);
        row.setLayoutParams(matchWrap());
        int p = dp(ctx, PADDING_SM);
        row.setPadding(0, p, 0, p);
        return row;
    }

    private static LinearLayout createTimeline(Context ctx) {
        LinearLayout ll = new LinearLayout(ctx);
        ll.setOrientation(LinearLayout.VERTICAL);
        ll.setLayoutParams(matchWrap());
        int p = dp(ctx, PADDING_SM);
        ll.setPadding(dp(ctx, PADDING_LG), p, dp(ctx, PADDING_MD), p);
        return ll;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // §12 — وسائط v3
    // ═══════════════════════════════════════════════════════════════════════════

    private static HorizontalScrollView createCarousel(Context ctx) {
        HorizontalScrollView hsv = new HorizontalScrollView(ctx);
        hsv.setLayoutParams(matchWrap());
        hsv.setHorizontalScrollBarEnabled(false);
        hsv.setOverScrollMode(View.OVER_SCROLL_NEVER);
        return hsv;
    }

    private static LinearLayout createCodeBlock(Context ctx) {
        LinearLayout code = new LinearLayout(ctx);
        code.setOrientation(LinearLayout.VERTICAL);
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(0xFF1E1E2E); // Dark theme
        bg.setCornerRadius(dp(ctx, CORNER_RADIUS));
        code.setBackground(bg);
        int p = dp(ctx, PADDING_MD);
        code.setPadding(p, p, p, p);
        code.setLayoutParams(matchWrap());
        return code;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // §13 — حركة v3
    // ═══════════════════════════════════════════════════════════════════════════

    private static View createSkeleton(Context ctx) {
        View v = new View(ctx);
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(SURFACE_DIM);
        bg.setCornerRadius(dp(ctx, 8));
        v.setBackground(bg);
        v.setLayoutParams(new ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, dp(ctx, 20)));
        return v;
    }

    private static LinearLayout createExpandable(Context ctx) {
        LinearLayout ll = new LinearLayout(ctx);
        ll.setOrientation(LinearLayout.VERTICAL);
        ll.setLayoutParams(matchWrap());
        ll.setClipChildren(true);
        return ll;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // §14 — تحكم متقدم v3
    // ═══════════════════════════════════════════════════════════════════════════

    private static LinearLayout createStepper(Context ctx) {
        LinearLayout row = new LinearLayout(ctx);
        row.setOrientation(LinearLayout.HORIZONTAL);
        row.setGravity(Gravity.CENTER);
        row.setLayoutParams(wrapWrap());

        Button minus = createStepperButton(ctx, "−");
        TextView value = new TextView(ctx);
        value.setText("0");
        value.setTextColor(TEXT_PRIMARY);
        value.setTextSize(TypedValue.COMPLEX_UNIT_SP, 18);
        value.setGravity(Gravity.CENTER);
        value.setMinWidth(dp(ctx, 48));
        Button plus = createStepperButton(ctx, "+");

        row.addView(minus);
        row.addView(value);
        row.addView(plus);
        return row;
    }

    private static Button createStepperButton(Context ctx, String label) {
        Button btn = new Button(ctx);
        btn.setText(label);
        btn.setAllCaps(false);
        btn.setTextColor(PRIMARY);
        btn.setTextSize(TypedValue.COMPLEX_UNIT_SP, 20);
        int size = dp(ctx, 40);
        btn.setLayoutParams(new ViewGroup.LayoutParams(size, size));
        GradientDrawable bg = new GradientDrawable();
        bg.setShape(GradientDrawable.OVAL);
        bg.setColor(Color.TRANSPARENT);
        bg.setStroke(dp(ctx, 2), PRIMARY);
        btn.setBackground(bg);
        btn.setPadding(0, 0, 0, 0);
        return btn;
    }

    private static LinearLayout createSegmented(Context ctx) {
        LinearLayout row = new LinearLayout(ctx);
        row.setOrientation(LinearLayout.HORIZONTAL);
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(SURFACE_DIM);
        bg.setCornerRadius(dp(ctx, CORNER_RADIUS));
        row.setBackground(bg);
        int p = dp(ctx, 2);
        row.setPadding(p, p, p, p);
        row.setLayoutParams(matchWrap());
        return row;
    }

    private static LinearLayout createRatingBar(Context ctx) {
        LinearLayout row = new LinearLayout(ctx);
        row.setOrientation(LinearLayout.HORIZONTAL);
        row.setGravity(Gravity.CENTER);
        row.setLayoutParams(wrapWrap());

        for (int i = 0; i < 5; i++) {
            TextView star = new TextView(ctx);
            star.setText(i < 3 ? "★" : "☆");
            star.setTextSize(TypedValue.COMPLEX_UNIT_SP, 28);
            star.setTextColor(i < 3 ? ACCENT : TEXT_SECONDARY);
            star.setPadding(dp(ctx, 2), 0, dp(ctx, 2), 0);
            final int rating = i;
            star.setClickable(true);
            star.setOnClickListener(v -> {
                ViewGroup parent = (ViewGroup) v.getParent();
                for (int j = 0; j < parent.getChildCount(); j++) {
                    TextView s = (TextView) parent.getChildAt(j);
                    s.setText(j <= rating ? "★" : "☆");
                    s.setTextColor(j <= rating ? ACCENT : TEXT_SECONDARY);
                }
            });
            row.addView(star, wrapWrap());
        }
        return row;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // §15 — تنقل متقدم v3
    // ═══════════════════════════════════════════════════════════════════════════

    private static LinearLayout createBottomNav(Context ctx) {
        LinearLayout bar = new LinearLayout(ctx);
        bar.setOrientation(LinearLayout.HORIZONTAL);
        bar.setGravity(Gravity.CENTER);
        bar.setBackgroundColor(SURFACE);
        bar.setElevation(dp(ctx, 8));
        int p = dp(ctx, PADDING_SM);
        bar.setPadding(0, p, 0, p);
        bar.setLayoutParams(new ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, dp(ctx, 56)));
        return bar;
    }

    private static LinearLayout createToolbar(Context ctx) {
        LinearLayout bar = new LinearLayout(ctx);
        bar.setOrientation(LinearLayout.HORIZONTAL);
        bar.setGravity(Gravity.CENTER_VERTICAL);
        bar.setBackgroundColor(SURFACE);
        bar.setElevation(dp(ctx, 2));
        int p = dp(ctx, PADDING_SM);
        bar.setPadding(dp(ctx, PADDING_MD), p, dp(ctx, PADDING_MD), p);
        bar.setLayoutParams(new ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, dp(ctx, 48)));
        return bar;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Placeholder لعناصر غير متوفرة حالياً
    // ═══════════════════════════════════════════════════════════════════════════

    private static LinearLayout createPlaceholder(Context ctx, String label) {
        LinearLayout box = new LinearLayout(ctx);
        box.setOrientation(LinearLayout.VERTICAL);
        box.setGravity(Gravity.CENTER);
        GradientDrawable bg = new GradientDrawable();
        bg.setColor(SURFACE_DIM);
        bg.setCornerRadius(dp(ctx, CORNER_RADIUS));
        bg.setStroke(dp(ctx, 1), BORDER_COLOR);
        box.setBackground(bg);
        int p = dp(ctx, PADDING_LG);
        box.setPadding(p, p, p, p);
        box.setLayoutParams(matchWrap());

        TextView tv = new TextView(ctx);
        tv.setText(label);
        tv.setGravity(Gravity.CENTER);
        tv.setTextColor(TEXT_SECONDARY);
        tv.setTextSize(TypedValue.COMPLEX_UNIT_SP, 14);
        box.addView(tv, wrapWrap());
        return box;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // مساعدات
    // ═══════════════════════════════════════════════════════════════════════════

    private static int dp(Context ctx, int dp) {
        return (int) (dp * ctx.getResources().getDisplayMetrics().density + 0.5f);
    }

    private static ViewGroup.LayoutParams matchWrap() {
        return new ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT,
            ViewGroup.LayoutParams.WRAP_CONTENT);
    }

    private static ViewGroup.LayoutParams wrapWrap() {
        return new ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.WRAP_CONTENT,
            ViewGroup.LayoutParams.WRAP_CONTENT);
    }

    private static int parseColor(String colorStr) {
        if (colorStr == null || colorStr.isEmpty()) return TEXT_PRIMARY;

        // ألوان عربية مسماة
        switch (colorStr) {
            case "أحمر": case "احمر": return 0xFFE53935;
            case "أخضر": case "اخضر": return 0xFF43A047;
            case "أزرق": case "ازرق": return 0xFF1E88E5;
            case "أصفر": case "اصفر": return 0xFFFDD835;
            case "برتقالي": return 0xFFFB8C00;
            case "بنفسجي": return 0xFF8E24AA;
            case "وردي": return 0xFFEC407A;
            case "أبيض": case "ابيض": return Color.WHITE;
            case "أسود": case "اسود": return 0xFF212121;
            case "رمادي": return 0xFF9E9E9E;
            case "تركوازي": return PRIMARY;
            case "ذهبي": return ACCENT;
            default:
                // محاولة تحليل كـ hex
                try {
                    if (colorStr.startsWith("#")) return Color.parseColor(colorStr);
                    if (colorStr.startsWith("0x") || colorStr.startsWith("0X")) {
                        return (int) Long.parseLong(colorStr.substring(2), 16);
                    }
                } catch (Exception ignored) {}
                return TEXT_PRIMARY;
        }
    }

    private static void applyAlignment(View view, String alignment) {
        if (view instanceof LinearLayout) {
            LinearLayout ll = (LinearLayout) view;
            switch (alignment) {
                case "وسط": case "center":
                    ll.setGravity(Gravity.CENTER); break;
                case "يمين": case "start": case "right":
                    ll.setGravity(Gravity.START | Gravity.CENTER_VERTICAL); break;
                case "يسار": case "end": case "left":
                    ll.setGravity(Gravity.END | Gravity.CENTER_VERTICAL); break;
                case "أعلى": case "top":
                    ll.setGravity(Gravity.TOP | Gravity.CENTER_HORIZONTAL); break;
                case "أسفل": case "bottom":
                    ll.setGravity(Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL); break;
            }
        } else if (view instanceof TextView) {
            switch (alignment) {
                case "وسط": case "center":
                    ((TextView) view).setGravity(Gravity.CENTER); break;
                case "يمين": case "start":
                    ((TextView) view).setGravity(Gravity.START); break;
                case "يسار": case "end":
                    ((TextView) view).setGravity(Gravity.END); break;
            }
        }
    }

    private static void applyBackgroundColor(View view, int color) {
        if (view.getBackground() instanceof GradientDrawable) {
            ((GradientDrawable) view.getBackground()).setColor(color);
        } else {
            view.setBackgroundColor(color);
        }
    }

    private static void setFirstChildText(ViewGroup vg, String text) {
        for (int i = 0; i < vg.getChildCount(); i++) {
            View child = vg.getChildAt(i);
            if (child instanceof TextView) {
                ((TextView) child).setText(text);
                return;
            }
            if (child instanceof ViewGroup) {
                setFirstChildText((ViewGroup) child, text);
                return;
            }
        }
    }
}
