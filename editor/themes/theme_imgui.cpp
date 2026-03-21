/**************************************************************************/
/*  theme_imgui.cpp                                                       */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "theme_imgui.h"

#include "core/math/math_defs.h"
#include "editor/editor_string_names.h"
#include "editor/settings/editor_settings.h"
#include "editor/themes/editor_scale.h"
#include "editor/themes/editor_theme_manager.h"
#include "scene/gui/graph_edit.h"
#include "scene/resources/dpi_texture.h"
#include "scene/resources/image_texture.h"
#include "scene/resources/style_box_flat.h"
#include "scene/resources/style_box_line.h"

// =============================================================================
// ImGui Color Palette
// =============================================================================
// ImGui's signature look comes from its distinctive color palette:
// - Very dark backgrounds (around 0.06-0.11 gray)
// - Blue accent color for interactive elements
// - High contrast white/light gray text
// - Subtle frame borders
// =============================================================================

// ImGui-style colors - these define the characteristic look
namespace ImGuiColors {
	// Background colors (very dark grays)
	const Color window_bg = Color(0.06f, 0.06f, 0.06f, 1.0f);         // Main window background
	const Color child_bg = Color(0.07f, 0.07f, 0.07f, 1.0f);          // Child window/panel background
	const Color popup_bg = Color(0.08f, 0.08f, 0.08f, 0.94f);         // Popup/tooltip background
	const Color frame_bg = Color(0.16f, 0.29f, 0.48f, 0.54f);         // Input field background (blue tint)
	const Color frame_bg_hovered = Color(0.26f, 0.59f, 0.98f, 0.40f); // Input field hovered
	const Color frame_bg_active = Color(0.26f, 0.59f, 0.98f, 0.67f);  // Input field active

	// Title bar colors
	const Color title_bg = Color(0.04f, 0.04f, 0.04f, 1.0f);          // Inactive title bar
	const Color title_bg_active = Color(0.16f, 0.29f, 0.48f, 1.0f);   // Active title bar (blue)
	const Color title_bg_collapsed = Color(0.0f, 0.0f, 0.0f, 0.51f);  // Collapsed title bar

	// Menu bar
	const Color menu_bar_bg = Color(0.14f, 0.14f, 0.14f, 1.0f);

	// Scrollbar colors
	const Color scrollbar_bg = Color(0.02f, 0.02f, 0.02f, 0.53f);
	const Color scrollbar_grab = Color(0.31f, 0.31f, 0.31f, 1.0f);
	const Color scrollbar_grab_hovered = Color(0.41f, 0.41f, 0.41f, 1.0f);
	const Color scrollbar_grab_active = Color(0.51f, 0.51f, 0.51f, 1.0f);

	// Button colors (blue accent)
	const Color button = Color(0.26f, 0.59f, 0.98f, 0.40f);
	const Color button_hovered = Color(0.26f, 0.59f, 0.98f, 1.0f);
	const Color button_active = Color(0.06f, 0.53f, 0.98f, 1.0f);

	// Header colors (tree nodes, collapsibles)
	const Color header = Color(0.26f, 0.59f, 0.98f, 0.31f);
	const Color header_hovered = Color(0.26f, 0.59f, 0.98f, 0.80f);
	const Color header_active = Color(0.26f, 0.59f, 0.98f, 1.0f);

	// Separators and borders
	const Color separator = Color(0.43f, 0.43f, 0.50f, 0.50f);
	const Color border = Color(0.43f, 0.43f, 0.50f, 0.50f);
	const Color border_shadow = Color(0.0f, 0.0f, 0.0f, 0.0f);

	// Resize grip
	const Color resize_grip = Color(0.26f, 0.59f, 0.98f, 0.20f);
	const Color resize_grip_hovered = Color(0.26f, 0.59f, 0.98f, 0.67f);
	const Color resize_grip_active = Color(0.26f, 0.59f, 0.98f, 0.95f);

	// Tab colors
	const Color tab = Color(0.18f, 0.35f, 0.58f, 0.86f);
	const Color tab_hovered = Color(0.26f, 0.59f, 0.98f, 0.80f);
	const Color tab_active = Color(0.20f, 0.41f, 0.68f, 1.0f);
	const Color tab_unfocused = Color(0.07f, 0.10f, 0.15f, 0.97f);
	const Color tab_unfocused_active = Color(0.14f, 0.26f, 0.42f, 1.0f);

	// Slider colors
	const Color slider_grab = Color(0.24f, 0.52f, 0.88f, 1.0f);
	const Color slider_grab_active = Color(0.26f, 0.59f, 0.98f, 1.0f);

	// Check mark
	const Color check_mark = Color(0.26f, 0.59f, 0.98f, 1.0f);

	// Text colors
	const Color text = Color(1.0f, 1.0f, 1.0f, 1.0f);
	const Color text_disabled = Color(0.50f, 0.50f, 0.50f, 1.0f);
	const Color text_selected_bg = Color(0.26f, 0.59f, 0.98f, 0.35f);

	// Accent color (the signature ImGui blue)
	const Color accent = Color(0.26f, 0.59f, 0.98f, 1.0f);

	// Status colors
	const Color success = Color(0.45f, 0.95f, 0.5f, 1.0f);
	const Color warning = Color(1.0f, 0.87f, 0.4f, 1.0f);
	const Color error = Color(1.0f, 0.47f, 0.42f, 1.0f);
}

// Helper to create colors relative to ImGui base
static Color _imgui_color(float brightness_offset = 0.0f, float alpha = 1.0f) {
	float base = CLAMP(0.06f + brightness_offset * 0.1f, 0.0f, 1.0f);
	return Color(base, base, base, alpha);
}

// Helper to blend with accent color
static Color _imgui_accent_blend(const Color &base, float amount) {
	return base.lerp(ImGuiColors::accent, amount);
}

