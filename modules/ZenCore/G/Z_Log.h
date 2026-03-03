#ifndef ZLOG_H
#define ZLOG_H

#include "core/object/ref_counted.h"
#include "core/variant/variant.h"
#include "core/math/color.h"
#include "scene/main/window.h"
#include "scene/gui/rich_text_label.h"


class Z_Log : public RefCounted {
	GDCLASS(Z_Log, RefCounted);
private:
	static RichTextLabel *debug_label;
	static bool is_initialized;

	static void _initialize_debug_label();
	static void _cleanup_expired_messages();

protected:
	static void _bind_methods();

public:
	// Main print function (like UE5's Print String)
	static void print(const String &text, const Color &color = Color::from_ok_hsl(1,1,1,1), float duration = 2.0f);

	// Convenience functions with preset colors
	static void print_warning(const String &text, float duration = 3.0f);
	static void print_error(const String &text, float duration = 5.0f);
	static void print_success(const String &text, float duration = 2.0f);

	// Control functions
	static void clear_all();
	static void set_position(int x, int y);
	static void set_font_size(int size);

	// Cleanup
	static void cleanup();

};

#endif
