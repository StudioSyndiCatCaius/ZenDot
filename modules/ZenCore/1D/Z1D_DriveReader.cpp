#include "Z1D_DriveReader.h"

// ---------------------------------------------------------------------------
// Binding
// ---------------------------------------------------------------------------

void Z1D_DriveReader::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_poll_interval", "seconds"), &Z1D_DriveReader::set_poll_interval);
    ClassDB::bind_method(D_METHOD("get_poll_interval"),            &Z1D_DriveReader::get_poll_interval);
    ClassDB::bind_method(D_METHOD("get_drives"),                   &Z1D_DriveReader::get_drives);
    ClassDB::bind_method(D_METHOD("refresh"),                      &Z1D_DriveReader::refresh);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "poll_interval", PROPERTY_HINT_RANGE, "0.1,30.0,0.1"),
                 "set_poll_interval", "get_poll_interval");

    // Emitted with the root path of the drive (e.g. "E:/" on Windows, "/media/usb" on Linux).
    ADD_SIGNAL(MethodInfo("drive_connected",    PropertyInfo(Variant::STRING, "path")));
    ADD_SIGNAL(MethodInfo("drive_disconnected", PropertyInfo(Variant::STRING, "path")));
}

// ---------------------------------------------------------------------------
// Node notifications
// ---------------------------------------------------------------------------

void Z1D_DriveReader::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY: {
            set_process(true);
            // Seed the known list silently so the first real poll has a baseline.
            _known_drives = _get_current_drives();
        } break;

        case NOTIFICATION_PROCESS: {
            _elapsed += (float)get_process_delta_time();
            if (_elapsed >= poll_interval) {
                _elapsed = 0.0f;
                _check_drives();
            }
        } break;

        case NOTIFICATION_EXIT_TREE: {
            set_process(false);
        } break;
    }
}

// ---------------------------------------------------------------------------
// Core logic
// ---------------------------------------------------------------------------

PackedStringArray Z1D_DriveReader::_get_current_drives() const {
    PackedStringArray drives;
    Ref<DirAccess> da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
    if (da.is_valid()) {
        int count = da->get_drive_count();
        for (int i = 0; i < count; i++) {
            drives.push_back(da->get_drive(i));
        }
    }
    return drives;
}

void Z1D_DriveReader::_check_drives() {
    PackedStringArray current = _get_current_drives();

    // Detect newly added drives.
    for (int i = 0; i < current.size(); i++) {
        if (!_known_drives.has(current[i])) {
            emit_signal("drive_connected", current[i]);
        }
    }

    // Detect removed drives.
    for (int i = 0; i < _known_drives.size(); i++) {
        if (!current.has(_known_drives[i])) {
            emit_signal("drive_disconnected", _known_drives[i]);
        }
    }

    _known_drives = current;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void  Z1D_DriveReader::set_poll_interval(float p_interval) { poll_interval = MAX(0.1f, p_interval); }
float Z1D_DriveReader::get_poll_interval() const           { return poll_interval; }

PackedStringArray Z1D_DriveReader::get_drives() const { return _known_drives; }

void Z1D_DriveReader::refresh() {
    _elapsed = 0.0f;
    _check_drives();
}
