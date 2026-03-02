#include "Z_Log.h"
#include "scene/main/scene_tree.h"
#include "scene/main/viewport.h"
#include "core/config/engine.h"
#include "core/os/time.h"

// Static member definitions
RichTextLabel *Z_Log::debug_label = nullptr;
bool Z_Log::is_initialized = false;

void Z_Log::_bind_methods() {
    // Main print function
    ClassDB::bind_static_method("Z_Log", D_METHOD("print", "text", "color", "duration"), &Z_Log::print, DEFVAL(Color::from_rgba8(255,255,255,255)), DEFVAL(2.0f));

    // Convenience functions
    ClassDB::bind_static_method("Z_Log", D_METHOD("print_warning", "text", "duration"), &Z_Log::print_warning, DEFVAL(3.0f));
    ClassDB::bind_static_method("Z_Log", D_METHOD("print_error", "text", "duration"), &Z_Log::print_error, DEFVAL(5.0f));
    ClassDB::bind_static_method("Z_Log", D_METHOD("print_success", "text", "duration"), &Z_Log::print_success, DEFVAL(2.0f));

    // Control functions
    ClassDB::bind_static_method("Z_Log", D_METHOD("clear_all"), &Z_Log::clear_all);
    ClassDB::bind_static_method("Z_Log", D_METHOD("set_position", "x", "y"), &Z_Log::set_position);
    ClassDB::bind_static_method("Z_Log", D_METHOD("set_font_size", "size"), &Z_Log::set_font_size);
}

void Z_Log::_initialize_debug_label() {
    if (is_initialized || Engine::get_singleton()->is_editor_hint()) {
        return;
    }

	// Get the main viewport
	SceneTree *scene_tree = cast_to<SceneTree>(OS::get_singleton()->get_main_loop());
	if (!scene_tree) {
		return;
	}

    if (!scene_tree) {
        return;
    }

    Viewport *viewport = scene_tree->get_root();
    if (!viewport) {
        return;
    }

    // Create the debug label
    debug_label = memnew(RichTextLabel);
    debug_label->set_name("Z_Log_DebugLabel");
    debug_label->set_anchors_and_offsets_preset(Control::PRESET_TOP_LEFT);
    debug_label->set_position(Vector2(10, 10));
    debug_label->set_size(Vector2(800, 600));
    debug_label->set_use_bbcode(true);
    debug_label->set_fit_content(true);
    debug_label->set_scroll_active(false);
    debug_label->set_mouse_filter(Control::MOUSE_FILTER_IGNORE);

    // Make it stay on top
    debug_label->set_z_index(1000);

    // Add to viewport
    viewport->add_child(debug_label);

    is_initialized = true;
}

void Z_Log::print(const String &text, const Color &color, float duration) {
    // Also print to console
    print_line(vformat("[Z_Log] %s", text));

    if (!is_initialized) {
        _initialize_debug_label();
    }

    if (!debug_label) {
        return;
    }

    // Convert color to hex for BBCode
    String color_hex = String("#") + color.to_html(false);

    // Get current time for cleanup
    uint64_t current_time = Time::get_singleton()->get_ticks_msec();
    uint64_t expire_time = current_time + (uint64_t)(duration * 1000);

    // Format the message with BBCode
    String formatted_message = vformat("[color=%s]%s[/color]\n", color_hex, text);

    // Add timestamp as metadata (hidden)
    formatted_message += vformat("[color=#00000000][font_size=1]%d[/font_size][/color]", expire_time);

    // Append to existing text
    String current_text = debug_label->get_text();
    debug_label->set_text(current_text + formatted_message + "\n");

    // Clean up old messages
    _cleanup_expired_messages();
}

void Z_Log::print_warning(const String &text, float duration) {
    print("[WARNING] " + text, Color::from_rgba8(255,255,255), duration);
}

void Z_Log::print_error(const String &text, float duration) {
    print("[ERROR] " + text, Color::from_rgba8(255,255,255), duration);
}

void Z_Log::print_success(const String &text, float duration) {
    print("[SUCCESS] " + text, Color::from_rgba8(255,255,255), duration);
}

void Z_Log::clear_all() {
    if (debug_label) {
        debug_label->set_text("");
    }
}

void Z_Log::set_position(int x, int y) {
    if (debug_label) {
        debug_label->set_position(Vector2(x, y));
    }
}

void Z_Log::set_font_size(int size) {
    if (debug_label) {
        // This would require creating a custom theme
        // For now, we can use BBCode font size in messages
    }
}

void Z_Log::_cleanup_expired_messages() {
    if (!debug_label) {
        return;
    }

    uint64_t current_time = Time::get_singleton()->get_ticks_msec();
    String current_text = debug_label->get_text();

    PackedStringArray lines = current_text.split("\n");
    String new_text = "";

    for (int i = 0; i < lines.size(); i++) {
        String line = lines[i];

        // Skip empty lines
        if (line.is_empty()) {
            continue;
        }

        // Look for timestamp in hidden text
        int timestamp_start = line.rfind("[font_size=1]");
        if (timestamp_start != -1) {
            int timestamp_end = line.rfind("[/font_size]");
            if (timestamp_end != -1) {
                String timestamp_str = line.substr(timestamp_start + 14, timestamp_end - timestamp_start - 14);
                uint64_t expire_time = timestamp_str.to_int();

                // If not expired, keep the line
                if (current_time < expire_time) {
                    new_text += line + "\n";
                }
                // If expired, skip this line
                continue;
            }
        }

        // Lines without timestamps (shouldn't happen, but keep them)
        new_text += line + "\n";
    }

    // Only update if text changed
    if (new_text != current_text) {
        debug_label->set_text(new_text);
    }
}

void Z_Log::cleanup() {
    if (debug_label && is_initialized) {
        debug_label->queue_free();
        debug_label = nullptr;
        is_initialized = false;
    }
}
