#include "capture_texture.hpp"
#include <shm_ringbuffers.h>
#include <chrono>

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void CaptureTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("connect_shm_buffer", "width", "height"), &CaptureTexture::connect_shm_buffer);
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

void CaptureTexture::connect_shm_buffer(int width, int height) {
	const char *shm_name = "/srb_video_test";
	const char *ring_buffer_name = "video_frames";

	// First we connect to the shared memory buffer
	srb_handle = srb_client_new((char *)shm_name);
	if (srb_handle == NULL) {
		return;
	}

	srb = srb_get_ring_by_description(srb_handle, (char *)ring_buffer_name);
	if (srb) {
		printf("Found ring: %s\n", srb->description);
	} else {
		fprintf(stderr, "Could not find ring: %s\n", ring_buffer_name);
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

	// Example: fill each pixel with random RGBA
	const int width = internal_image->get_width();
	const int height = internal_image->get_height();

	// auto t1 = std::chrono::high_resolution_clock::now();
	// uint8_t *a = (uint8_t *)srb_subscriber_get_most_recent_buffer(srb);
	struct buf_rgba *a = (struct buf_rgba *)srb_subscriber_get_most_recent_buffer(srb);
	// auto t2 = std::chrono::high_resolution_clock::now();

	float c = 1.0 / 255;
	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			internal_image->set_pixel(x, y, Color(a->r * c, a->g * c, a->b * c, a->a * c));
			a++;
		}
	}
	// for (size_t i = 0; i < frame_size; i++) {
	// 	pba.set(i, *(a++));
	// }
	// auto t3 = std::chrono::high_resolution_clock::now();
	// internal_image->set_data(width, height, false, Image::FORMAT_RGBA8, pba);
	// auto t4 = std::chrono::high_resolution_clock::now();
	// std::cout << "Timing (ms): "
	// 		  << static_cast<long long int>(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()) << " "
	// 		  << static_cast<long long int>(std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count()) << " "
	// 		  << std::endl;

	update(internal_image);
}
