/**************************************************************************/
<<<<<<<< HEAD:core/io/socket_server.h
/*  socket_server.h                                                       */
========
/*  blit_material.h                                                       */
>>>>>>>> upstream/master:scene/resources/blit_material.h
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

#pragma once

<<<<<<<< HEAD:core/io/socket_server.h
#include "core/io/net_socket.h"
#include "core/io/stream_peer_socket.h"

class SocketServer : public RefCounted {
	GDCLASS(SocketServer, RefCounted);

protected:
	enum {
		MAX_PENDING_CONNECTIONS = 8,
	};

	Ref<NetSocket> _sock;
========
#include "scene/resources/material.h"

class BlitMaterial : public Material {
	GDCLASS(BlitMaterial, Material);

public:
	enum BlendMode {
		BLEND_MODE_MIX,
		BLEND_MODE_ADD,
		BLEND_MODE_SUB,
		BLEND_MODE_MUL,
		BLEND_MODE_DISABLED
	};

private:
	static Mutex shader_mutex;
	static RID shader_cache[5];
	static void _update_shader(BlendMode p_blend);
	mutable bool shader_set = false;

	BlendMode blend_mode = BLEND_MODE_MIX;

protected:
>>>>>>>> upstream/master:scene/resources/blit_material.h
	static void _bind_methods();

	Error _listen(const NetSocket::Address &p_addr);

	template <typename T>
	Ref<T> _take_connection() {
		Ref<T> conn;
		if (!is_connection_available()) {
			return conn;
		}

		Ref<NetSocket> ns;
		NetSocket::Address addr;
		ns = _sock->accept(addr);
		if (ns.is_null()) {
			return conn;
		}

		conn.instantiate();
		conn->accept_socket(ns, addr);
		return conn;
	}

public:
<<<<<<<< HEAD:core/io/socket_server.h
	bool is_listening() const;
	bool is_connection_available() const;
	virtual Ref<StreamPeerSocket> take_socket_connection() = 0;

	void stop(); // Stop listening

	SocketServer();
	~SocketServer();
========
	void set_blend_mode(BlendMode p_blend_mode);
	BlendMode get_blend_mode() const;

	virtual Shader::Mode get_shader_mode() const override;
	virtual RID get_shader_rid() const override;
	virtual RID get_rid() const override;

	static void cleanup_shader();

	BlitMaterial();
	virtual ~BlitMaterial();
>>>>>>>> upstream/master:scene/resources/blit_material.h
};

VARIANT_ENUM_CAST(BlitMaterial::BlendMode);
