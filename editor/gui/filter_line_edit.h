/**************************************************************************/
<<<<<<<< HEAD:scene/gui/file_dialog.compat.inc
/*  file_dialog.compat.inc                                                */
========
/*  filter_line_edit.h                                                    */
>>>>>>>> upstream/master:editor/gui/filter_line_edit.h
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

<<<<<<<< HEAD:scene/gui/file_dialog.compat.inc
#ifndef DISABLE_DEPRECATED

void FileDialog::_add_filter_bind_compat_111439(const String &p_filter, const String &p_description) {
	add_filter(p_filter, p_description, "");
}

void FileDialog::_bind_compatibility_methods() {
	ClassDB::bind_compatibility_method(D_METHOD("add_filter", "filter", "description"), &FileDialog::_add_filter_bind_compat_111439, DEFVAL(""));
}

#endif
========
#pragma once

#include "scene/gui/line_edit.h"

class FilterLineEdit : public LineEdit {
	GDCLASS(FilterLineEdit, LineEdit);

	Control *forward_control = nullptr;

protected:
	void _notification(int p_what);

	virtual void gui_input(const Ref<InputEvent> &p_event) override;

public:
	void set_forward_control(Control *p_control);

	FilterLineEdit();
};
>>>>>>>> upstream/master:editor/gui/filter_line_edit.h
