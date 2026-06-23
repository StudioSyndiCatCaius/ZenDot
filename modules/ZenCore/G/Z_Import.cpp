#include "Z_Import.h"

#include "core/io/file_access.h"
#include "core/io/image.h"
#include "core/io/resource_loader.h"
#include "scene/resources/audio_stream_wav.h"
#include "servers/audio/audio_stream.h"
#include "scene/resources/image_texture.h"

void Z_Import::_bind_methods() {
	ClassDB::bind_static_method("Z_Import", D_METHOD("PNG", "file_path"), &Z_Import::PNG);
	ClassDB::bind_static_method("Z_Import", D_METHOD("WAV", "file_path"), &Z_Import::WAV);
	ClassDB::bind_static_method("Z_Import", D_METHOD("GLTF", "file_path"), &Z_Import::GLTF);
	//ClassDB::bind_static_method("Z_Import", D_METHOD("OGG", "file_path"), &Z_Import::OGG);
}

Ref<Texture2D> Z_Import::PNG(const String &p_path) {
	Ref<Image> img = Image::load_from_file(p_path);
	if (img.is_null()) {
		ERR_PRINT("Z_Import::PNG — failed to load: " + p_path);
		return Ref<Texture2D>();
	}
	return ImageTexture::create_from_image(img);
}


// implementations:
Ref<AudioStream> Z_Import::WAV(const String &p_path) {
	Vector<uint8_t> file_data = FileAccess::get_file_as_bytes(p_path);
	if (file_data.is_empty()) {
		ERR_PRINT("Z_Import::WAV — failed to read file: " + p_path);
		return Ref<AudioStream>();
	}

	const uint8_t *d = file_data.ptr();

	// Parse fmt chunk
	int num_channels  = d[22] | (d[23] << 8);
	int sample_rate   = d[24] | (d[25] << 8) | (d[26] << 16) | (d[27] << 24);
	int bits_per_sample = d[34] | (d[35] << 8);

	// Find the "data" chunk (it may not always be at byte 36)
	int data_offset = 12;
	while (data_offset + 8 < file_data.size()) {
		if (d[data_offset] == 'd' && d[data_offset+1] == 'a' &&
			d[data_offset+2] == 't' && d[data_offset+3] == 'a') {
			data_offset += 8; // skip "data" + chunk size
			break;
			}
		int chunk_size = d[data_offset+4] | (d[data_offset+5] << 8) |
						 (d[data_offset+6] << 16) | (d[data_offset+7] << 24);
		data_offset += 8 + chunk_size;
	}

	Vector<uint8_t> pcm_data;
	pcm_data.resize(file_data.size() - data_offset);
	memcpy(pcm_data.ptrw(), d + data_offset, pcm_data.size());

	Ref<AudioStreamWAV> stream;
	stream.instantiate();
	stream->set_data(pcm_data);
	stream->set_mix_rate(sample_rate);
	stream->set_stereo(num_channels == 2);
	stream->set_format(bits_per_sample == 16 ? AudioStreamWAV::FORMAT_16_BITS : AudioStreamWAV::FORMAT_8_BITS);

	return stream;
}

Node* Z_Import::GLTF(const String &p_path) {
	Ref<GLTFDocument> doc;
	doc.instantiate();

	Ref<GLTFState> state;
	state.instantiate();

	Error err = doc->append_from_file(p_path, state);
	if (err != OK) {
		ERR_PRINT("Z_Import::GLTF — failed to load: " + p_path);
		return nullptr;
	}

	Node* scene = doc->generate_scene(state);
	if (scene == nullptr) {
		ERR_PRINT("Z_Import::GLTF — failed to generate scene: " + p_path);
		return nullptr;
	}

	return scene;
}
