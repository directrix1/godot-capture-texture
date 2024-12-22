#include "capture_texture.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void CaptureTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create_internal_image", "width", "height"), &CaptureTexture::create_internal_image);
	ClassDB::bind_method(D_METHOD("update_texture_contents"), &CaptureTexture::update_texture_contents);
	ClassDB::bind_method(D_METHOD("_ready"), &CaptureTexture::_ready);
	ClassDB::bind_method(D_METHOD("_process", "delta"), &CaptureTexture::_process);
}

CaptureTexture::CaptureTexture() {
	// Optionally initialize your image here or do it in create_internal_image().
}

void CaptureTexture::_ready() {
	// Called when the node enters the scene tree, if used as a Node or attached to a Node.
	// You can optionally create an internal image with a default size:
	// create_internal_image(256, 256);
}

void CaptureTexture::_process(double delta) {
	// If you want this texture to be updated every frame, do it here:
	update_texture_contents();
}

void CaptureTexture::create_internal_image(int width, int height) {
	// We create an Image of the desired size/format.
	// RGBA8 is common; choose a format that suits your needs.
	internal_image = Image::create(width, height, false, Image::FORMAT_RGBA8);

	// Initialize the contents of the image in some way...
	// For instance, fill with opaque white:
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			internal_image->set_pixel(x, y, Color(1.0, 0.0, 0.0, 1.0));
		}
	}

	// Now turn the Image into an ImageTexture
	// (the 'this' object IS already an ImageTexture, so we can just call create_from_image)
	set_image(internal_image);
	// If you want filter, repeats, or other flags, you can set them here, e.g.:
	// set_filter(use_filter);
	// set_repeat(false);
}

void CaptureTexture::update_texture_contents() {
	if (internal_image.is_null()) {
		return;
	}

	// Example: fill each pixel with random RGBA
	const int width = internal_image->get_width();
	const int height = internal_image->get_height();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			internal_image->set_pixel(x, y, Color(UtilityFunctions::randf(), // R
													UtilityFunctions::randf(), // G
													UtilityFunctions::randf(), // B
													1.0));
		}
	}

	// Re-upload to the GPU
	set_image(internal_image);
}
