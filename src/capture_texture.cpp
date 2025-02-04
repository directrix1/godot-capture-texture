#include "capture_texture.hpp"
#include <shm_ringbuffers.h>
// #include <chrono>

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void CaptureTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("connect_shm_buffer", "shm_name", "ring_buffer_name", "width", "height"), &CaptureTexture::connect_shm_buffer);
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

void CaptureTexture::connect_shm_buffer(String shm_name, String ring_buffer_name, int width, int height) {
	// First we connect to the shared memory buffer
	srb_handle = srb_client_new(shm_name.utf8().ptrw());
	if (srb_handle == NULL) {
		return;
	}

	srb = srb_get_ring_by_description(srb_handle, ring_buffer_name.utf8().ptrw());
	if (srb) {
		printf("Found ring: %s\n", srb->description);
	} else {
		fprintf(stderr, "Could not find ring: %s\n", ring_buffer_name.utf8().ptrw());
		return;
	}

	// We create an Image of the desired size/format.
	// RGBA8 is common; choose a format that suits your needs.
	internal_image = Image::create(width, height, false, Image::FORMAT_RGBA8);
	frame_size = width * height * 4;

	pba.resize(frame_size);

	set_image(internal_image);
}

void CaptureTexture::update_texture_contents() {
	if (internal_image.is_null()) {
		return;
	}

	memcpy((void *)pba.ptrw(), (const void *)srb_subscriber_get_most_recent_buffer(srb), frame_size);

	internal_image->set_data(internal_image->get_width(), internal_image->get_height(), false, Image::FORMAT_RGBA8, pba);

	update(internal_image);
}
