#include "portal_capture.hpp"
#include "gio/gio.h"
#include <glib.h>
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

using namespace Glib;

void PortalCapture::init() {
	_active_frame = 0;
	_capturing = false;
}

void PortalCapture::on_portal_screencast_session_started(GObject *source_object, GAsyncResult *res, PortalCapture *data) {
	if (xdp_session_start_finish(data->_xdpsession, res, NULL)) {
		printf("Session started!\n");
	} else {
		printf("Session not started!\n");
	}
}

void PortalCapture::on_portal_create_screencast_done(GObject *source_object, GAsyncResult *res, PortalCapture *data) {
	printf("Callback called\n");
	data->_xdpsession = xdp_portal_create_screencast_session_finish(data->_portal, res, NULL);
	xdp_session_start(data->_xdpsession,
			NULL,
			NULL,
			(GAsyncReadyCallback)(&data->on_portal_screencast_session_started),
			data);
}

void PortalCapture::setup_portal_screencast() {
	this->_portal = xdp_portal_new();
	xdp_portal_create_screencast_session(this->_portal,
			XdpOutputType(XDP_OUTPUT_MONITOR | XDP_OUTPUT_VIRTUAL | XDP_OUTPUT_WINDOW),
			XDP_SCREENCAST_FLAG_NONE,
			XDP_CURSOR_MODE_HIDDEN,
			XDP_PERSIST_MODE_TRANSIENT,
			NULL, // const char *restore_token,
			NULL, // int *cancellable,
			(GAsyncReadyCallback)(&this->on_portal_create_screencast_done), // int callback,
			this);
	this->_loop->run();
}

void PortalCapture::startCapturing() {
	this->_loop = MainLoop::create(false);
	this->setup_portal_screencast();
}

bool PortalCapture::isCapturing() {
	return _capturing;
}

void PortalCapture::stopCapturing() {}

struct portal_capture_dims PortalCapture::getDimensions() {
	return _dimensions;
}

int PortalCapture::getLastFrameNumber() {
	return _active_frame - 1;
}

unsigned char *PortalCapture::getLastFrame() {
	return NULL;
}

#ifdef PORTAL_CAPTURE_TEST
// Compile with:
//   g++ -D PORTAL_CAPTURE_TEST -o PortalCaptureTest portal_capture.cpp -latomic $(pkg-config glibmm-2.4 giomm-2.4 libportal libpipewire-0.3 --cflags --libs)
int main() {
	PortalCapture pc;
	pc.init();
	pc.startCapturing();
	return 0;
}
#endif
