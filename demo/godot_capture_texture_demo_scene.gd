extends Node2D

var gct : CaptureTexture

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	gct = $Sprite2D.texture


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	gct.update_texture_contents()
