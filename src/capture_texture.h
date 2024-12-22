#pragma once

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

class CaptureTexture : public ImageTexture {
	GDCLASS(CaptureTexture, ImageTexture);

private:
	// We'll store our image in here.
	Ref<Image> internal_image;

protected:
	static void _bind_methods();

public:
	CaptureTexture();
	~CaptureTexture() override = default;

	// Called after the object is initialized in Godot.
	void _ready();
	void _process(double delta);

	// Helper to initialize your image with a desired size and format.
	void create_internal_image(int width, int height);

	// Manually update pixels and reupload
	void update_texture_contents();
};

} // namespace godot
