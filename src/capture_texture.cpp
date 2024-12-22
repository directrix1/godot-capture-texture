#include "capture_texture.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_format.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>

using namespace godot;

void CaptureTexture::_bind_methods() {
    // Bind your methods so they are visible to GDScript (optional)
    ClassDB::bind_method(D_METHOD("update_texture_contents"), &CaptureTexture::update_texture_contents);
    ClassDB::bind_method(D_METHOD("create_internal_image", "width", "height", "use_filter"),
                         &CaptureTexture::create_internal_image);

    // If you want to process automatically in Godot, you can register _process() and _ready()
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

void CaptureTexture::create_internal_image(int width, int height, bool use_filter) {
    // We create an Image of the desired size/format. 
    // RGBA8 is common; choose a format that suits your needs.
    internal_image = Image::create(width, height, false, Image::FORMAT_RGBA8);

    // Initialize the contents of the image in some way...
    // For instance, fill with opaque white:
    internal_image->lock();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            internal_image->set_pixel(x, y, Color(1.0, 1.0, 1.0, 1.0));
        }
    }
    internal_image->unlock();

    // Now create the texture from this image. The second argument is usage flags.
    // For dynamic usage in Godot 4, you can use TEXTURE_FLAGS_DEFAULT or 
    // TEXTURE_USAGE_2D | TEXTURE_USAGE_FILTER if needed, etc.
    create_from_image(internal_image, 0 /* usage_flags */);
    
    // If you want filter, repeats, or other flags, you can set them here, e.g.:
    // set_filter(use_filter);
    // set_repeat(false);
}

void CaptureTexture::update_texture_contents() {
    if (internal_image.is_null()) {
        return;
    }

    // Lock the image and update its pixels however you like:
    internal_image->lock();

    // Example: fill each pixel with random RGBA
    const int width = internal_image->get_width();
    const int height = internal_image->get_height();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float r = Math::randf();
            float g = Math::randf();
            float b = Math::randf();
            internal_image->set_pixel(x, y, Color(r, g, b, 1.0));
        }
    }

    internal_image->unlock();

    // Now push changes to the GPU. 
    // For a standard Texture2D, re-create or call update():
    //    create_from_image(internal_image, 0);
    // or if using Godot’s advanced APIs:
    //    update(internal_image);

    // The simplest route here is re-create from the image:
    create_from_image(internal_image, 0 /* usage_flags */);
}