void ThemeImGui::populate_shared_styles(const Ref<EditorTheme> &p_theme, EditorThemeManager::ThemeConfiguration &p_config) {
	// ==========================================================================
	// COLORS
	// ==========================================================================
	{
		// Base colors - Override with ImGui palette
		p_theme->set_color("base_color", EditorStringName(Editor), ImGuiColors::window_bg);
		p_theme->set_color("accent_color", EditorStringName(Editor), ImGuiColors::accent);

		// ImGui is always dark theme
		p_config.mono_color = Color(1, 1, 1);
		p_config.mono_color_font = Color(1, 1, 1);
		p_config.mono_color_inv = Color(0, 0, 0);

		// Dark color variants (ImGui style)
		p_config.dark_color_1 = ImGuiColors::child_bg;
		p_config.dark_color_2 = _imgui_color(0.5f, 0.3f);  // Slightly lighter, semi-transparent
		p_config.dark_color_3 = _imgui_color(1.0f);        // Lighter for contrast

		// Contrast colors
		p_config.contrast_color_1 = ImGuiColors::border;
		p_config.contrast_color_2 = ImGuiColors::border.lightened(0.2f);

		// Highlight colors (selection)
		p_config.highlight_color = ImGuiColors::text_selected_bg;
		p_config.highlight_disabled_color = p_config.highlight_color.lerp(Color(0, 0, 0), 0.5f);

		// Status colors
		p_config.success_color = ImGuiColors::success;
		p_config.warning_color = ImGuiColors::warning;
		p_config.error_color = ImGuiColors::error;

		// Dark background versions for 3D editor
		p_theme->set_color("success_color_dark_background", EditorStringName(Editor), ImGuiColors::success);
		p_theme->set_color("warning_color_dark_background", EditorStringName(Editor), ImGuiColors::warning);
		p_theme->set_color("error_color_dark_background", EditorStringName(Editor), ImGuiColors::error);

		p_theme->set_color("mono_color", EditorStringName(Editor), p_config.mono_color);
		p_theme->set_color("dark_color_1", EditorStringName(Editor), p_config.dark_color_1);
		p_theme->set_color("dark_color_2", EditorStringName(Editor), p_config.dark_color_2);
		p_theme->set_color("dark_color_3", EditorStringName(Editor), p_config.dark_color_3);
		p_theme->set_color("contrast_color_1", EditorStringName(Editor), p_config.contrast_color_1);
		p_theme->set_color("contrast_color_2", EditorStringName(Editor), p_config.contrast_color_2);
		p_theme->set_color("highlight_color", EditorStringName(Editor), p_config.highlight_color);
		p_theme->set_color("highlight_disabled_color", EditorStringName(Editor), p_config.highlight_disabled_color);
		p_theme->set_color("success_color", EditorStringName(Editor), p_config.success_color);
		p_theme->set_color("warning_color", EditorStringName(Editor), p_config.warning_color);
		p_theme->set_color("error_color", EditorStringName(Editor), p_config.error_color);
		p_theme->set_color("ruler_color", EditorStringName(Editor), ImGuiColors::border);

#ifndef DISABLE_DEPRECATED
		p_theme->set_color("disabled_highlight_color", EditorStringName(Editor), p_config.highlight_disabled_color);
#endif

		// Extra border colors
		p_config.extra_border_color_1 = ImGuiColors::border;
		p_config.extra_border_color_2 = ImGuiColors::border.darkened(0.3f);

		p_theme->set_color("extra_border_color_1", EditorStringName(Editor), p_config.extra_border_color_1);
		p_theme->set_color("extra_border_color_2", EditorStringName(Editor), p_config.extra_border_color_2);

		// Font colors - High contrast white on dark backgrounds
		p_config.font_color = ImGuiColors::text * Color(1, 1, 1, 0.87f);
		p_config.font_secondary_color = ImGuiColors::text * Color(1, 1, 1, 0.60f);
		p_config.font_focus_color = ImGuiColors::text;
		p_config.font_hover_color = ImGuiColors::text;
		p_config.font_pressed_color = ImGuiColors::accent;
		p_config.font_hover_pressed_color = ImGuiColors::accent.lightened(0.2f);
		p_config.font_disabled_color = ImGuiColors::text_disabled;
		p_config.font_readonly_color = ImGuiColors::text * Color(1, 1, 1, 0.65f);
		p_config.font_placeholder_color = ImGuiColors::text_disabled;
		p_config.font_outline_color = Color(0, 0, 0, 0);

		// Dark background font colors
		p_config.font_dark_background_color = p_config.font_color;
		p_config.font_dark_background_focus_color = p_config.font_focus_color;
		p_config.font_dark_background_hover_color = p_config.font_hover_color;
		p_config.font_dark_background_pressed_color = p_config.font_pressed_color;
		p_config.font_dark_background_hover_pressed_color = p_config.font_hover_pressed_color;

		p_theme->set_color(SceneStringName(font_color), EditorStringName(Editor), p_config.font_color);
		p_theme->set_color("font_focus_color", EditorStringName(Editor), p_config.font_focus_color);
		p_theme->set_color("font_hover_color", EditorStringName(Editor), p_config.font_hover_color);
		p_theme->set_color("font_pressed_color", EditorStringName(Editor), p_config.font_pressed_color);
		p_theme->set_color("font_hover_pressed_color", EditorStringName(Editor), p_config.font_hover_pressed_color);
		p_theme->set_color("font_disabled_color", EditorStringName(Editor), p_config.font_disabled_color);
		p_theme->set_color("font_readonly_color", EditorStringName(Editor), p_config.font_readonly_color);
		p_theme->set_color("font_placeholder_color", EditorStringName(Editor), p_config.font_placeholder_color);
		p_theme->set_color("font_outline_color", EditorStringName(Editor), p_config.font_outline_color);

		p_theme->set_color("font_dark_background_color", EditorStringName(Editor), p_config.font_dark_background_color);
		p_theme->set_color("font_dark_background_focus_color", EditorStringName(Editor), p_config.font_dark_background_focus_color);
		p_theme->set_color("font_dark_background_hover_color", EditorStringName(Editor), p_config.font_dark_background_hover_color);
		p_theme->set_color("font_dark_background_pressed_color", EditorStringName(Editor), p_config.font_dark_background_pressed_color);
		p_theme->set_color("font_dark_background_hover_pressed_color", EditorStringName(Editor), p_config.font_dark_background_hover_pressed_color);

#ifndef DISABLE_DEPRECATED
		p_theme->set_color("readonly_font_color", EditorStringName(Editor), p_config.font_readonly_color);
		p_theme->set_color("disabled_font_color", EditorStringName(Editor), p_config.font_disabled_color);
		p_theme->set_color("readonly_color", EditorStringName(Editor), p_config.font_readonly_color);
		p_theme->set_color("highlighted_font_color", EditorStringName(Editor), p_config.font_hover_color);
#endif

		// Icon colors
		p_config.icon_normal_color = Color(1, 1, 1, 0.9f);
		p_config.icon_secondary_color = Color(1, 1, 1, 0.65f);
		p_config.icon_focus_color = Color(1, 1, 1, 1.0f);
		p_config.icon_hover_color = Color(1, 1, 1, 1.0f);
		p_config.icon_pressed_color = ImGuiColors::accent * 1.15f;
		p_config.icon_pressed_color.a = 1.0f;
		p_config.icon_disabled_color = Color(1, 1, 1, 0.4f);

		p_theme->set_color("icon_normal_color", EditorStringName(Editor), p_config.icon_normal_color);
		p_theme->set_color("icon_focus_color", EditorStringName(Editor), p_config.icon_focus_color);
		p_theme->set_color("icon_hover_color", EditorStringName(Editor), p_config.icon_hover_color);
		p_theme->set_color("icon_pressed_color", EditorStringName(Editor), p_config.icon_pressed_color);
		p_theme->set_color("icon_disabled_color", EditorStringName(Editor), p_config.icon_disabled_color);

		// Additional GUI colors - ImGui style
		p_config.surface_popup_color = ImGuiColors::popup_bg;
		p_config.surface_lowest_color = _imgui_color(0.0f);   // Darkest
		p_config.surface_lower_color = _imgui_color(0.2f);
		p_config.surface_low_color = _imgui_color(0.4f);
		p_config.surface_base_color = _imgui_color(0.6f);
		p_config.surface_high_color = _imgui_color(0.8f);
		p_config.surface_higher_color = _imgui_color(1.0f);
		p_config.surface_highest_color = _imgui_color(1.5f);

		// Button colors - ImGui blue style
		p_config.button_normal_color = ImGuiColors::button;
		p_config.button_hover_color = ImGuiColors::button_hovered;
		p_config.button_pressed_color = ImGuiColors::button_active;
		p_config.button_disabled_color = ImGuiColors::button * Color(1, 1, 1, 0.3f);
		p_config.button_border_normal_color = ImGuiColors::border;
		p_config.button_border_hover_color = ImGuiColors::accent * Color(1, 1, 1, 0.8f);
		p_config.button_border_pressed_color = ImGuiColors::accent;

		// Flat button colors
		p_config.flat_button_hover_color = ImGuiColors::header;
		p_config.flat_button_pressed_color = ImGuiColors::header_active;
		p_config.flat_button_hover_pressed_color = ImGuiColors::header_hovered;

		p_config.shadow_color = Color(0, 0, 0, 0.3f);
		p_config.selection_color = ImGuiColors::text_selected_bg;
		p_config.disabled_border_color = ImGuiColors::border * Color(1, 1, 1, 0.5f);
		p_config.disabled_bg_color = _imgui_color(0.3f, 0.5f);
		p_config.separator_color = ImGuiColors::separator;

		p_theme->set_color("selection_color", EditorStringName(Editor), p_config.selection_color);
		p_theme->set_color("disabled_border_color", EditorStringName(Editor), p_config.disabled_border_color);
		p_theme->set_color("disabled_bg_color", EditorStringName(Editor), p_config.disabled_bg_color);
		p_theme->set_color("separator_color", EditorStringName(Editor), p_config.separator_color);

		// Editor-specific colors
		p_theme->set_color("box_selection_fill_color", EditorStringName(Editor), p_config.mono_color * Color(1, 1, 1, 0.12f));
		p_theme->set_color("box_selection_stroke_color", EditorStringName(Editor), p_config.mono_color * Color(1, 1, 1, 0.4f));

		// Axis colors
		p_theme->set_color("axis_x_color", EditorStringName(Editor), Color(0.96f, 0.20f, 0.32f));
		p_theme->set_color("axis_y_color", EditorStringName(Editor), Color(0.53f, 0.84f, 0.01f));
		p_theme->set_color("axis_z_color", EditorStringName(Editor), Color(0.16f, 0.55f, 0.96f));
		p_theme->set_color("axis_w_color", EditorStringName(Editor), Color(0.55f, 0.55f, 0.55f));

		// Property colors
		p_theme->set_color("property_color_x", EditorStringName(Editor), Color(0.88f, 0.38f, 0.47f));
		p_theme->set_color("property_color_y", EditorStringName(Editor), Color(0.76f, 0.93f, 0.40f));
		p_theme->set_color("property_color_z", EditorStringName(Editor), Color(0.42f, 0.67f, 0.96f));
		p_theme->set_color("property_color_w", EditorStringName(Editor), p_config.font_color);

		// Rendering method colors
		p_theme->set_color("forward_plus_color", EditorStringName(Editor), Color::hex(0x5d8c3fff));
		p_theme->set_color("mobile_color", EditorStringName(Editor), Color::hex(0xa5557dff));
		p_theme->set_color("gl_compatibility_color", EditorStringName(Editor), Color::hex(0x5586a4ff));
	}

	// ==========================================================================
	// CONSTANTS
	// ==========================================================================
	{
		p_theme->set_color("icon_saturation", EditorStringName(Editor), Color(p_config.icon_saturation, p_config.icon_saturation, p_config.icon_saturation));

		p_theme->set_default_base_scale(EDSCALE);
		p_theme->set_constant("scale", EditorStringName(Editor), EDSCALE);

		p_theme->set_constant("thumb_size", EditorStringName(Editor), p_config.thumb_size);
		p_theme->set_constant("class_icon_size", EditorStringName(Editor), p_config.class_icon_size);
		p_theme->set_constant("gizmo_handle_scale", EditorStringName(Editor), p_config.gizmo_handle_scale);

		p_theme->set_constant("base_margin", EditorStringName(Editor), p_config.base_margin);
		p_theme->set_constant("increased_margin", EditorStringName(Editor), p_config.increased_margin);
		p_theme->set_constant("window_border_margin", EditorStringName(Editor), p_config.window_border_margin);
		p_theme->set_constant("top_bar_separation", EditorStringName(Editor), p_config.top_bar_separation);

		p_theme->set_constant("dark_theme", EditorStringName(Editor), 1); // ImGui is always dark
	}

	// ==========================================================================
	// STYLEBOXES
	// ==========================================================================
	{
		// ImGui uses very minimal corner radius (0-2px typically)
		const int imgui_corner_radius = 2;

		// Base style - ImGui window background
		p_config.base_style = EditorThemeManager::make_flat_stylebox(
			ImGuiColors::child_bg,
			p_config.increased_margin * 1.5f, p_config.increased_margin * 1.5f,
			p_config.increased_margin * 1.5f, p_config.increased_margin * 1.5f,
			imgui_corner_radius
		);

		// Focus style - Blue outline, ImGui style
		p_config.focus_style = p_config.base_style->duplicate();
		p_config.focus_style->set_draw_center(false);
		p_config.focus_style->set_border_color(ImGuiColors::accent * Color(1, 1, 1, 0.8f));
		p_config.focus_style->set_border_width_all(2);

		p_config.base_empty_style = EditorThemeManager::make_empty_stylebox();

		p_config.base_empty_wide_style = EditorThemeManager::make_empty_stylebox();
		float base_empty_wide_margin = MAX(p_config.base_margin, 3.0f);
		p_config.base_empty_wide_style->set_content_margin_individual(
			base_empty_wide_margin * 1.5f * EDSCALE, base_empty_wide_margin * EDSCALE,
			base_empty_wide_margin * 1.5f * EDSCALE, base_empty_wide_margin * EDSCALE
		);

		// Button styles - ImGui blue buttons
		{
			p_config.widget_margin = Vector2(p_config.increased_margin + 2, p_config.increased_margin + 1) * EDSCALE;

			p_config.button_style = p_config.base_style->duplicate();
			p_config.button_style->set_content_margin_individual(
				p_config.base_margin * 2 * EDSCALE, p_config.base_margin * 1.5f * EDSCALE,
				p_config.base_margin * 2 * EDSCALE, p_config.base_margin * 1.5f * EDSCALE
			);
			p_config.button_style->set_bg_color(ImGuiColors::button);
			p_config.button_style->set_border_width_all(Math::round(EDSCALE));
			p_config.button_style->set_border_color(ImGuiColors::border);
			p_config.button_style->set_corner_radius_all(imgui_corner_radius);
			// No shadow for ImGui style
			p_config.button_style->set_shadow_size(0);

			p_config.button_style_disabled = p_config.button_style->duplicate();
			p_config.button_style_disabled->set_bg_color(p_config.button_disabled_color);
			p_config.button_style_disabled->set_border_color(ImGuiColors::border * Color(1, 1, 1, 0.5f));

			p_config.button_style_pressed = p_config.button_style->duplicate();
			p_config.button_style_pressed->set_bg_color(ImGuiColors::button_active);
			p_config.button_style_pressed->set_border_color(ImGuiColors::accent);

			p_config.button_style_hover = p_config.button_style->duplicate();
			p_config.button_style_hover->set_bg_color(ImGuiColors::button_hovered);
			p_config.button_style_hover->set_border_color(ImGuiColors::accent * Color(1, 1, 1, 0.8f));

			// Flat buttons (for tree items, etc.)
			p_config.flat_button_hover = p_config.base_style->duplicate();
			p_config.flat_button_hover->set_bg_color(ImGuiColors::header);
			p_config.flat_button_hover->set_content_margin_individual(
				p_config.base_margin * 1.5f * EDSCALE, p_config.base_margin * 0.9f * EDSCALE,
				p_config.base_margin * 1.5f * EDSCALE, p_config.base_margin * 0.9f * EDSCALE
			);
			p_config.flat_button_hover->set_corner_radius_all(imgui_corner_radius);

			p_config.flat_button_pressed = p_config.flat_button_hover->duplicate();
			p_config.flat_button_pressed->set_bg_color(ImGuiColors::header_active);

			p_config.flat_button_hover_pressed = p_config.flat_button_hover->duplicate();
			p_config.flat_button_hover_pressed->set_bg_color(ImGuiColors::header_hovered);

			p_config.flat_button = p_config.flat_button_hover->duplicate();
			p_config.flat_button->set_draw_center(false);
		}

		// Windows and popups - ImGui style
		{
			p_config.popup_panel_style = p_config.base_style->duplicate();
			p_config.popup_panel_style->set_bg_color(ImGuiColors::popup_bg);
			p_config.popup_panel_style->set_shadow_color(Color(0, 0, 0, 0.3f));
			p_config.popup_panel_style->set_shadow_size(p_config.base_margin * 0.75f * EDSCALE);
			p_config.popup_panel_style->set_content_margin_all(p_config.popup_margin * EDSCALE);
			p_config.popup_panel_style->set_corner_radius_all(0); // ImGui uses square popups
			p_config.popup_panel_style->set_border_width_all(Math::round(EDSCALE));
			p_config.popup_panel_style->set_border_color(ImGuiColors::border);

			// Window style with ImGui blue title bar
			p_config.window_style = p_config.base_style->duplicate();
			p_config.window_style->set_bg_color(ImGuiColors::window_bg);
			p_config.window_style->set_content_margin_all(p_config.popup_margin);
			p_config.window_style->set_shadow_color(p_config.shadow_color);
			p_config.window_style->set_shadow_size(4 * EDSCALE);
			p_config.window_style->set_border_color(ImGuiColors::title_bg_active); // Blue top border
			p_config.window_style->set_border_width(SIDE_TOP, 24 * EDSCALE);
			p_config.window_style->set_expand_margin(SIDE_TOP, 24 * EDSCALE);
			p_config.window_style->set_corner_radius_all(0);

			p_config.window_complex_style = p_config.window_style->duplicate();
			p_config.window_complex_style->set_bg_color(ImGuiColors::window_bg);

			// Dialog style
			p_config.dialog_style = p_config.base_style->duplicate();
			p_config.dialog_style->set_bg_color(ImGuiColors::popup_bg);
			p_config.dialog_style->set_content_margin_all(p_config.popup_margin);
			p_config.dialog_style->set_corner_radius_all(0);
		}

		// Panels
		{
			p_config.panel_container_style = p_config.button_style->duplicate();
			p_config.panel_container_style->set_draw_center(false);
			p_config.panel_container_style->set_border_width_all(0);

			// Content panel for tabs
			const int content_panel_margin = p_config.base_margin * EDSCALE + p_config.border_width;

			p_config.content_panel_style = p_config.base_style->duplicate();
			p_config.content_panel_style->set_bg_color(ImGuiColors::child_bg);
			p_config.content_panel_style->set_border_color(ImGuiColors::border);
			p_config.content_panel_style->set_border_width_all(p_config.border_width);
			p_config.content_panel_style->set_border_width(Side::SIDE_TOP, 0);
			p_config.content_panel_style->set_corner_radius(CORNER_TOP_LEFT, 0);
			p_config.content_panel_style->set_corner_radius(CORNER_TOP_RIGHT, 0);
			p_config.content_panel_style->set_content_margin_individual(
				content_panel_margin, 2 * EDSCALE + content_panel_margin,
				content_panel_margin, content_panel_margin
			);

			p_config.tab_container_style = p_config.base_style->duplicate();
			p_config.tab_container_style->set_bg_color(ImGuiColors::child_bg);
			p_config.tab_container_style->set_content_margin_all(p_config.increased_margin * 1.5f * EDSCALE);
			p_config.tab_container_style->set_corner_radius_individual(0, 0, imgui_corner_radius * EDSCALE, imgui_corner_radius * EDSCALE);

			p_config.foreground_panel = p_config.tab_container_style->duplicate();
			p_config.foreground_panel->set_corner_radius(CORNER_TOP_LEFT, p_config.tab_container_style->get_corner_radius(CORNER_BOTTOM_LEFT));
			p_config.foreground_panel->set_corner_radius(CORNER_TOP_RIGHT, p_config.tab_container_style->get_corner_radius(CORNER_BOTTOM_RIGHT));

			// Tree panel - darker background for inset feeling
			p_config.tree_panel_style = p_config.base_style->duplicate();
			p_config.tree_panel_style->set_bg_color(ImGuiColors::frame_bg);
			p_config.tree_panel_style->set_border_color(ImGuiColors::border);
			p_config.tree_panel_style->set_corner_radius_all(imgui_corner_radius);
			if (p_config.draw_extra_borders) {
				p_config.tree_panel_style->set_border_width_all(Math::round(EDSCALE));
			}
		}
	}
}

