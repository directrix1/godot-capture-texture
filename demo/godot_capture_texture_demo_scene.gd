extends Node2D

var gct : CaptureTexture

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	gct = CaptureTexture.new()
	gct.connect_shm_buffer("/srb_video_test", "video_frames", 1920, 1080)
	$Sprite2D.texture = gct


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	gct.update_texture_contents()
