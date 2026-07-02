/**************************************************************************/
<<<<<<<< HEAD:modules/luaAPI/external/godot-cpp/include/godot_cpp/core/mutex_lock.hpp
/*  mutex_lock.hpp                                                        */
========
/*  image_resource_format.h                                               */
>>>>>>>> upstream/master:core/io/image_resource_format.h
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

#ifndef GODOT_MUTEX_LOCK_HPP
#define GODOT_MUTEX_LOCK_HPP

<<<<<<<< HEAD:modules/luaAPI/external/godot-cpp/include/godot_cpp/core/mutex_lock.hpp
#include <godot_cpp/classes/mutex.hpp>

namespace godot {

class MutexLock {
	const Mutex &mutex;

public:
	_ALWAYS_INLINE_ explicit MutexLock(const Mutex &p_mutex) :
			mutex(p_mutex) {
		const_cast<Mutex *>(&mutex)->lock();
	}

	_ALWAYS_INLINE_ ~MutexLock() {
		const_cast<Mutex *>(&mutex)->unlock();
	}
};

#define _THREAD_SAFE_CLASS_ mutable Mutex _thread_safe_;
#define _THREAD_SAFE_METHOD_ MutexLock _thread_safe_method_(_thread_safe_);
#define _THREAD_SAFE_LOCK_ _thread_safe_.lock();
#define _THREAD_SAFE_UNLOCK_ _thread_safe_.unlock();

} // namespace godot

#endif // GODOT_MUTEX_LOCK_HPP
========
#include "core/io/image_loader.h"
#include "core/io/resource_loader.h"

class ResourceFormatLoaderImage : public ResourceFormatLoader {
	GDSOFTCLASS(ResourceFormatLoaderImage, ResourceFormatLoader);

public:
	virtual Ref<Resource> load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr, bool p_use_sub_threads = false, float *r_progress = nullptr, CacheMode p_cache_mode = CACHE_MODE_REUSE) override;
	virtual void get_recognized_extensions(List<String> *p_extensions) const override;
	virtual bool handles_type(const String &p_type) const override;
	virtual String get_resource_type(const String &p_path) const override;
};
>>>>>>>> upstream/master:core/io/image_resource_format.h