void ThemeImGui::populate_standard_styles(const Ref<EditorTheme> &p_theme, EditorThemeManager::ThemeConfiguration &p_config) {
	const int imgui_corner_radius = 2;

	// ==========================================================================
	// PANELS
	// ==========================================================================
	{
		// Panel
		p_theme->set_stylebox(SceneStringName(panel), "Panel",
			EditorThemeManager::make_flat_stylebox(ImGuiColors::child_bg, 6, 4, 6, 4, imgui_corner_radius));

		// PanelContainer
		p_theme->set_stylebox(SceneStringName(panel), "PanelContainer", p_config.base_empty_wide_style);

		// TooltipPanel & TooltipLabel
		{
			p_theme->set_color(SceneStringName(font_color), "TooltipLabel", p_config.font_hover_color);
			p_theme->set_color("font_shadow_color", "TooltipLabel", Color(0, 0, 0, 0));

			Ref<StyleBoxFlat> tooltip_style = p_config.base_style->duplicate();
			tooltip_style->set_bg_color(ImGuiColors::popup_bg);
			tooltip_style->set_content_margin_all(p_config.base_margin * 0.75f * EDSCALE);
			tooltip_style->set_corner_radius_all(0);
			tooltip_style->set_border_width_all(Math::round(EDSCALE));
			tooltip_style->set_border_color(ImGuiColors::border);
			p_theme->set_stylebox(SceneStringName(panel), "TooltipPanel", tooltip_style);
		}

		// PopupPanel
		p_theme->set_stylebox(SceneStringName(panel), "PopupPanel", p_config.popup_panel_style);
	}

	// ==========================================================================
	// BUTTONS
	// ==========================================================================
	{
		// Button - ImGui blue style
		p_theme->set_stylebox(CoreStringName(normal), "Button", p_config.button_style);
		p_theme->set_stylebox(SceneStringName(hover), "Button", p_config.button_style_hover);
		p_theme->set_stylebox(SceneStringName(pressed), "Button", p_config.button_style_pressed);
		p_theme->set_stylebox("hover_pressed", "Button", p_config.button_style_pressed);
		p_theme->set_stylebox("focus", "Button", p_config.focus_style);
		p_theme->set_stylebox("disabled", "Button", p_config.button_style_disabled);

		p_theme->set_stylebox("normal_mirrored", "Button", p_config.button_style);
		p_theme->set_stylebox("hover_mirrored", "Button", p_config.button_style_hover);
		p_theme->set_stylebox("pressed_mirrored", "Button", p_config.button_style_pressed);
		p_theme->set_stylebox("hover_pressed_mirrored", "Button", p_config.button_style_pressed);
		p_theme->set_stylebox("disabled_mirrored", "Button", p_config.button_style_disabled);

		p_theme->set_color(SceneStringName(font_color), "Button", p_config.font_color);
		p_theme->set_color("font_hover_color", "Button", p_config.font_hover_color);
		p_theme->set_color("font_hover_pressed_color", "Button", p_config.font_hover_pressed_color);
		p_theme->set_color("font_focus_color", "Button", p_config.font_focus_color);
		p_theme->set_color("font_pressed_color", "Button", p_config.font_pressed_color);
		p_theme->set_color("font_disabled_color", "Button", p_config.font_disabled_color);
		p_theme->set_color("font_outline_color", "Button", p_config.font_outline_color);

		p_theme->set_color("icon_normal_color", "Button", p_config.icon_normal_color);
		p_theme->set_color("icon_hover_color", "Button", p_config.icon_hover_color);
		p_theme->set_color("icon_focus_color", "Button", p_config.icon_focus_color);
		p_theme->set_color("icon_hover_pressed_color", "Button", p_config.icon_pressed_color);
		p_theme->set_color("icon_pressed_color", "Button", p_config.icon_pressed_color);
		p_theme->set_color("icon_disabled_color", "Button", p_config.icon_disabled_color);

		p_theme->set_constant("h_separation", "Button", 4 * EDSCALE);
		p_theme->set_constant("outline_size", "Button", 0);
		p_theme->set_constant("align_to_largest_stylebox", "Button", 1);

		// MenuBar
		p_theme->set_stylebox(CoreStringName(normal), "MenuBar", p_config.button_style);
		p_theme->set_stylebox(SceneStringName(hover), "MenuBar", p_config.button_style_hover);
		p_theme->set_stylebox(SceneStringName(pressed), "MenuBar", p_config.button_style_pressed);
		p_theme->set_stylebox("disabled", "MenuBar", p_config.button_style_disabled);

		p_theme->set_color(SceneStringName(font_color), "MenuBar", p_config.font_color);
		p_theme->set_color("font_hover_color", "MenuBar", p_config.font_hover_color);
		p_theme->set_color("font_hover_pressed_color", "MenuBar", p_config.font_hover_pressed_color);
		p_theme->set_color("font_focus_color", "MenuBar", p_config.font_focus_color);
		p_theme->set_color("font_pressed_color", "MenuBar", p_config.font_pressed_color);
		p_theme->set_color("font_disabled_color", "MenuBar", p_config.font_disabled_color);
		p_theme->set_color("font_outline_color", "MenuBar", p_config.font_outline_color);

		// MenuButton
		p_theme->set_stylebox(CoreStringName(normal), "MenuButton", p_config.flat_button);
		p_theme->set_stylebox(SceneStringName(hover), "MenuButton", p_config.flat_button_hover);
		p_theme->set_stylebox(SceneStringName(pressed), "MenuButton", p_config.flat_button_pressed);
		p_theme->set_stylebox("disabled", "MenuButton", p_config.flat_button);

		p_theme->set_color(SceneStringName(font_color), "MenuButton", p_config.font_color);
		p_theme->set_color("font_hover_color", "MenuButton", p_config.font_hover_color);
		p_theme->set_color("font_hover_pressed_color", "MenuButton", p_config.font_hover_pressed_color);
		p_theme->set_color("font_focus_color", "MenuButton", p_config.font_focus_color);
		p_theme->set_color("font_pressed_color", "MenuButton", p_config.font_pressed_color);
		p_theme->set_color("font_disabled_color", "MenuButton", p_config.font_disabled_color);
		p_theme->set_color("font_outline_color", "MenuButton", p_config.font_outline_color);

		// OptionButton
		p_theme->set_stylebox(CoreStringName(normal), "OptionButton", p_config.button_style);
		p_theme->set_stylebox(SceneStringName(hover), "OptionButton", p_config.button_style_hover);
		p_theme->set_stylebox(SceneStringName(pressed), "OptionButton", p_config.button_style_pressed);
		p_theme->set_stylebox("disabled", "OptionButton", p_config.button_style_disabled);
		p_theme->set_stylebox("focus", "OptionButton", p_config.focus_style);

		p_theme->set_stylebox("normal_mirrored", "OptionButton", p_config.button_style);
		p_theme->set_stylebox("hover_mirrored", "OptionButton", p_config.button_style_hover);
		p_theme->set_stylebox("pressed_mirrored", "OptionButton", p_config.button_style_pressed);
		p_theme->set_stylebox("disabled_mirrored", "OptionButton", p_config.button_style_disabled);

		p_theme->set_color(SceneStringName(font_color), "OptionButton", p_config.font_color);
		p_theme->set_color("font_hover_color", "OptionButton", p_config.font_hover_color);
		p_theme->set_color("font_hover_pressed_color", "OptionButton", p_config.font_hover_pressed_color);
		p_theme->set_color("font_focus_color", "OptionButton", p_config.font_focus_color);
		p_theme->set_color("font_pressed_color", "OptionButton", p_config.font_pressed_color);
		p_theme->set_color("font_disabled_color", "OptionButton", p_config.font_disabled_color);
		p_theme->set_color("font_outline_color", "OptionButton", p_config.font_outline_color);

		p_theme->set_color("icon_normal_color", "OptionButton", p_config.icon_normal_color);
		p_theme->set_color("icon_hover_color", "OptionButton", p_config.icon_hover_color);
		p_theme->set_color("icon_focus_color", "OptionButton", p_config.icon_focus_color);
		p_theme->set_color("icon_pressed_color", "OptionButton", p_config.icon_pressed_color);
		p_theme->set_color("icon_disabled_color", "OptionButton", p_config.icon_disabled_color);

		p_theme->set_icon("arrow", "OptionButton", p_theme->get_icon(SNAME("GuiOptionArrow"), EditorStringName(EditorIcons)));
		p_theme->set_constant("arrow_margin", "OptionButton", p_config.widget_margin.x);
		p_theme->set_constant("modulate_arrow", "OptionButton", 1);
		p_theme->set_constant("h_separation", "OptionButton", 4 * EDSCALE);
		p_theme->set_constant("outline_size", "OptionButton", 0);

		// CheckButton
		p_theme->set_stylebox(CoreStringName(normal), "CheckButton", p_config.flat_button);
		p_theme->set_stylebox(SceneStringName(hover), "CheckButton", p_config.flat_button);
		p_theme->set_stylebox(SceneStringName(pressed), "CheckButton", p_config.flat_button);
		p_theme->set_stylebox("disabled", "CheckButton", p_config.flat_button);
		p_theme->set_stylebox("hover_pressed", "CheckButton", p_config.flat_button);
		p_theme->set_stylebox("focus", "CheckButton", p_config.focus_style);

		p_theme->set_color(SceneStringName(font_color), "CheckButton", p_config.font_color);
		p_theme->set_color("font_hover_color", "CheckButton", p_config.font_hover_color);
		p_theme->set_color("font_hover_pressed_color", "CheckButton", p_config.font_hover_pressed_color);
		p_theme->set_color("font_focus_color", "CheckButton", p_config.font_focus_color);
		p_theme->set_color("font_pressed_color", "CheckButton", p_config.font_pressed_color);
		p_theme->set_color("font_disabled_color", "CheckButton", p_config.font_disabled_color);
		p_theme->set_color("font_outline_color", "CheckButton", p_config.font_outline_color);

		p_theme->set_color("icon_normal_color", "CheckButton", p_config.icon_normal_color);
		p_theme->set_color("icon_hover_color", "CheckButton", p_config.icon_hover_color);
		p_theme->set_color("icon_focus_color", "CheckButton", p_config.icon_focus_color);
		p_theme->set_color("icon_hover_pressed_color", "CheckButton", p_config.icon_pressed_color);
		p_theme->set_color("icon_pressed_color", "CheckButton", p_config.icon_pressed_color);
		p_theme->set_color("icon_disabled_color", "CheckButton", p_config.icon_disabled_color);

		p_theme->set_icon("checked", "CheckButton", p_theme->get_icon(SNAME("GuiToggleOn"), EditorStringName(EditorIcons)));
		p_theme->set_icon("checked_disabled", "CheckButton", p_theme->get_icon(SNAME("GuiToggleOnDisabled"), EditorStringName(EditorIcons)));
		p_theme->set_icon("unchecked", "CheckButton", p_theme->get_icon(SNAME("GuiToggleOff"), EditorStringName(EditorIcons)));
		p_theme->set_icon("unchecked_disabled", "CheckButton", p_theme->get_icon(SNAME("GuiToggleOffDisabled"), EditorStringName(EditorIcons)));
		p_theme->set_constant("h_separation", "CheckButton", 8 * EDSCALE);
		p_theme->set_constant("check_v_offset", "CheckButton", 0);
		p_theme->set_constant("outline_size", "CheckButton", 0);

		// CheckBox
		p_theme->set_stylebox(CoreStringName(normal), "CheckBox", p_config.flat_button);
		p_theme->set_stylebox(SceneStringName(hover), "CheckBox", p_config.flat_button);
		p_theme->set_stylebox(SceneStringName(pressed), "CheckBox", p_config.flat_button);
		p_theme->set_stylebox("disabled", "CheckBox", p_config.flat_button);
		p_theme->set_stylebox("hover_pressed", "CheckBox", p_config.flat_button);
		p_theme->set_stylebox("focus", "CheckBox", p_config.focus_style);

		p_theme->set_color(SceneStringName(font_color), "CheckBox", p_config.font_color);
		p_theme->set_color("font_hover_color", "CheckBox", p_config.font_hover_color);
		p_theme->set_color("font_hover_pressed_color", "CheckBox", p_config.font_hover_pressed_color);
		p_theme->set_color("font_focus_color", "CheckBox", p_config.font_focus_color);
		p_theme->set_color("font_pressed_color", "CheckBox", p_config.font_pressed_color);
		p_theme->set_color("font_disabled_color", "CheckBox", p_config.font_disabled_color);
		p_theme->set_color("font_outline_color", "CheckBox", p_config.font_outline_color);

		p_theme->set_color("icon_normal_color", "CheckBox", p_config.icon_normal_color);
		p_theme->set_color("icon_hover_color", "CheckBox", p_config.icon_hover_color);
		p_theme->set_color("icon_focus_color", "CheckBox", p_config.icon_focus_color);
		p_theme->set_color("icon_hover_pressed_color", "CheckBox", p_config.icon_pressed_color);
		p_theme->set_color("icon_pressed_color", "CheckBox", p_config.icon_pressed_color);
		p_theme->set_color("icon_disabled_color", "CheckBox", p_config.icon_disabled_color);

		p_theme->set_icon("checked", "CheckBox", p_theme->get_icon(SNAME("GuiChecked"), EditorStringName(EditorIcons)));
		p_theme->set_icon("checked_disabled", "CheckBox", p_theme->get_icon(SNAME("GuiCheckedDisabled"), EditorStringName(EditorIcons)));
		p_theme->set_icon("unchecked", "CheckBox", p_theme->get_icon(SNAME("GuiUnchecked"), EditorStringName(EditorIcons)));
		p_theme->set_icon("unchecked_disabled", "CheckBox", p_theme->get_icon(SNAME("GuiUncheckedDisabled"), EditorStringName(EditorIcons)));
		p_theme->set_icon("radio_checked", "CheckBox", p_theme->get_icon(SNAME("GuiRadioChecked"), EditorStringName(EditorIcons)));
		p_theme->set_icon("radio_checked_disabled", "CheckBox", p_theme->get_icon(SNAME("GuiRadioCheckedDisabled"), EditorStringName(EditorIcons)));
		p_theme->set_icon("radio_unchecked", "CheckBox", p_theme->get_icon(SNAME("GuiRadioUnchecked"), EditorStringName(EditorIcons)));
		p_theme->set_icon("radio_unchecked_disabled", "CheckBox", p_theme->get_icon(SNAME("GuiRadioUncheckedDisabled"), EditorStringName(EditorIcons)));
		p_theme->set_constant("h_separation", "CheckBox", 8 * EDSCALE);
		p_theme->set_constant("check_v_offset", "CheckBox", 0);
		p_theme->set_constant("outline_size", "CheckBox", 0);

		// LinkButton
		p_theme->set_stylebox("focus", "LinkButton", p_config.focus_style);

		p_theme->set_color(SceneStringName(font_color), "LinkButton", p_config.font_color);
		p_theme->set_color("font_hover_color", "LinkButton", p_config.font_hover_color);
		p_theme->set_color("font_hover_pressed_color", "LinkButton", p_config.font_hover_pressed_color);
		p_theme->set_color("font_focus_color", "LinkButton", p_config.font_focus_color);
		p_theme->set_color("font_pressed_color", "LinkButton", p_config.font_pressed_color);
		p_theme->set_color("font_disabled_color", "LinkButton", p_config.font_disabled_color);
		p_theme->set_color("font_outline_color", "LinkButton", p_config.font_outline_color);

		p_theme->set_constant("outline_size", "LinkButton", 0);
		p_theme->set_constant("underline_spacing", "LinkButton", 2 * EDSCALE);
	}

	// ==========================================================================
	// TREE & ITEM LIST
	// ==========================================================================
	{
		// Tree
		{
			Ref<StyleBoxEmpty> style_tree_panel = p_config.base_empty_style->duplicate();
			style_tree_panel->set_content_margin_individual(
				p_config.base_margin * 1.5f * EDSCALE, p_config.base_margin * 2.5f * EDSCALE,
				p_config.base_margin * 1.5f * EDSCALE, p_config.base_margin * 2.5f * EDSCALE
			);

			Ref<StyleBoxFlat> style_button_pressed = p_config.flat_button_pressed->duplicate();
			style_button_pressed->set_content_margin_individual(p_config.base_margin, 0, p_config.base_margin, 0);

			p_theme->set_icon("checked", "Tree", p_theme->get_icon(SNAME("GuiChecked"), EditorStringName(EditorIcons)));
			p_theme->set_icon("checked_disabled", "Tree", p_theme->get_icon(SNAME("GuiCheckedDisabled"), EditorStringName(EditorIcons)));
			p_theme->set_icon("indeterminate", "Tree", p_theme->get_icon(SNAME("GuiIndeterminate"), EditorStringName(EditorIcons)));
			p_theme->set_icon("indeterminate_disabled", "Tree", p_theme->get_icon(SNAME("GuiIndeterminateDisabled"), EditorStringName(EditorIcons)));
			p_theme->set_icon("unchecked", "Tree", p_theme->get_icon(SNAME("GuiUnchecked"), EditorStringName(EditorIcons)));
			p_theme->set_icon("unchecked_disabled", "Tree", p_theme->get_icon(SNAME("GuiUncheckedDisabled"), EditorStringName(EditorIcons)));
			p_theme->set_icon("arrow", "Tree", p_theme->get_icon(SNAME("GuiTreeArrowDown"), EditorStringName(EditorIcons)));
			p_theme->set_icon("arrow_collapsed", "Tree", p_theme->get_icon(SNAME("GuiTreeArrowRight"), EditorStringName(EditorIcons)));
			p_theme->set_icon("arrow_collapsed_mirrored", "Tree", p_theme->get_icon(SNAME("GuiTreeArrowLeft"), EditorStringName(EditorIcons)));
			p_theme->set_icon("updown", "Tree", p_theme->get_icon(SNAME("GuiTreeUpdown"), EditorStringName(EditorIcons)));
			p_theme->set_icon("select_arrow", "Tree", p_theme->get_icon(SNAME("GuiDropdown"), EditorStringName(EditorIcons)));

			p_theme->set_stylebox(SceneStringName(panel), "Tree", style_tree_panel);
			p_theme->set_stylebox("focus", "Tree", p_config.focus_style);
			p_theme->set_stylebox("button_pressed", "Tree", style_button_pressed);
			p_theme->set_stylebox("custom_button", "Tree", p_config.flat_button);
			p_theme->set_stylebox("custom_button_pressed", "Tree", style_button_pressed);

			p_theme->set_color("custom_button_font_highlight", "Tree", p_config.font_hover_color);
			p_theme->set_color(SceneStringName(font_color), "Tree", p_config.font_color);
			p_theme->set_color("font_hovered_color", "Tree", p_config.font_hover_color);
			p_theme->set_color("font_hovered_dimmed_color", "Tree", p_config.font_hover_color);
			p_theme->set_color("font_hovered_selected_color", "Tree", p_config.mono_color_font);
			p_theme->set_color("font_selected_color", "Tree", p_config.mono_color_font);
			p_theme->set_color("font_disabled_color", "Tree", p_config.font_disabled_color);
			p_theme->set_color("font_outline_color", "Tree", p_config.font_outline_color);
			p_theme->set_color("title_button_color", "Tree", p_config.font_color);
			p_theme->set_color("drop_position_color", "Tree", ImGuiColors::accent);

			int tree_v_sep = p_config.enable_touch_optimizations ? (p_config.separation_margin * 0.9f) : Math::pow(p_config.base_margin * 0.175f * EDSCALE, 3);
			p_theme->set_constant("v_separation", "Tree", tree_v_sep);
			p_theme->set_constant("h_separation", "Tree", (p_config.increased_margin + 2) * EDSCALE);
			p_theme->set_constant("guide_width", "Tree", p_config.border_width);
			p_theme->set_constant("item_margin", "Tree", MAX(3 * p_config.increased_margin * EDSCALE, 12 * EDSCALE));
			p_theme->set_constant("inner_item_margin_top", "Tree", p_config.base_margin * 0.75f * EDSCALE);
			p_theme->set_constant("inner_item_margin_bottom", "Tree", p_config.base_margin * 0.75f * EDSCALE);
			p_theme->set_constant("inner_item_margin_left", "Tree", p_config.base_margin * EDSCALE);
			p_theme->set_constant("inner_item_margin_right", "Tree", p_config.base_margin * EDSCALE);
			p_theme->set_constant("check_h_separation", "Tree", p_config.base_margin * 1.5f * EDSCALE);
			p_theme->set_constant("icon_h_separation", "Tree", p_config.base_margin * 1.5f * EDSCALE);
			p_theme->set_constant("button_margin", "Tree", p_config.base_margin * EDSCALE);
			p_theme->set_constant("dragging_unfold_wait_msec", "Tree", p_config.dragging_hover_wait_msec);
			p_theme->set_constant("scroll_border", "Tree", 40 * EDSCALE);
			p_theme->set_constant("scroll_speed", "Tree", 12);
			p_theme->set_constant("outline_size", "Tree", 0);

			Color relationship_line_color = p_config.mono_color * Color(1, 1, 1, p_config.relationship_line_opacity);
			Color highlight_line_color = p_config.mono_color * Color(1, 1, 1, p_config.relationship_line_opacity * 2);

			int draw_relationship_lines = 0;
			int relationship_line_width = 0;
			int highlighted_line_width = Math::ceil(EDSCALE);
			if (p_config.draw_relationship_lines == EditorThemeManager::RELATIONSHIP_ALL) {
				draw_relationship_lines = 1;
				relationship_line_width = 1;
			} else if (p_config.draw_relationship_lines == EditorThemeManager::RELATIONSHIP_SELECTED_ONLY) {
				draw_relationship_lines = 1;
			}

			p_theme->set_constant("draw_guides", "Tree", 0);
			p_theme->set_constant("draw_relationship_lines", "Tree", draw_relationship_lines && p_config.relationship_line_opacity >= 0.01);
			p_theme->set_constant("relationship_line_width", "Tree", relationship_line_width);
			p_theme->set_constant("parent_hl_line_width", "Tree", highlighted_line_width);
			p_theme->set_constant("children_hl_line_width", "Tree", 1);
			p_theme->set_constant("parent_hl_line_margin", "Tree", 3);
			p_theme->set_color("relationship_line_color", "Tree", relationship_line_color);
			p_theme->set_color("parent_hl_line_color", "Tree", highlight_line_color);
			p_theme->set_color("children_hl_line_color", "Tree", relationship_line_color);
			p_theme->set_color("drop_position_color", "Tree", p_config.icon_normal_color);
			p_theme->set_color("guide_color", "Tree", Color(1, 1, 1, 0));

			// Tree hover/selected - ImGui header colors
			Ref<StyleBoxFlat> style_tree_hover = p_config.flat_button_hover->duplicate();
			style_tree_hover->set_content_margin_all(0);
			style_tree_hover->set_bg_color(ImGuiColors::header);

			p_theme->set_stylebox("button_hover", "Tree", style_tree_hover);
			p_theme->set_stylebox("hovered", "Tree", style_tree_hover);
			p_theme->set_stylebox("hovered_dimmed", "Tree", style_tree_hover);
			p_theme->set_stylebox("custom_button_hover", "Tree", style_tree_hover);

			Ref<StyleBoxFlat> style_tree_selected = p_config.flat_button_pressed->duplicate();
			style_tree_selected->set_content_margin_all(0);
			style_tree_selected->set_bg_color(ImGuiColors::header_active);

			p_theme->set_stylebox("selected", "Tree", style_tree_selected);
			p_theme->set_stylebox("selected_focus", "Tree", p_config.focus_style);

			Ref<StyleBoxFlat> style_tree_hovered_selected = p_config.flat_button_hover_pressed->duplicate();
			style_tree_hovered_selected->set_content_margin_all(0);
			style_tree_hovered_selected->set_bg_color(ImGuiColors::header_hovered);

			p_theme->set_stylebox("hovered_selected", "Tree", style_tree_hovered_selected);
			p_theme->set_stylebox("hovered_selected_focus", "Tree", p_config.focus_style);

			// Cursor
			Ref<StyleBoxFlat> style_tree_cursor = p_config.base_style->duplicate();
			style_tree_cursor->set_bg_color(p_config.mono_color * Color(1, 1, 1, 0.04f));

			// Title bar
			Ref<StyleBoxFlat> style_tree_title = p_config.base_style->duplicate();
			style_tree_title->set_bg_color(ImGuiColors::title_bg);
		}

		// ItemList
		{
			Ref<StyleBoxFlat> style_itemlist = p_config.base_style->duplicate();
			style_itemlist->set_bg_color(ImGuiColors::child_bg);
			style_itemlist->set_border_width_all(p_config.border_width);
			style_itemlist->set_border_color(ImGuiColors::border);

			Ref<StyleBoxFlat> style_itemlist_hover = p_config.base_style->duplicate();
			style_itemlist_hover->set_bg_color(ImGuiColors::header);
			style_itemlist_hover->set_corner_radius_all(imgui_corner_radius);

			Ref<StyleBoxFlat> style_itemlist_selected = style_itemlist_hover->duplicate();
			style_itemlist_selected->set_bg_color(ImGuiColors::header_active);

			p_theme->set_stylebox(SceneStringName(panel), "ItemList", style_itemlist);
			p_theme->set_stylebox("focus", "ItemList", p_config.focus_style);
			p_theme->set_stylebox("hovered", "ItemList", style_itemlist_hover);
			p_theme->set_stylebox("selected", "ItemList", style_itemlist_selected);
			p_theme->set_stylebox("selected_focus", "ItemList", style_itemlist_selected);
			p_theme->set_stylebox("hovered_selected", "ItemList", style_itemlist_selected);
			p_theme->set_stylebox("hovered_selected_focus", "ItemList", style_itemlist_selected);
			p_theme->set_stylebox("cursor", "ItemList", style_itemlist_hover);
			p_theme->set_stylebox("cursor_unfocused", "ItemList", style_itemlist_hover);

			p_theme->set_color(SceneStringName(font_color), "ItemList", p_config.font_color);
			p_theme->set_color("font_hovered_color", "ItemList", p_config.font_hover_color);
			p_theme->set_color("font_selected_color", "ItemList", p_config.mono_color_font);
			p_theme->set_color("font_outline_color", "ItemList", p_config.font_outline_color);
			p_theme->set_color("guide_color", "ItemList", Color(1, 1, 1, 0));

			p_theme->set_constant("v_separation", "ItemList", p_config.increased_margin * 0.5f * EDSCALE);
			p_theme->set_constant("h_separation", "ItemList", p_config.increased_margin * EDSCALE);
			p_theme->set_constant("icon_margin", "ItemList", p_config.increased_margin * EDSCALE);
			p_theme->set_constant("outline_size", "ItemList", 0);
		}
	}

	// ==========================================================================
	// TAB CONTAINER & TAB BAR
	// ==========================================================================
	{
		// ImGui-style tabs with blue accent
		Ref<StyleBoxFlat> style_tab_selected = p_config.base_style->duplicate();
		style_tab_selected->set_bg_color(ImGuiColors::tab_active);
		style_tab_selected->set_content_margin_individual(
			(p_config.increased_margin + 2) * EDSCALE, p_config.base_margin * EDSCALE,
			(p_config.increased_margin + 2) * EDSCALE, p_config.base_margin * EDSCALE
		);
		style_tab_selected->set_corner_radius_individual(imgui_corner_radius * EDSCALE, imgui_corner_radius * EDSCALE, 0, 0);
		style_tab_selected->set_border_width(SIDE_BOTTOM, 0);

		Ref<StyleBoxFlat> style_tab_focus = style_tab_selected->duplicate();
		style_tab_focus->set_draw_center(false);
		style_tab_focus->set_border_width_all(2 * EDSCALE);
		style_tab_focus->set_bg_color(ImGuiColors::child_bg);
		style_tab_focus->set_border_color(ImGuiColors::accent);

		Ref<StyleBoxFlat> style_tab_unselected = style_tab_selected->duplicate();
		style_tab_unselected->set_bg_color(ImGuiColors::tab_unfocused);
		style_tab_unselected->set_border_width_all(0);

		Ref<StyleBoxFlat> style_tab_hovered = style_tab_unselected->duplicate();
		style_tab_hovered->set_bg_color(ImGuiColors::tab_hovered);

		Color drop_mark_color = ImGuiColors::accent;

		Ref<StyleBoxFlat> style_tabbar_background = p_config.base_style->duplicate();
		style_tabbar_background->set_bg_color(ImGuiColors::window_bg);
		style_tabbar_background->set_corner_radius(CORNER_BOTTOM_LEFT, 0);
		style_tabbar_background->set_corner_radius(CORNER_BOTTOM_RIGHT, 0);
		style_tabbar_background->set_content_margin_individual(0, 0, p_config.base_margin * 0.25f * EDSCALE, 0);

		p_theme->set_stylebox("tabbar_background", "TabContainer", style_tabbar_background);
		p_theme->set_stylebox(SceneStringName(panel), "TabContainer", p_config.tab_container_style);

		p_theme->set_stylebox("tab_selected", "TabContainer", style_tab_selected);
		p_theme->set_stylebox("tab_hovered", "TabContainer", style_tab_hovered);
		p_theme->set_stylebox("tab_unselected", "TabContainer", style_tab_unselected);
		p_theme->set_stylebox("tab_disabled", "TabContainer", style_tab_unselected);
		p_theme->set_stylebox("tab_focus", "TabContainer", p_config.focus_style);
		p_theme->set_stylebox("tab_selected", "TabBar", style_tab_selected);
		p_theme->set_stylebox("tab_hovered", "TabBar", style_tab_hovered);
		p_theme->set_stylebox("tab_unselected", "TabBar", style_tab_unselected);
		p_theme->set_stylebox("tab_disabled", "TabBar", style_tab_unselected);
		p_theme->set_stylebox("tab_focus", "TabBar", p_config.focus_style);
		p_theme->set_stylebox("button_pressed", "TabBar", p_config.panel_container_style);
		p_theme->set_stylebox("button_highlight", "TabBar", p_config.panel_container_style);

		p_theme->set_color("font_selected_color", "TabContainer", p_config.font_color);
		p_theme->set_color("font_hovered_color", "TabContainer", p_config.font_hover_color);
		p_theme->set_color("font_unselected_color", "TabContainer", p_config.font_secondary_color);
		p_theme->set_color("font_disabled_color", "TabContainer", p_config.font_disabled_color * Color(1, 1, 1, 0.55f));
		p_theme->set_color("font_outline_color", "TabContainer", p_config.font_outline_color);
		p_theme->set_color("font_selected_color", "TabBar", p_config.font_color);
		p_theme->set_color("font_hovered_color", "TabBar", p_config.font_hover_color);
		p_theme->set_color("font_unselected_color", "TabBar", p_config.font_secondary_color);
		p_theme->set_color("font_disabled_color", "TabBar", p_config.font_disabled_color * Color(1, 1, 1, 0.55f));
		p_theme->set_color("font_outline_color", "TabBar", p_config.font_outline_color);
		p_theme->set_color("drop_mark_color", "TabContainer", drop_mark_color);
		p_theme->set_color("drop_mark_color", "TabBar", drop_mark_color);

		p_theme->set_icon("menu", "TabContainer", p_theme->get_icon(SNAME("GuiTabMenu"), EditorStringName(EditorIcons)));
		p_theme->set_icon("menu_highlight", "TabContainer", p_theme->get_icon(SNAME("GuiTabMenuHl"), EditorStringName(EditorIcons)));
		p_theme->set_icon("close", "TabBar", p_theme->get_icon(SNAME("GuiClose"), EditorStringName(EditorIcons)));
		p_theme->set_icon("increment", "TabContainer", p_theme->get_icon(SNAME("GuiScrollArrowRight"), EditorStringName(EditorIcons)));
		p_theme->set_icon("decrement", "TabContainer", p_theme->get_icon(SNAME("GuiScrollArrowLeft"), EditorStringName(EditorIcons)));
		p_theme->set_icon("increment", "TabBar", p_theme->get_icon(SNAME("GuiScrollArrowRight"), EditorStringName(EditorIcons)));
		p_theme->set_icon("decrement", "TabBar", p_theme->get_icon(SNAME("GuiScrollArrowLeft"), EditorStringName(EditorIcons)));

		p_theme->set_constant("side_margin", "TabContainer", 0);
		p_theme->set_constant("outline_size", "TabContainer", 0);
		p_theme->set_constant("h_separation", "TabBar", 4 * EDSCALE);
		p_theme->set_constant("outline_size", "TabBar", 0);
	}

	// ==========================================================================
	// SEPARATORS
	// ==========================================================================
	{
		Ref<StyleBoxLine> style_h_separator = EditorThemeManager::make_line_stylebox(
			ImGuiColors::separator, Math::round(2 * EDSCALE),
			p_config.base_margin * -1 * EDSCALE, p_config.base_margin * -1 * EDSCALE
		);
		p_theme->set_stylebox("separator", "HSeparator", style_h_separator);

		Ref<StyleBoxLine> style_v_separator = style_h_separator->duplicate();
		style_v_separator->set_vertical(true);
		p_theme->set_stylebox("separator", "VSeparator", style_v_separator);

		p_theme->set_constant("separation", "Separator", p_config.base_margin * 2 * EDSCALE);
	}

	// ==========================================================================
	// LINE EDIT & TEXT EDIT
	// ==========================================================================
	{
		// ImGui-style input fields with blue-tinted background
		Ref<StyleBoxFlat> text_editor_style = p_config.base_style->duplicate();
		text_editor_style->set_bg_color(ImGuiColors::frame_bg);
		text_editor_style->set_content_margin_individual(
			p_config.base_margin * 2 * EDSCALE, ((p_config.base_margin * 0.75f) + 1) * EDSCALE,
			p_config.base_margin * 2 * EDSCALE, ((p_config.base_margin * 0.75f) + 1) * EDSCALE
		);
		text_editor_style->set_border_width_all(Math::round(EDSCALE));
		text_editor_style->set_border_color(ImGuiColors::border);
		text_editor_style->set_corner_radius_all(imgui_corner_radius);

		Ref<StyleBoxFlat> text_editor_disabled_style = text_editor_style->duplicate();
		text_editor_disabled_style->set_bg_color(ImGuiColors::frame_bg * Color(1, 1, 1, 0.5f));

		// LineEdit
		p_theme->set_stylebox(CoreStringName(normal), "LineEdit", text_editor_style);
		p_theme->set_stylebox("focus", "LineEdit", p_config.focus_style);
		p_theme->set_stylebox("read_only", "LineEdit", text_editor_disabled_style);

		p_theme->set_icon("clear", "LineEdit", p_theme->get_icon(SNAME("GuiClose"), EditorStringName(EditorIcons)));

		p_theme->set_color(SceneStringName(font_color), "LineEdit", p_config.font_color);
		p_theme->set_color("font_selected_color", "LineEdit", p_config.font_pressed_color);
		p_theme->set_color("font_uneditable_color", "LineEdit", p_config.font_readonly_color);
		p_theme->set_color("font_placeholder_color", "LineEdit", p_config.font_placeholder_color);
		p_theme->set_color("font_outline_color", "LineEdit", p_config.font_outline_color);
		p_theme->set_color("caret_color", "LineEdit", p_config.font_color);
		p_theme->set_color("selection_color", "LineEdit", p_config.selection_color);
		p_theme->set_color("clear_button_color", "LineEdit", p_config.icon_normal_color);
		p_theme->set_color("clear_button_color_pressed", "LineEdit", p_config.icon_pressed_color);

		p_theme->set_constant("minimum_character_width", "LineEdit", 4);
		p_theme->set_constant("outline_size", "LineEdit", 0);
		p_theme->set_constant("caret_width", "LineEdit", 1);

		// TextEdit
		p_theme->set_stylebox(CoreStringName(normal), "TextEdit", text_editor_style);
		p_theme->set_stylebox("focus", "TextEdit", p_config.focus_style);
		p_theme->set_stylebox("read_only", "TextEdit", text_editor_disabled_style);

		p_theme->set_icon("tab", "TextEdit", p_theme->get_icon(SNAME("GuiTab"), EditorStringName(EditorIcons)));
		p_theme->set_icon("space", "TextEdit", p_theme->get_icon(SNAME("GuiSpace"), EditorStringName(EditorIcons)));

		p_theme->set_color(SceneStringName(font_color), "TextEdit", p_config.font_color);
		p_theme->set_color("font_selected_color", "TextEdit", p_config.mono_color_font);
		p_theme->set_color("font_readonly_color", "TextEdit", p_config.font_readonly_color);
		p_theme->set_color("font_placeholder_color", "TextEdit", p_config.font_placeholder_color);
		p_theme->set_color("font_outline_color", "TextEdit", p_config.font_outline_color);
		p_theme->set_color("caret_color", "TextEdit", p_config.font_color);
		p_theme->set_color("selection_color", "TextEdit", p_config.selection_color);
		p_theme->set_color("background_color", "TextEdit", Color(0, 0, 0, 0));

		p_theme->set_constant("line_spacing", "TextEdit", 4 * EDSCALE);
		p_theme->set_constant("outline_size", "TextEdit", 0);
		p_theme->set_constant("caret_width", "TextEdit", 1);
	}

	// ==========================================================================
	// SLIDERS & SCROLLBARS
	// ==========================================================================
	{
		Ref<Texture2D> empty_icon = memnew(ImageTexture);

		// ImGui-style grabbers
		Ref<StyleBoxFlat> grabber_style = p_config.base_style->duplicate();
		grabber_style->set_bg_color(ImGuiColors::scrollbar_grab);
		grabber_style->set_corner_radius_all(imgui_corner_radius);

		Ref<StyleBoxFlat> grabber_hl_style = p_config.base_style->duplicate();
		grabber_hl_style->set_bg_color(ImGuiColors::scrollbar_grab_hovered);
		grabber_hl_style->set_corner_radius_all(imgui_corner_radius);

		Ref<StyleBoxFlat> grabber_pressed_style = p_config.base_style->duplicate();
		grabber_pressed_style->set_bg_color(ImGuiColors::scrollbar_grab_active);
		grabber_pressed_style->set_corner_radius_all(imgui_corner_radius);

		int scroll_margin = (p_config.enable_touch_optimizations ? 10 : 3) * EDSCALE;

		// HScrollBar
		Ref<StyleBoxEmpty> h_scroll_style = p_config.base_empty_style->duplicate();
		h_scroll_style->set_content_margin_individual(0, scroll_margin, 0, scroll_margin);

		p_theme->set_stylebox("scroll", "HScrollBar", h_scroll_style);
		p_theme->set_stylebox("scroll_focus", "HScrollBar", p_config.focus_style);
		p_theme->set_stylebox("grabber", "HScrollBar", grabber_style);
		p_theme->set_stylebox("grabber_highlight", "HScrollBar", grabber_hl_style);
		p_theme->set_stylebox("grabber_pressed", "HScrollBar", grabber_pressed_style);

		p_theme->set_icon("increment", "HScrollBar", empty_icon);
		p_theme->set_icon("increment_highlight", "HScrollBar", empty_icon);
		p_theme->set_icon("increment_pressed", "HScrollBar", empty_icon);
		p_theme->set_icon("decrement", "HScrollBar", empty_icon);
		p_theme->set_icon("decrement_highlight", "HScrollBar", empty_icon);
		p_theme->set_icon("decrement_pressed", "HScrollBar", empty_icon);

		p_theme->set_constant("padding_top", "HScrollBar", p_config.base_margin * EDSCALE);
		p_theme->set_constant("padding_bottom", "HScrollBar", p_config.base_margin * EDSCALE);

		// VScrollBar
		Ref<StyleBoxEmpty> v_scroll_style = p_config.base_empty_style->duplicate();
		v_scroll_style->set_content_margin_individual(scroll_margin, 0, scroll_margin, 0);

		p_theme->set_stylebox("scroll", "VScrollBar", v_scroll_style);
		p_theme->set_stylebox("scroll_focus", "VScrollBar", p_config.focus_style);
		p_theme->set_stylebox("grabber", "VScrollBar", grabber_style);
		p_theme->set_stylebox("grabber_highlight", "VScrollBar", grabber_hl_style);
		p_theme->set_stylebox("grabber_pressed", "VScrollBar", grabber_pressed_style);

		p_theme->set_icon("increment", "VScrollBar", empty_icon);
		p_theme->set_icon("increment_highlight", "VScrollBar", empty_icon);
		p_theme->set_icon("increment_pressed", "VScrollBar", empty_icon);
		p_theme->set_icon("decrement", "VScrollBar", empty_icon);
		p_theme->set_icon("decrement_highlight", "VScrollBar", empty_icon);
		p_theme->set_icon("decrement_pressed", "VScrollBar", empty_icon);

		p_theme->set_constant("padding_left", "VScrollBar", p_config.base_margin * EDSCALE);
		p_theme->set_constant("padding_right", "VScrollBar", p_config.base_margin * EDSCALE);

		// Sliders - ImGui blue grabber style
		const int background_margin = MAX(2, p_config.base_margin / 2);

		Ref<StyleBoxFlat> style_h_slider = p_config.base_style->duplicate();
		style_h_slider->set_bg_color(ImGuiColors::frame_bg);
		style_h_slider->set_content_margin_individual(0, 2 * EDSCALE, 0, 2 * EDSCALE);
		style_h_slider->set_corner_radius_all(imgui_corner_radius);

		Ref<StyleBoxFlat> slider_grabber_area = EditorThemeManager::make_flat_stylebox(
			ImGuiColors::slider_grab, 0, background_margin, 0, background_margin, imgui_corner_radius
		);

		Ref<StyleBoxFlat> slider_grabber_area_hl = EditorThemeManager::make_flat_stylebox(
			ImGuiColors::slider_grab_active, 0, background_margin, 0, background_margin, imgui_corner_radius
		);

		// HSlider
		p_theme->set_icon("grabber_highlight", "HSlider", p_theme->get_icon(SNAME("GuiSliderGrabberHl"), EditorStringName(EditorIcons)));
		p_theme->set_icon("grabber", "HSlider", p_theme->get_icon(SNAME("GuiSliderGrabber"), EditorStringName(EditorIcons)));
		p_theme->set_stylebox("slider", "HSlider", style_h_slider);
		p_theme->set_stylebox("grabber_area", "HSlider", slider_grabber_area);
		p_theme->set_stylebox("grabber_area_highlight", "HSlider", slider_grabber_area_hl);
		p_theme->set_constant("center_grabber", "HSlider", 0);
		p_theme->set_constant("grabber_offset", "HSlider", 0);

		Ref<StyleBoxFlat> style_v_slider = style_h_slider->duplicate();
		style_v_slider->set_content_margin_individual(2 * EDSCALE, 0, 2 * EDSCALE, 0);

		Ref<StyleBoxFlat> slider_grabber_area_v = EditorThemeManager::make_flat_stylebox(
			ImGuiColors::slider_grab, background_margin, 0, background_margin, 0, imgui_corner_radius
		);

		Ref<StyleBoxFlat> slider_grabber_area_hl_v = EditorThemeManager::make_flat_stylebox(
			ImGuiColors::slider_grab_active, background_margin, 0, background_margin, 0, imgui_corner_radius
		);

		// VSlider
		p_theme->set_icon("grabber", "VSlider", p_theme->get_icon(SNAME("GuiSliderGrabber"), EditorStringName(EditorIcons)));
		p_theme->set_icon("grabber_highlight", "VSlider", p_theme->get_icon(SNAME("GuiSliderGrabberHl"), EditorStringName(EditorIcons)));
		p_theme->set_stylebox("slider", "VSlider", style_v_slider);
		p_theme->set_stylebox("grabber_area", "VSlider", slider_grabber_area_v);
		p_theme->set_stylebox("grabber_area_highlight", "VSlider", slider_grabber_area_hl_v);
		p_theme->set_constant("center_grabber", "VSlider", 0);
		p_theme->set_constant("grabber_offset", "VSlider", 0);
	}

	// ==========================================================================
	// LABELS
	// ==========================================================================
	{
		// RichTextLabel
		Ref<StyleBoxFlat> rich_text_style = p_config.base_style->duplicate();
		rich_text_style->set_bg_color(ImGuiColors::child_bg);
		rich_text_style->set_content_margin_all(p_config.base_margin * 2 * EDSCALE);

		p_theme->set_stylebox(CoreStringName(normal), "RichTextLabel", rich_text_style);
		p_theme->set_stylebox("focus", "RichTextLabel", EditorThemeManager::make_empty_stylebox());

		p_theme->set_color("default_color", "RichTextLabel", p_config.font_color);
		p_theme->set_color("font_shadow_color", "RichTextLabel", Color(0, 0, 0, 0));
		p_theme->set_color("font_outline_color", "RichTextLabel", p_config.font_outline_color);
		p_theme->set_color("selection_color", "RichTextLabel", p_config.selection_color);

		p_theme->set_constant("shadow_offset_x", "RichTextLabel", 1 * EDSCALE);
		p_theme->set_constant("shadow_offset_y", "RichTextLabel", 1 * EDSCALE);
		p_theme->set_constant("shadow_outline_size", "RichTextLabel", 1 * EDSCALE);
		p_theme->set_constant("outline_size", "RichTextLabel", 0);

		// Label
		Ref<StyleBoxEmpty> label_style = p_config.base_empty_style->duplicate();
		label_style->set_content_margin_individual(
			p_config.base_margin * 2 * EDSCALE, p_config.base_margin * EDSCALE,
			p_config.base_margin * 2 * EDSCALE, p_config.base_margin * EDSCALE
		);

		p_theme->set_stylebox(CoreStringName(normal), "Label", label_style);
		p_theme->set_stylebox("focus", "Label", p_config.focus_style);

		p_theme->set_color(SceneStringName(font_color), "Label", p_config.font_color);
		p_theme->set_color("font_shadow_color", "Label", Color(0, 0, 0, 0));
		p_theme->set_color("font_outline_color", "Label", p_config.font_outline_color);

		p_theme->set_constant("shadow_offset_x", "Label", 1 * EDSCALE);
		p_theme->set_constant("shadow_offset_y", "Label", 1 * EDSCALE);
		p_theme->set_constant("shadow_outline_size", "Label", 1 * EDSCALE);
		p_theme->set_constant("line_spacing", "Label", 3 * EDSCALE);
		p_theme->set_constant("outline_size", "Label", 0);
	}

	// ==========================================================================
	// SPINBOX
	// ==========================================================================
	{
		Ref<Texture2D> empty_icon = memnew(ImageTexture);
		p_theme->set_icon("updown", "SpinBox", empty_icon);
		p_theme->set_icon("up", "SpinBox", p_theme->get_icon(SNAME("GuiSpinboxUp"), EditorStringName(EditorIcons)));
		p_theme->set_icon("up_hover", "SpinBox", p_theme->get_icon(SNAME("GuiSpinboxUp"), EditorStringName(EditorIcons)));
		p_theme->set_icon("up_pressed", "SpinBox", p_theme->get_icon(SNAME("GuiSpinboxUp"), EditorStringName(EditorIcons)));
		p_theme->set_icon("up_disabled", "SpinBox", p_theme->get_icon(SNAME("GuiSpinboxUp"), EditorStringName(EditorIcons)));
		p_theme->set_icon("down", "SpinBox", p_theme->get_icon(SNAME("GuiSpinboxDown"), EditorStringName(EditorIcons)));
		p_theme->set_icon("down_hover", "SpinBox", p_theme->get_icon(SNAME("GuiSpinboxDown"), EditorStringName(EditorIcons)));
		p_theme->set_icon("down_pressed", "SpinBox", p_theme->get_icon(SNAME("GuiSpinboxDown"), EditorStringName(EditorIcons)));
		p_theme->set_icon("down_disabled", "SpinBox", p_theme->get_icon(SNAME("GuiSpinboxDown"), EditorStringName(EditorIcons)));

		p_theme->set_stylebox("up_background", "SpinBox", EditorThemeManager::make_empty_stylebox());
		p_theme->set_stylebox("up_background_hovered", "SpinBox", p_config.button_style_hover);
		p_theme->set_stylebox("up_background_pressed", "SpinBox", p_config.button_style_pressed);
		p_theme->set_stylebox("up_background_disabled", "SpinBox", EditorThemeManager::make_empty_stylebox());
		p_theme->set_stylebox("down_background", "SpinBox", EditorThemeManager::make_empty_stylebox());
		p_theme->set_stylebox("down_background_hovered", "SpinBox", p_config.button_style_hover);
		p_theme->set_stylebox("down_background_pressed", "SpinBox", p_config.button_style_pressed);
		p_theme->set_stylebox("down_background_disabled", "SpinBox", EditorThemeManager::make_empty_stylebox());

		p_theme->set_color("up_icon_modulate", "SpinBox", p_config.icon_normal_color);
		p_theme->set_color("up_hover_icon_modulate", "SpinBox", p_config.icon_hover_color);
		p_theme->set_color("up_pressed_icon_modulate", "SpinBox", p_config.icon_pressed_color);
		p_theme->set_color("up_disabled_icon_modulate", "SpinBox", p_config.icon_disabled_color);
		p_theme->set_color("down_icon_modulate", "SpinBox", p_config.icon_normal_color);
		p_theme->set_color("down_hover_icon_modulate", "SpinBox", p_config.icon_hover_color);
		p_theme->set_color("down_pressed_icon_modulate", "SpinBox", p_config.icon_pressed_color);
		p_theme->set_color("down_disabled_icon_modulate", "SpinBox", p_config.icon_disabled_color);

		p_theme->set_stylebox("field_and_buttons_separator", "SpinBox", EditorThemeManager::make_empty_stylebox());
		p_theme->set_stylebox("up_down_buttons_separator", "SpinBox", EditorThemeManager::make_empty_stylebox());

		p_theme->set_constant("buttons_vertical_separation", "SpinBox", 0);
		p_theme->set_constant("field_and_buttons_separation", "SpinBox", 2);
		p_theme->set_constant("buttons_width", "SpinBox", 16);
	}

	// ==========================================================================
	// PROGRESS BAR
	// ==========================================================================
	{
		Ref<StyleBoxFlat> progress_bar_style = p_config.base_style->duplicate();
		progress_bar_style->set_bg_color(ImGuiColors::frame_bg);
		progress_bar_style->set_expand_margin(SIDE_TOP, p_config.base_margin * 0.5f * EDSCALE);
		progress_bar_style->set_expand_margin(SIDE_BOTTOM, p_config.base_margin * 0.5f * EDSCALE);
		progress_bar_style->set_content_margin_all(p_config.base_margin * EDSCALE);
		progress_bar_style->set_border_width_all(Math::round(EDSCALE));
		progress_bar_style->set_border_color(ImGuiColors::border);
		progress_bar_style->set_corner_radius_all(imgui_corner_radius);

		Ref<StyleBoxFlat> progress_fill_style = progress_bar_style->duplicate();
		progress_fill_style->set_bg_color(ImGuiColors::slider_grab);
		progress_fill_style->set_border_color(ImGuiColors::accent);

		p_theme->set_stylebox("background", "ProgressBar", progress_bar_style);
		p_theme->set_stylebox("fill", "ProgressBar", progress_fill_style);
		p_theme->set_color(SceneStringName(font_color), "ProgressBar", p_config.font_color);
		p_theme->set_color("font_outline_color", "ProgressBar", p_config.font_outline_color);
		p_theme->set_constant("outline_size", "ProgressBar", 0);
	}

	// ==========================================================================
	// WINDOW & DIALOGS
	// ==========================================================================
	{
		// Window
		p_theme->set_stylebox("embedded_border", "Window", p_config.window_style);
		p_theme->set_stylebox("embedded_unfocused_border", "Window", p_config.window_style);

		p_theme->set_color("title_color", "Window", p_config.font_color);
		p_theme->set_icon("close", "Window", p_theme->get_icon(SNAME("GuiClose"), EditorStringName(EditorIcons)));
		p_theme->set_icon("close_pressed", "Window", p_theme->get_icon(SNAME("GuiClose"), EditorStringName(EditorIcons)));
		p_theme->set_constant("close_h_offset", "Window", 22 * EDSCALE);
		p_theme->set_constant("close_v_offset", "Window", 20 * EDSCALE);
		p_theme->set_constant("title_height", "Window", 24 * EDSCALE);
		p_theme->set_constant("resize_margin", "Window", 4 * EDSCALE);
		p_theme->set_font("title_font", "Window", p_theme->get_font(SNAME("title"), EditorStringName(EditorFonts)));
		p_theme->set_font_size("title_font_size", "Window", p_theme->get_font_size(SNAME("title_size"), EditorStringName(EditorFonts)));

		// AcceptDialog
		p_theme->set_stylebox(SceneStringName(panel), "AcceptDialog", p_config.dialog_style);
		p_theme->set_constant("buttons_separation", "AcceptDialog", 8 * EDSCALE);
		p_theme->set_constant("buttons_min_width", "AcceptDialog", p_config.dialogs_buttons_min_size.x * EDSCALE);
		p_theme->set_constant("buttons_min_height", "AcceptDialog", p_config.dialogs_buttons_min_size.y * EDSCALE);

		// FileDialog
		p_theme->set_icon("folder", "FileDialog", p_theme->get_icon("Folder", EditorStringName(EditorIcons)));
		p_theme->set_icon("parent_folder", "FileDialog", p_theme->get_icon("ArrowUp", EditorStringName(EditorIcons)));
		p_theme->set_icon("back_folder", "FileDialog", p_theme->get_icon("Back", EditorStringName(EditorIcons)));
		p_theme->set_icon("forward_folder", "FileDialog", p_theme->get_icon("Forward", EditorStringName(EditorIcons)));
		p_theme->set_icon("reload", "FileDialog", p_theme->get_icon("Reload", EditorStringName(EditorIcons)));
		p_theme->set_icon("toggle_hidden", "FileDialog", p_theme->get_icon("GuiVisibilityVisible", EditorStringName(EditorIcons)));
		p_theme->set_color("folder_icon_color", "FileDialog", ImGuiColors::accent.lightened(0.3f));
		p_theme->set_color("file_disabled_color", "FileDialog", p_config.font_disabled_color);

		// PopupDialog
		p_theme->set_stylebox(SceneStringName(panel), "PopupDialog", p_config.dialog_style);

		// PopupMenu
		{
			Ref<StyleBoxFlat> style_popup_menu = p_config.base_style->duplicate();
			style_popup_menu->set_bg_color(ImGuiColors::popup_bg);
			style_popup_menu->set_content_margin_all(p_config.popup_margin);
			style_popup_menu->set_corner_radius_all(0);
			style_popup_menu->set_border_width_all(Math::round(EDSCALE));
			style_popup_menu->set_border_color(ImGuiColors::border);
			p_theme->set_stylebox(SceneStringName(panel), "PopupMenu", style_popup_menu);

			Ref<StyleBoxFlat> style_popup_hover = p_config.flat_button_hover->duplicate();
			style_popup_hover->set_bg_color(ImGuiColors::header);
			p_theme->set_stylebox(SceneStringName(hover), "PopupMenu", style_popup_hover);

			Ref<StyleBoxLine> style_popup_separator = EditorThemeManager::make_line_stylebox(
				ImGuiColors::separator, Math::round(2 * EDSCALE),
				p_config.base_margin * -2 * EDSCALE, p_config.base_margin * -2 * EDSCALE
			);

			p_theme->set_stylebox("separator", "PopupMenu", style_popup_separator);
			p_theme->set_stylebox("labeled_separator_left", "PopupMenu", style_popup_separator);
			p_theme->set_stylebox("labeled_separator_right", "PopupMenu", style_popup_separator);

			p_theme->set_color(SceneStringName(font_color), "PopupMenu", p_config.font_color);
			p_theme->set_color("font_hover_color", "PopupMenu", p_config.font_hover_color);
			p_theme->set_color("font_accelerator_color", "PopupMenu", p_config.font_disabled_color);
			p_theme->set_color("font_disabled_color", "PopupMenu", p_config.font_disabled_color);
			p_theme->set_color("font_separator_color", "PopupMenu", p_config.font_disabled_color);
			p_theme->set_color("font_outline_color", "PopupMenu", p_config.font_outline_color);

			p_theme->set_icon("checked", "PopupMenu", p_theme->get_icon(SNAME("GuiChecked"), EditorStringName(EditorIcons)));
			p_theme->set_icon("unchecked", "PopupMenu", p_theme->get_icon(SNAME("GuiUnchecked"), EditorStringName(EditorIcons)));
			p_theme->set_icon("radio_checked", "PopupMenu", p_theme->get_icon(SNAME("GuiRadioChecked"), EditorStringName(EditorIcons)));
			p_theme->set_icon("radio_unchecked", "PopupMenu", p_theme->get_icon(SNAME("GuiRadioUnchecked"), EditorStringName(EditorIcons)));
			p_theme->set_icon("submenu", "PopupMenu", p_theme->get_icon(SNAME("ArrowRight"), EditorStringName(EditorIcons)));
			p_theme->set_icon("submenu_mirrored", "PopupMenu", p_theme->get_icon(SNAME("ArrowLeft"), EditorStringName(EditorIcons)));

			p_theme->set_constant("h_separation", "PopupMenu", p_config.base_margin * 1.75f * EDSCALE);
			int v_sep = (p_config.enable_touch_optimizations ? 12 : p_config.base_margin * 1.75f) * EDSCALE;
			p_theme->set_constant("v_separation", "PopupMenu", v_sep);
			p_theme->set_constant("outline_size", "PopupMenu", 0);
			p_theme->set_constant("item_start_padding", "PopupMenu", p_config.popup_margin);
			p_theme->set_constant("item_end_padding", "PopupMenu", p_config.popup_margin);
		}
	}

	// ==========================================================================
	// SCROLL CONTAINER
	// ==========================================================================
	{
		p_theme->set_stylebox(SceneStringName(panel), "ScrollContainer", EditorThemeManager::make_empty_stylebox());

		// Secondary scroll container with visible background
		Ref<StyleBoxFlat> scroll_secondary_style = p_config.base_style->duplicate();
		scroll_secondary_style->set_bg_color(ImGuiColors::child_bg);
		scroll_secondary_style->set_corner_radius_all(imgui_corner_radius);
		p_theme->set_stylebox(SceneStringName(panel), "ScrollContainerSecondary", scroll_secondary_style);
	}

	// ==========================================================================
	// GRAPH EDIT & GRAPH NODE
	// ==========================================================================
	{
		// GraphEdit
		Ref<StyleBoxFlat> graph_panel = p_config.tree_panel_style->duplicate();
		graph_panel->set_content_margin_all(0);
		graph_panel->set_bg_color(ImGuiColors::window_bg);

		p_theme->set_stylebox(SceneStringName(panel), "GraphEdit", graph_panel);
		p_theme->set_color("grid_minor", "GraphEdit", ImGuiColors::border * Color(1, 1, 1, 0.3f));
		p_theme->set_color("grid_major", "GraphEdit", ImGuiColors::border * Color(1, 1, 1, 0.5f));
		p_theme->set_color("activity", "GraphEdit", ImGuiColors::accent);
		p_theme->set_color("selection_fill", "GraphEdit", ImGuiColors::text_selected_bg);
		p_theme->set_color("selection_stroke", "GraphEdit", ImGuiColors::accent);
		p_theme->set_color("connection_rim_color", "GraphEdit", Color(0.1f, 0.1f, 0.1f, 0.6f));
		p_theme->set_constant("port_hotzone_inner_extent", "GraphEdit", 22 * EDSCALE);
		p_theme->set_constant("port_hotzone_outer_extent", "GraphEdit", 26 * EDSCALE);

		// GraphEditMinimap
		Ref<StyleBoxFlat> minimap_style = p_config.base_style->duplicate();
		minimap_style->set_bg_color(ImGuiColors::popup_bg);
		minimap_style->set_border_color(ImGuiColors::border);
		minimap_style->set_border_width_all(Math::round(EDSCALE));

		p_theme->set_stylebox(SceneStringName(panel), "GraphEditMinimap", minimap_style);
		p_theme->set_color("resizer_color", "GraphEditMinimap", ImGuiColors::resize_grip);
	}
}

