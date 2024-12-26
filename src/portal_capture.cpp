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

void PortalCapture::setup_pipewire() {
	_pipewireNodeID = xdp_session_open_pipewire_remote(_xdpsession);
	_xdpsession_streams = xdp_session_get_streams(_xdpsession);
	printf("\tstreams: %s\n", g_variant_print(_xdpsession_streams, false));
	GVariantIter *streamsIter = g_variant_iter_new(_xdpsession_streams);
	GVariantIter *streamPropsIter;
	uint32_t streamId;
	while (g_variant_iter_loop(streamsIter, "(ua{sv})", &streamId, &streamPropsIter)) {
		_pipewireRemoteID = streamId;
		GString propName;
		GVariant *propValue;
		while (g_variant_iter_loop(streamPropsIter, "{sv}", &propName, &propValue)) {
			if (std::string("size").compare(propName.str) == 0) {
				int32_t w, h;
				g_variant_get(propValue, "(ii)", &w, &h);
				_dimensions = {
					.width = w,
					.height = h,
					.bytes_per_pixel = 4,
				};
				printf("\tsize: %dx%d\n", _dimensions.width, _dimensions.height);
			}
		}
		//g_variant_iter_free(streamPropsIter);
	}
}

void PortalCapture::on_portal_screencast_session_started(GObject *source_object, GAsyncResult *res, PortalCapture *data) {
	if (xdp_session_start_finish(data->_xdpsession, res, NULL)) {
		printf("Session started!\n");
		data->setup_pipewire();
		printf("\tpw node id: %d\n", data->_pipewireNodeID);
		printf("\tpw remote id: %d\n", data->_pipewireRemoteID);
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
