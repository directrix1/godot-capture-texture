/******************************************************************************
 *
 * Copyright (c) 2025-present Edward Andrew Flick.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

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
	ClassDB::bind_method(D_METHOD("set_width", "width"), &CaptureTexture::set_width);
	ClassDB::bind_method(D_METHOD("get_width"), &CaptureTexture::get_width);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &CaptureTexture::set_height);
	ClassDB::bind_method(D_METHOD("get_height"), &CaptureTexture::get_height);
	ClassDB::bind_method(D_METHOD("set_shm_name", "shm_name"), &CaptureTexture::set_shm_name);
	ClassDB::bind_method(D_METHOD("get_shm_name"), &CaptureTexture::get_shm_name);
	ClassDB::bind_method(D_METHOD("set_ring_buffer_name", "ring_buffer_name"), &CaptureTexture::set_ring_buffer_name);
	ClassDB::bind_method(D_METHOD("get_ring_buffer_name"), &CaptureTexture::get_ring_buffer_name);
	ClassDB::bind_method(D_METHOD("ensure_size"), &CaptureTexture::ensure_size);
	ClassDB::bind_method(D_METHOD("ensure_connected"), &CaptureTexture::ensure_connected);
	ClassDB::bind_method(D_METHOD("ensure_disconnected"), &CaptureTexture::ensure_disconnected);
	ClassDB::bind_method(D_METHOD("update_texture_contents"), &CaptureTexture::update_texture_contents);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "1,10000"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "height", PROPERTY_HINT_RANGE, "1,10000"), "set_height", "get_height");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "shm_name"), "set_shm_name", "get_shm_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "ring_buffer_name"), "set_ring_buffer_name", "get_ring_buffer_name");
}

CaptureTexture::CaptureTexture() {
	ensure_size();
}

CaptureTexture::~CaptureTexture() {
	ensure_disconnected();
}

void CaptureTexture::set_width(int p_width) {
	if (p_width > 0 && p_width != width) {
		width = p_width;
		ensure_size();
	}
}

int CaptureTexture::get_width(void) const {
	return width;
}

void CaptureTexture::set_height(int p_height) {
	if (p_height > 0 && p_height != height) {
		height = p_height;
		ensure_size();
	}
}

int CaptureTexture::get_height(void) const {
	return height;
}

void CaptureTexture::set_shm_name(const String &p_shm_name) {
	if (p_shm_name.casecmp_to(shm_name) != 0) {
		shm_name = p_shm_name;
		ensure_disconnected();
	}
}

String CaptureTexture::get_shm_name(void) const {
	return shm_name;
}

void CaptureTexture::set_ring_buffer_name(const String &p_ring_buffer_name) {
	if (p_ring_buffer_name.casecmp_to(ring_buffer_name) != 0) {
		ring_buffer_name = p_ring_buffer_name;
		ensure_disconnected();
	}
}

String CaptureTexture::get_ring_buffer_name(void) const {
	return ring_buffer_name;
}

void CaptureTexture::ensure_size() {
	ensure_disconnected();
	UtilityFunctions::print("SRB: Resized internal image");
	frame_size = width * height * 4;
	internal_image = Image::create(width, height, false, Image::FORMAT_RGBA8);
	internal_image->fill(Color(1.0, 0.2, 0.9, 1.0));
	set_image(internal_image);
}

bool CaptureTexture::ensure_connected() {
	if (srb) {
		if (srb->shared->buffer_size >= frame_size && srb_client_get_state(srb_handle) == SRB_RUNNING) {
			return true;
		}

		if (srb->shared->buffer_size < frame_size) {
			UtilityFunctions::print("SRB: Buffer size %d < frame_size %d", srb->shared->buffer_size, frame_size);
		}

		// This shouldn't happen if things are running right
		ensure_disconnected();
		return false;
	}

	// First we connect to the shared memory buffer
	srb_handle = srb_client_new(shm_name.utf8().ptrw());
	if (srb_handle == NULL) {
		UtilityFunctions::print("SRB: Connected to shm_name: %s", shm_name);
		return false;
	}

	srb = srb_get_ring_by_description(srb_handle, ring_buffer_name.utf8().ptrw());
	if (srb) {
		UtilityFunctions::print("SRB: Found ring: %s", srb->description);
	} else {
		ensure_disconnected();
		UtilityFunctions::print("SRB: Could not find ring: %s", ring_buffer_name.utf8().ptrw());
		return false;
	}

	if (srb->shared->buffer_size <= frame_size && srb_client_get_state(srb_handle) == SRB_RUNNING) {
		return true;
	}

	if (srb->shared->buffer_size < frame_size) {
		UtilityFunctions::print("SRB: Buffer size %d < frame_size %d", srb->shared->buffer_size, frame_size);
	}

	// This shouldn't happen if things are running right
	ensure_disconnected();
	return false;
}

void CaptureTexture::ensure_disconnected() {
	srb = NULL;
	if (srb_handle) {
		UtilityFunctions::print("SRB: Disconnected");
		srb_close(srb_handle);
		srb_handle = NULL;
	}
}

void CaptureTexture::update_texture_contents() {
	if (!ensure_connected()) {
		return;
	}

	uint8_t *b = srb_subscriber_get_most_recent_buffer(srb);

	if (b) {
		memcpy((void *)internal_image->ptrw(), (const void *)b, frame_size);

		update(internal_image);
	}
}