void ThemeImGui::populate_editor_styles(const Ref<EditorTheme> &p_theme, EditorThemeManager::ThemeConfiguration &p_config) {
	const int imgui_corner_radius = 2;

	// ==========================================================================
	// EDITOR TOP BAR & MAIN PANEL
	// ==========================================================================
	{
		// Main editor screen background
		Ref<StyleBoxFlat> style_main_panel = p_config.base_style->duplicate();
		style_main_panel->set_bg_color(ImGuiColors::window_bg);
		style_main_panel->set_corner_radius_all(0);
		p_theme->set_stylebox("MainPanel", EditorStringName(EditorStyles), style_main_panel);

		// Top bar background - ImGui menu bar style
		Ref<StyleBoxFlat> style_topbar = p_config.base_style->duplicate();
		style_topbar->set_bg_color(ImGuiColors::menu_bar_bg);
		style_topbar->set_corner_radius_all(0);
		style_topbar->set_content_margin_all(0);
		p_theme->set_stylebox("Topbar", EditorStringName(EditorStyles), style_topbar);

		// Bottom bar
		Ref<StyleBoxFlat> style_bottombar = style_topbar->duplicate();
		style_bottombar->set_bg_color(ImGuiColors::title_bg);
		p_theme->set_stylebox("Bottombar", EditorStringName(EditorStyles), style_bottombar);
	}

	// ==========================================================================
	// EDITOR BUTTONS
	// ==========================================================================
	{
		// Flat button (used extensively in editor)
		p_theme->set_stylebox(CoreStringName(normal), "FlatButton", p_config.flat_button);
		p_theme->set_stylebox("hover", "FlatButton", p_config.flat_button_hover);
		p_theme->set_stylebox(SceneStringName(pressed), "FlatButton", p_config.flat_button_pressed);
		p_theme->set_stylebox("disabled", "FlatButton", p_config.flat_button);

		p_theme->set_color(SceneStringName(font_color), "FlatButton", p_config.font_color);
		p_theme->set_color("font_hover_color", "FlatButton", p_config.font_hover_color);
		p_theme->set_color("font_pressed_color", "FlatButton", p_config.font_pressed_color);
		p_theme->set_color("font_disabled_color", "FlatButton", p_config.font_disabled_color);

		p_theme->set_color("icon_normal_color", "FlatButton", p_config.icon_normal_color);
		p_theme->set_color("icon_hover_color", "FlatButton", p_config.icon_hover_color);
		p_theme->set_color("icon_pressed_color", "FlatButton", p_config.icon_pressed_color);
		p_theme->set_color("icon_disabled_color", "FlatButton", p_config.icon_disabled_color);

		// Flat menu button
		p_theme->set_stylebox(CoreStringName(normal), "FlatMenuButton", p_config.flat_button);
		p_theme->set_stylebox("hover", "FlatMenuButton", p_config.flat_button_hover);
		p_theme->set_stylebox(SceneStringName(pressed), "FlatMenuButton", p_config.flat_button_pressed);
		p_theme->set_stylebox("disabled", "FlatMenuButton", p_config.flat_button);

		p_theme->set_color(SceneStringName(font_color), "FlatMenuButton", p_config.font_color);
		p_theme->set_color("font_hover_color", "FlatMenuButton", p_config.font_hover_color);
		p_theme->set_color("font_pressed_color", "FlatMenuButton", p_config.font_pressed_color);
		p_theme->set_color("font_disabled_color", "FlatMenuButton", p_config.font_disabled_color);

		// Editor log filter button
		p_theme->set_stylebox(CoreStringName(normal), "EditorLogFilterButton", p_config.flat_button);
		p_theme->set_stylebox("hover", "EditorLogFilterButton", p_config.flat_button_hover);
		p_theme->set_stylebox(SceneStringName(pressed), "EditorLogFilterButton", p_config.flat_button_pressed);

		p_theme->set_color(SceneStringName(font_color), "EditorLogFilterButton", p_config.font_color);
		p_theme->set_color("font_hover_color", "EditorLogFilterButton", p_config.font_hover_color);
		p_theme->set_color("font_pressed_color", "EditorLogFilterButton", p_config.font_pressed_color);

		p_theme->set_color("icon_normal_color", "EditorLogFilterButton", p_config.icon_normal_color);
		p_theme->set_color("icon_hover_color", "EditorLogFilterButton", p_config.icon_hover_color);
		p_theme->set_color("icon_pressed_color", "EditorLogFilterButton", p_config.icon_pressed_color);
	}

	// ==========================================================================
	// INSPECTOR
	// ==========================================================================
	{
		// Inspector property background
		Ref<StyleBoxFlat> style_inspector_section = p_config.base_style->duplicate();
		style_inspector_section->set_bg_color(ImGuiColors::header * Color(1, 1, 1, 0.3f));
		style_inspector_section->set_corner_radius_all(imgui_corner_radius);

		Ref<StyleBoxFlat> style_inspector_section_hover = style_inspector_section->duplicate();
		style_inspector_section_hover->set_bg_color(ImGuiColors::header);

		p_theme->set_stylebox("section", "Inspector", style_inspector_section);
		p_theme->set_stylebox("section_hover", "Inspector", style_inspector_section_hover);

		// Property group
		Ref<StyleBoxFlat> style_property_group = p_config.base_style->duplicate();
		style_property_group->set_bg_color(ImGuiColors::child_bg.lightened(0.05f));
		style_property_group->set_corner_radius_all(0);

		p_theme->set_stylebox("PropertyGroup", EditorStringName(EditorStyles), style_property_group);

		// Category backgrounds with ImGui header colors
		Ref<StyleBoxFlat> style_category_bg = p_config.base_style->duplicate();
		style_category_bg->set_bg_color(ImGuiColors::title_bg_active);
		style_category_bg->set_corner_radius_all(imgui_corner_radius);
		style_category_bg->set_content_margin_individual(
			p_config.base_margin * 2 * EDSCALE, p_config.base_margin * EDSCALE,
			p_config.base_margin * 2 * EDSCALE, p_config.base_margin * EDSCALE
		);

		p_theme->set_stylebox("CategoryBg", EditorStringName(EditorStyles), style_category_bg);
		p_theme->set_color("category_font_color", EditorStringName(Editor), p_config.font_color);

		// Sub-inspector background
		Ref<StyleBoxFlat> style_sub_inspector = p_config.content_panel_style->duplicate();
		style_sub_inspector->set_bg_color(ImGuiColors::child_bg.lightened(0.02f));
		style_sub_inspector->set_border_width_all(p_config.border_width);
		style_sub_inspector->set_border_color(ImGuiColors::border);
		style_sub_inspector->set_corner_radius_all(imgui_corner_radius);

		p_theme->set_stylebox("sub_inspector_bg", EditorStringName(EditorStyles), style_sub_inspector);
		p_theme->set_stylebox("sub_inspector_property_bg", EditorStringName(EditorStyles), style_sub_inspector);

		// Inspector action buttons
		p_theme->set_stylebox("InspectorActionButton", EditorStringName(EditorStyles), p_config.button_style);
	}

	// ==========================================================================
	// SCENE TREE / NODE DOCK
	// ==========================================================================
	{
		// Scene tree background
		Ref<StyleBoxFlat> scene_tree_style = p_config.tree_panel_style->duplicate();
		scene_tree_style->set_bg_color(ImGuiColors::child_bg);
		scene_tree_style->set_border_color(ImGuiColors::border);

		p_theme->set_stylebox("SceneTree", EditorStringName(EditorStyles), scene_tree_style);

		// Node highlight
		p_theme->set_color("node_highlight_color", EditorStringName(Editor), ImGuiColors::accent);

		// Scene tab bar
		Ref<StyleBoxFlat> scene_tab_bar = p_config.base_style->duplicate();
		scene_tab_bar->set_bg_color(ImGuiColors::menu_bar_bg);
		scene_tab_bar->set_corner_radius_all(0);

		p_theme->set_stylebox("SceneTabBar", EditorStringName(EditorStyles), scene_tab_bar);
	}

	// ==========================================================================
	// FILE SYSTEM DOCK
	// ==========================================================================
	{
		Ref<StyleBoxFlat> filesystem_panel = p_config.base_style->duplicate();
		filesystem_panel->set_bg_color(ImGuiColors::child_bg);
		filesystem_panel->set_corner_radius_all(imgui_corner_radius);

		p_theme->set_stylebox("FileSystemPanel", EditorStringName(EditorStyles), filesystem_panel);

		// File/folder colors
		p_theme->set_color("folder_color", "FileSystemDock", ImGuiColors::accent.lightened(0.3f));
		p_theme->set_color("file_color", "FileSystemDock", p_config.font_color);
		p_theme->set_color("file_disabled_color", "FileSystemDock", p_config.font_disabled_color);
	}

	// ==========================================================================
	// CODE EDITOR
	// ==========================================================================
	{
		// Code editor background
		Ref<StyleBoxFlat> code_editor_style = p_config.base_style->duplicate();
		code_editor_style->set_bg_color(ImGuiColors::window_bg);
		code_editor_style->set_border_color(ImGuiColors::border);
		code_editor_style->set_border_width_all(p_config.border_width);
		code_editor_style->set_corner_radius_all(imgui_corner_radius);

		p_theme->set_stylebox("CodeEditorPanel", EditorStringName(EditorStyles), code_editor_style);

		// Line number background
		Ref<StyleBoxFlat> line_number_bg = p_config.base_style->duplicate();
		line_number_bg->set_bg_color(ImGuiColors::child_bg);
		line_number_bg->set_corner_radius_all(0);

		p_theme->set_stylebox("LineNumberBg", EditorStringName(EditorStyles), line_number_bg);

		// Breakpoint and bookmark colors
		p_theme->set_color("breakpoint_color", "CodeEdit", Color(0.9f, 0.3f, 0.3f, 1.0f));
		p_theme->set_color("bookmark_color", "CodeEdit", ImGuiColors::accent);
		p_theme->set_color("executing_line_color", "CodeEdit", Color(0.98f, 0.89f, 0.27f, 1.0f));

		// Syntax highlighting colors (ImGui-inspired)
		p_theme->set_color("keyword_color", "CodeEdit", Color(0.86f, 0.56f, 0.86f, 1.0f)); // Purple
		p_theme->set_color("control_flow_keyword_color", "CodeEdit", Color(0.86f, 0.56f, 0.86f, 1.0f));
		p_theme->set_color("base_type_color", "CodeEdit", Color(0.55f, 0.82f, 0.92f, 1.0f)); // Cyan
		p_theme->set_color("engine_type_color", "CodeEdit", Color(0.55f, 0.82f, 0.92f, 1.0f));
		p_theme->set_color("user_type_color", "CodeEdit", Color(0.55f, 0.82f, 0.92f, 1.0f));
		p_theme->set_color("string_color", "CodeEdit", Color(0.81f, 0.67f, 0.51f, 1.0f)); // Brown/Orange
		p_theme->set_color("comment_color", "CodeEdit", Color(0.5f, 0.62f, 0.5f, 1.0f)); // Gray-green
		p_theme->set_color("number_color", "CodeEdit", Color(0.71f, 0.84f, 0.67f, 1.0f)); // Light green
		p_theme->set_color("symbol_color", "CodeEdit", p_config.font_color);
		p_theme->set_color("function_color", "CodeEdit", Color(0.82f, 0.82f, 0.67f, 1.0f)); // Yellow
		p_theme->set_color("member_variable_color", "CodeEdit", Color(0.61f, 0.73f, 0.89f, 1.0f)); // Light blue
	}

	// ==========================================================================
	// EDITOR DOCKS
	// ==========================================================================
	{
		// Dock background
		Ref<StyleBoxFlat> dock_style = p_config.base_style->duplicate();
		dock_style->set_bg_color(ImGuiColors::child_bg);
		dock_style->set_corner_radius_all(0);

		p_theme->set_stylebox("DockBg", EditorStringName(EditorStyles), dock_style);

		// Dock tab style
		Ref<StyleBoxFlat> dock_tab_selected = p_config.base_style->duplicate();
		dock_tab_selected->set_bg_color(ImGuiColors::tab_active);
		dock_tab_selected->set_corner_radius_individual(imgui_corner_radius * EDSCALE, imgui_corner_radius * EDSCALE, 0, 0);

		Ref<StyleBoxFlat> dock_tab_unselected = dock_tab_selected->duplicate();
		dock_tab_unselected->set_bg_color(ImGuiColors::tab_unfocused);

		p_theme->set_stylebox("DockTabSelected", EditorStringName(EditorStyles), dock_tab_selected);
		p_theme->set_stylebox("DockTabUnselected", EditorStringName(EditorStyles), dock_tab_unselected);

		// Dock close button
		p_theme->set_stylebox("DockCloseButton", EditorStringName(EditorStyles), p_config.flat_button);
		p_theme->set_stylebox("DockCloseButtonHover", EditorStringName(EditorStyles), p_config.flat_button_hover);
		p_theme->set_stylebox("DockCloseButtonPressed", EditorStringName(EditorStyles), p_config.flat_button_pressed);
	}

	// ==========================================================================
	// EDITOR TOOLBAR / TOOL BUTTONS
	// ==========================================================================
	{
		// Toolbar button
		p_theme->set_stylebox("ToolbarButton", EditorStringName(EditorStyles), p_config.flat_button);
		p_theme->set_stylebox("ToolbarButtonHover", EditorStringName(EditorStyles), p_config.flat_button_hover);
		p_theme->set_stylebox("ToolbarButtonPressed", EditorStringName(EditorStyles), p_config.flat_button_pressed);

		// Run/Stop button styles
		Ref<StyleBoxFlat> run_button_style = p_config.button_style->duplicate();
		run_button_style->set_bg_color(Color(0.2f, 0.6f, 0.2f, 0.8f)); // Green tint

		Ref<StyleBoxFlat> stop_button_style = p_config.button_style->duplicate();
		stop_button_style->set_bg_color(Color(0.6f, 0.2f, 0.2f, 0.8f)); // Red tint

		p_theme->set_stylebox("RunButton", EditorStringName(EditorStyles), run_button_style);
		p_theme->set_stylebox("StopButton", EditorStringName(EditorStyles), stop_button_style);
	}

	// ==========================================================================
	// ANIMATION EDITOR
	// ==========================================================================
	{
		// Animation panel
		Ref<StyleBoxFlat> animation_panel = p_config.base_style->duplicate();
		animation_panel->set_bg_color(ImGuiColors::child_bg);
		animation_panel->set_corner_radius_all(imgui_corner_radius);

		p_theme->set_stylebox("AnimationPanel", EditorStringName(EditorStyles), animation_panel);

		// Keyframe colors
		p_theme->set_color("keyframe_color", "AnimationTrack", ImGuiColors::accent);
		p_theme->set_color("keyframe_selected_color", "AnimationTrack", ImGuiColors::accent.lightened(0.3f));
	}

	// ==========================================================================
	// 2D/3D VIEWPORT
	// ==========================================================================
	{
		// Viewport background
		Ref<StyleBoxFlat> viewport_style = p_config.base_style->duplicate();
		viewport_style->set_bg_color(ImGuiColors::window_bg);
		viewport_style->set_corner_radius_all(0);

		p_theme->set_stylebox("ViewportPanel", EditorStringName(EditorStyles), viewport_style);

		// Viewport info overlay
		Ref<StyleBoxFlat> viewport_info = p_config.base_style->duplicate();
		viewport_info->set_bg_color(ImGuiColors::popup_bg);
		viewport_info->set_corner_radius_all(imgui_corner_radius);
		viewport_info->set_content_margin_all(p_config.base_margin * EDSCALE);

		p_theme->set_stylebox("ViewportInfo", EditorStringName(EditorStyles), viewport_info);
	}

	// ==========================================================================
	// PROJECT MANAGER
	// ==========================================================================
	{
		// Project list item
		Ref<StyleBoxFlat> project_item = p_config.base_style->duplicate();
		project_item->set_bg_color(ImGuiColors::child_bg);
		project_item->set_corner_radius_all(imgui_corner_radius);
		project_item->set_border_width_all(Math::round(EDSCALE));
		project_item->set_border_color(ImGuiColors::border);

		Ref<StyleBoxFlat> project_item_hover = project_item->duplicate();
		project_item_hover->set_bg_color(ImGuiColors::header);

		Ref<StyleBoxFlat> project_item_selected = project_item->duplicate();
		project_item_selected->set_bg_color(ImGuiColors::header_active);
		project_item_selected->set_border_color(ImGuiColors::accent);

		p_theme->set_stylebox("ProjectItem", EditorStringName(EditorStyles), project_item);
		p_theme->set_stylebox("ProjectItemHover", EditorStringName(EditorStyles), project_item_hover);
		p_theme->set_stylebox("ProjectItemSelected", EditorStringName(EditorStyles), project_item_selected);
	}

	// ==========================================================================
	// SEARCH / QUICK OPEN
	// ==========================================================================
	{
		// Search result background
		Ref<StyleBoxFlat> search_result = p_config.base_style->duplicate();
		search_result->set_bg_color(ImGuiColors::header);
		search_result->set_corner_radius_all(imgui_corner_radius);

		Ref<StyleBoxFlat> search_result_selected = search_result->duplicate();
		search_result_selected->set_bg_color(ImGuiColors::header_active);

		p_theme->set_stylebox("SearchResult", EditorStringName(EditorStyles), search_result);
		p_theme->set_stylebox("SearchResultSelected", EditorStringName(EditorStyles), search_result_selected);

		// Quick open panel
		Ref<StyleBoxFlat> quick_open = p_config.popup_panel_style->duplicate();
		quick_open->set_bg_color(ImGuiColors::popup_bg);

		p_theme->set_stylebox("QuickOpenPanel", EditorStringName(EditorStyles), quick_open);
	}

	// ==========================================================================
	// ERROR/WARNING PANELS
	// ==========================================================================
	{
		// Error panel
		Ref<StyleBoxFlat> error_panel = p_config.base_style->duplicate();
		error_panel->set_bg_color(ImGuiColors::error * Color(1, 1, 1, 0.2f));
		error_panel->set_border_width_all(Math::round(EDSCALE));
		error_panel->set_border_color(ImGuiColors::error);
		error_panel->set_corner_radius_all(imgui_corner_radius);

		p_theme->set_stylebox("ErrorPanel", EditorStringName(EditorStyles), error_panel);

		// Warning panel
		Ref<StyleBoxFlat> warning_panel = error_panel->duplicate();
		warning_panel->set_bg_color(ImGuiColors::warning * Color(1, 1, 1, 0.2f));
		warning_panel->set_border_color(ImGuiColors::warning);

		p_theme->set_stylebox("WarningPanel", EditorStringName(EditorStyles), warning_panel);

		// Success panel
		Ref<StyleBoxFlat> success_panel = error_panel->duplicate();
		success_panel->set_bg_color(ImGuiColors::success * Color(1, 1, 1, 0.2f));
		success_panel->set_border_color(ImGuiColors::success);

		p_theme->set_stylebox("SuccessPanel", EditorStringName(EditorStyles), success_panel);
	}

	// ==========================================================================
	// DEBUGGER
	// ==========================================================================
	{
		Ref<StyleBoxFlat> debugger_panel_style = p_config.content_panel_style->duplicate();
		debugger_panel_style->set_border_width(SIDE_BOTTOM, 0);
		debugger_panel_style->set_bg_color(ImGuiColors::child_bg);

		p_theme->set_stylebox("DebuggerPanel", EditorStringName(EditorStyles), debugger_panel_style);

		// Stack frame colors
		p_theme->set_color("current_frame_color", "Debugger", ImGuiColors::accent);
		p_theme->set_color("breakpoint_color", "Debugger", Color(0.9f, 0.3f, 0.3f, 1.0f));
	}

	// ==========================================================================
	// ASSET LIBRARY
	// ==========================================================================
	{
		p_theme->set_stylebox("bg", "AssetLib", EditorThemeManager::make_empty_stylebox(
			p_config.base_margin, p_config.base_margin, p_config.base_margin, p_config.base_margin
		));
		p_theme->set_stylebox(SceneStringName(panel), "AssetLib", p_config.foreground_panel);
		p_theme->set_stylebox("downloads", "AssetLib", p_theme->get_stylebox(SceneStringName(panel), SNAME("ScrollContainerSecondary")));
		p_theme->set_color("status_color", "AssetLib", ImGuiColors::text_disabled);
		p_theme->set_icon("dismiss", "AssetLib", p_theme->get_icon(SNAME("Close"), EditorStringName(EditorIcons)));
	}

	// ==========================================================================
	// OBJECT DB
	// ==========================================================================
	{
		Ref<StyleBoxFlat> style_content_wrapper = p_config.panel_container_style->duplicate();
		style_content_wrapper->set_draw_center(true);
		style_content_wrapper->set_bg_color(ImGuiColors::child_bg);

		p_theme->set_stylebox("ObjectDBContentWrapper", EditorStringName(EditorStyles), style_content_wrapper);

		Ref<StyleBoxFlat> style_title = style_content_wrapper->duplicate();
		style_title->set_bg_color(ImGuiColors::title_bg_active);

		p_theme->set_stylebox("ObjectDBTitle", EditorStringName(EditorStyles), style_title);
	}

	// ==========================================================================
	// VISUAL SHADER / STATE MACHINE
	// ==========================================================================
	{
		// VisualShader editor
		p_theme->set_stylebox("label_style", "VShaderEditor",
			EditorThemeManager::make_empty_stylebox(4, 6, 4, 6));

		// StateMachine graph
		{
			p_theme->set_stylebox(SceneStringName(panel), "GraphStateMachine", p_config.tree_panel_style);
			p_theme->set_stylebox("error_panel", "GraphStateMachine", p_config.tree_panel_style);
			p_theme->set_color("error_color", "GraphStateMachine", p_config.error_color);

			const int sm_margin_side = 10 * EDSCALE;
			const int sm_margin_bottom = 2;
			const Color sm_bg_color = ImGuiColors::child_bg;

			Ref<StyleBoxFlat> sm_node_style = EditorThemeManager::make_flat_stylebox(
				ImGuiColors::child_bg * Color(1, 1, 1, 0.7f),
				sm_margin_side, 24 * EDSCALE, sm_margin_side, sm_margin_bottom, imgui_corner_radius
			);
			sm_node_style->set_border_width_all(p_config.border_width);
			sm_node_style->set_border_color(ImGuiColors::border);

			Ref<StyleBoxFlat> sm_node_selected_style = EditorThemeManager::make_flat_stylebox(
				sm_bg_color * Color(1, 1, 1, 0.9f),
				sm_margin_side, 24 * EDSCALE, sm_margin_side, sm_margin_bottom, imgui_corner_radius
			);
			sm_node_selected_style->set_border_width_all(2 * EDSCALE + p_config.border_width);
			sm_node_selected_style->set_border_color(ImGuiColors::accent * Color(1, 1, 1, 0.9f));
			sm_node_selected_style->set_shadow_size(8 * EDSCALE);
			sm_node_selected_style->set_shadow_color(p_config.shadow_color);

			Ref<StyleBoxFlat> sm_node_playing_style = sm_node_selected_style->duplicate();
			sm_node_playing_style->set_border_color(p_config.warning_color);
			sm_node_playing_style->set_shadow_color(p_config.warning_color * Color(1, 1, 1, 0.2f));
			sm_node_playing_style->set_draw_center(false);

			p_theme->set_stylebox("node_frame", "GraphStateMachine", sm_node_style);
			p_theme->set_stylebox("node_frame_selected", "GraphStateMachine", sm_node_selected_style);
			p_theme->set_stylebox("node_frame_playing", "GraphStateMachine", sm_node_playing_style);

			Ref<StyleBoxFlat> sm_node_start_style = sm_node_style->duplicate();
			sm_node_start_style->set_border_width_all(1 * EDSCALE);
			sm_node_start_style->set_border_color(p_config.success_color.lightened(0.24f));
			p_theme->set_stylebox("node_frame_start", "GraphStateMachine", sm_node_start_style);

			Ref<StyleBoxFlat> sm_node_end_style = sm_node_style->duplicate();
			sm_node_end_style->set_border_width_all(1 * EDSCALE);
			sm_node_end_style->set_border_color(p_config.error_color);
			p_theme->set_stylebox("node_frame_end", "GraphStateMachine", sm_node_end_style);

			p_theme->set_font("node_title_font", "GraphStateMachine", p_theme->get_font(SceneStringName(font), SNAME("Label")));
			p_theme->set_font_size("node_title_font_size", "GraphStateMachine", p_theme->get_font_size(SceneStringName(font_size), SNAME("Label")));
			p_theme->set_color("node_title_font_color", "GraphStateMachine", p_config.font_color);

			p_theme->set_color("transition_color", "GraphStateMachine", p_config.font_color);
			p_theme->set_color("transition_disabled_color", "GraphStateMachine", p_config.font_color * Color(1, 1, 1, 0.2f));
			p_theme->set_color("transition_icon_color", "GraphStateMachine", Color(1, 1, 1));
			p_theme->set_color("transition_icon_disabled_color", "GraphStateMachine", Color(1, 1, 1, 0.2f));
			p_theme->set_color("highlight_color", "GraphStateMachine", ImGuiColors::accent);
			p_theme->set_color("highlight_disabled_color", "GraphStateMachine", ImGuiColors::accent * Color(1, 1, 1, 0.6f));
			p_theme->set_color("focus_color", "GraphStateMachine", ImGuiColors::accent * Color(1, 1, 1, 0.8f));
			p_theme->set_color("guideline_color", "GraphStateMachine", p_config.font_color * Color(1, 1, 1, 0.3f));

			p_theme->set_color("playback_color", "GraphStateMachine", p_config.font_color);
			p_theme->set_color("playback_background_color", "GraphStateMachine", p_config.font_color * Color(1, 1, 1, 0.3f));
		}
	}

	// ==========================================================================
	// TEXTURE REGION / THEME EDITOR
	// ==========================================================================
	{
		// TextureRegion editor
		Ref<StyleBoxFlat> style_texture_region_bg = p_config.tree_panel_style->duplicate();
		style_texture_region_bg->set_content_margin_all(0);
		p_theme->set_stylebox("TextureRegionPreviewBG", EditorStringName(EditorStyles), style_texture_region_bg);
		p_theme->set_stylebox("TextureRegionPreviewFG", EditorStringName(EditorStyles), EditorThemeManager::make_empty_stylebox(0, 0, 0, 0));

		// Theme editor
		{
			p_theme->set_color("preview_picker_overlay_color", "ThemeEditor", Color(0.1f, 0.1f, 0.1f, 0.25f));

			Color theme_preview_picker_bg_color = ImGuiColors::accent;
			theme_preview_picker_bg_color.a = 0.2f;
			Ref<StyleBoxFlat> theme_preview_picker_sb = EditorThemeManager::make_flat_stylebox(theme_preview_picker_bg_color, 0, 0, 0, 0);
			theme_preview_picker_sb->set_border_color(ImGuiColors::accent);
			theme_preview_picker_sb->set_border_width_all(1.0f * EDSCALE);
			p_theme->set_stylebox("preview_picker_overlay", "ThemeEditor", theme_preview_picker_sb);

			Color theme_preview_picker_label_bg_color = ImGuiColors::accent;
			theme_preview_picker_label_bg_color.set_v(0.5f);
			Ref<StyleBoxFlat> theme_preview_picker_label_sb = EditorThemeManager::make_flat_stylebox(theme_preview_picker_label_bg_color, 4.0f, 1.0f, 4.0f, 3.0f);
			p_theme->set_stylebox("preview_picker_label", "ThemeEditor", theme_preview_picker_label_sb);

			Ref<StyleBoxFlat> style_theme_preview_tab = p_theme->get_stylebox(SNAME("tab_selected"), SNAME("TabContainer"))->duplicate();
			style_theme_preview_tab->set_expand_margin(SIDE_BOTTOM, 5 * EDSCALE);
			p_theme->set_stylebox("ThemeEditorPreviewFG", EditorStringName(EditorStyles), style_theme_preview_tab);

			Ref<StyleBoxFlat> style_theme_preview_bg_tab = p_theme->get_stylebox(SNAME("tab_unselected"), SNAME("TabContainer"))->duplicate();
			style_theme_preview_bg_tab->set_expand_margin(SIDE_BOTTOM, 2 * EDSCALE);
			p_theme->set_stylebox("ThemeEditorPreviewBG", EditorStringName(EditorStyles), style_theme_preview_bg_tab);
		}
	}

	// ==========================================================================
	// TILESET EDITOR
	// ==========================================================================
	{
		Ref<StyleBoxFlat> tile_expand_style = p_config.base_style->duplicate();
		tile_expand_style->set_bg_color(ImGuiColors::child_bg);
		tile_expand_style->set_corner_radius_all(0);
		p_theme->set_stylebox("expand_panel", "TileSetEditor", tile_expand_style);
	}
}
