#pragma once

#include <glibmm.h>
#include <thread>
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
	int32_t width;
	int32_t height;
	int32_t bytes_per_pixel;
};

enum portal_state {
	PORTAL_NEW = 0,
	PORTAL_INITED = 1,
	PORTAL_READY_FOR_PIPEWIRE = 2,
};

class PortalCapture {
private:
protected:
	std::thread _portal_thread;
	std::atomic<bool> _capturing;
	std::atomic<enum portal_state> _portal_state;
	RefPtr<MainLoop> _loop;
	XdpPortal *_portal;
	XdpSession *_xdpsession;
	GVariant *_xdpsession_streams;

	std::thread _pipewire_thread;

	int _pipewireNodeID;
	uint32_t _pipewireRemoteID;
	volatile unsigned char *_frame_buffers[PORTAL_CAPTURE_FRAME_BUFS];
	std::atomic<int> _active_frame;
	struct portal_capture_dims _dimensions;

	static void on_portal_screencast_session_started(GObject *source_object, GAsyncResult *res, PortalCapture *data);
	static void on_portal_create_screencast_done(GObject *source_object, GAsyncResult *res, PortalCapture *data);
	void setup_portal_screencast();
	void handoff_to_pipewire();
	void setup_pipewire();

public:
	void init();
	void startCapturing();
	bool isCapturing();
	void stopCapturing();
	struct portal_capture_dims getDimensions();
	int getLastFrameNumber();
	unsigned char *getLastFrame();
};
