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

#pragma once

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
extern "C" {
#include <shm_ringbuffers.h>
}

namespace godot {

class CaptureTexture : public ImageTexture {
	GDCLASS(CaptureTexture, ImageTexture);

private:
	// We'll store our image in here.
	Ref<Image> internal_image;
	uint32_t frame_size;
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
	void connect_shm_buffer(String shm_name, String ring_buffer_name, int width, int height);

	// Manually update pixels and reupload
	void update_texture_contents();
};

} // namespace godot
