#pragma once

#include <glibmm.h>
extern "C" {
#include <libportal/portal-helpers.h>
#include <libportal/portal.h>
#include <libportal/remote.h>
#include <pipewire/pipewire.h>
#include <spa/debug/types.h>
#include <spa/param/video/format-utils.h>
#include <spa/param/video/type-info.h>
}
#include <atomic>

#define PORTAL_CAPTURE_FRAME_BUFS 3

using namespace Glib;

struct portal_capture_dims {
	unsigned int width;
	unsigned int height;
	unsigned int bytes_per_pixel;
};

class PortalCapture : public Object {
private:
protected:
	std::atomic<bool> _capturing;
	RefPtr<MainLoop> _loop;
	XdpPortal *_portal;
	XdpSession *_xdpsession;
	volatile unsigned char *_frame_buffers[PORTAL_CAPTURE_FRAME_BUFS];
	std::atomic<int> _active_frame;
	std::atomic<struct portal_capture_dims> _dimensions;

	static void on_portal_screencast_session_started(GObject *source_object, GAsyncResult *res, PortalCapture *data);
	static void on_portal_create_screencast_done(GObject *source_object, GAsyncResult *res, PortalCapture *data);
	void setup_portal_screencast();

public:
	void init();
	void startCapturing();
	bool isCapturing();
	void stopCapturing();
	struct portal_capture_dims getDimensions();
	int getLastFrameNumber();
	unsigned char *getLastFrame();
};