#pragma once

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/core/class_db.hpp>
extern "C" {
#include <shm_ringbuffers.h>
}

namespace godot {

#pragma pack(1)
struct buf_rgba {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

class CaptureTexture : public ImageTexture {
	GDCLASS(CaptureTexture, ImageTexture);

private:
	// We'll store our image in here.
	Ref<Image> internal_image;
	uint32_t frame_size;
	PackedByteArray pba;
	SRBHandle srb_handle;
	struct ShmRingBuffer *srb;

protected:
	static void _bind_methods();

public:
	CaptureTexture();
	~CaptureTexture() override = default;

	// Called after the object is initialized in Godot.
	void _ready();
	void _process(double delta);

	// Helper to initialize your image with a desired size and format.
	void connect_shm_buffer(int width, int height);

	// Manually update pixels and reupload
	void update_texture_contents();
};

} // namespace godot
