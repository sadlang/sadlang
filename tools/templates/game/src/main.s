// بسم الله الرحمن الرحيم
/**
 * @file main.s
 * @brief {{PROJECT_NAME}} - 2D Game
 * @author {{AUTHOR_NAME}}
 * @date {{DATE}}
 */

استورد "raylib" كـ RL;

// Game state
حالة = {
    "لاعب_x": 400,
    "لاعب_y": 300,
    "سرعة": 5,
    "نتيجة": 0
};

/**
 * Initialize game
 */
دالة تهيئة() {
    RL.init_window(800, 600, "{{PROJECT_NAME}}");
    RL.set_target_fps(60);
}

/**
 * Update game state
 */
دالة تحديث() {
    // Handle input
    إذا (RL.is_key_down(RL.KEY_RIGHT)) {
        حالة["لاعب_x"] += حالة["سرعة"];
    }
    إذا (RL.is_key_down(RL.KEY_LEFT)) {
        حالة["لاعب_x"] -= حالة["سرعة"];
    }
    إذا (RL.is_key_down(RL.KEY_DOWN)) {
        حالة["لاعب_y"] += حالة["سرعة"];
    }
    إذا (RL.is_key_down(RL.KEY_UP)) {
        حالة["لاعب_y"] -= حالة["سرعة"];
    }
    
    // Keep player on screen
    إذا (حالة["لاعب_x"] < 0) حالة["لاعب_x"] = 0;
    إذا (حالة["لاعب_x"] > 780) حالة["لاعب_x"] = 780;
    إذا (حالة["لاعب_y"] < 0) حالة["لاعب_y"] = 0;
    إذا (حالة["لاعب_y"] > 580) حالة["لاعب_y"] = 580;
}

/**
 * Draw game
 */
دالة رسم() {
    RL.begin_drawing();
    RL.clear_background(RL.RAYWHITE);
    
    // Draw player
    RL.draw_rectangle(
        حالة["لاعب_x"],
        حالة["لاعب_y"],
        20,
        20,
        RL.BLUE
    );
    
    // Draw UI
    RL.draw_text("{{PROJECT_NAME}}", 10, 10, 20, RL.DARKGRAY);
    RL.draw_text("Score: " + نص(حالة["نتيجة"]), 10, 40, 20, RL.DARKGRAY);
    RL.draw_text("Use arrow keys to move", 10, 570, 16, RL.GRAY);
    
    RL.end_drawing();
}

/**
 * Cleanup
 */
دالة تنظيف() {
    RL.close_window();
}

/**
 * Main game loop
 */
دالة رئيسية() {
    تهيئة();
    
    // Game loop
    بينما (!RL.window_should_close()) {
        تحديث();
        رسم();
    }
    
    تنظيف();
    أرجع 0;
}
