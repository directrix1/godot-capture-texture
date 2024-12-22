#pragma once

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

class CaptureTexture : public Texture2D {
    GDCLASS(CaptureTexture, Texture2D);

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

    // Called every frame (if we want automated updates).
    void _process(double delta);

    // Example function to manually update texture from code.
    void update_texture_contents();

    // Helper to initialize your image with a desired size and format.
    void create_internal_image(int width, int height, bool use_filter = false);
};

} // namespace godot
