#pragma once
#include "core/object/ref_counted.h"
#include "scene/resources/texture.h"
#include "servers/audio/audio_stream.h"
#include "modules/gltf/gltf_document.h"
#include "modules/gltf/gltf_state.h"
#include "scene/main/node.h"

class Z_Import : public RefCounted {
	GDCLASS(Z_Import, RefCounted);

protected:
	static void _bind_methods();

public:
	static Ref<Texture2D> PNG(const String &p_path);
	static Ref<AudioStream> WAV(const String &p_path);

	static Node* GLTF(const String &p_path);
};
