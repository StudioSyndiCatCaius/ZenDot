#pragma once

#include "scene/main/node.h"
#include "core/io/dir_access.h"

// ---------------------------------------------------------------------------
// Z1D_DriveReader
// Polls the OS for connected drives and emits signals when drives are
// inserted or removed. Add to the scene tree and it runs automatically.
// ---------------------------------------------------------------------------
class Z1D_DriveReader : public Node {
	GDCLASS(Z1D_DriveReader, Node);

	float poll_interval = 2.0f;  // Seconds between drive-list checks
	float _elapsed      = 0.0f;  // Time accumulator

	PackedStringArray _known_drives; // Snapshot from the last poll

	// Returns the current list of drive root paths from the OS.
	PackedStringArray _get_current_drives() const;

	// Diffs _known_drives against the current list and emits signals.
	void _check_drives();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	// How often (in seconds) to poll for drive changes.
	void  set_poll_interval(float p_interval);
	float get_poll_interval() const;

	// Returns the last known drive list without triggering a new poll.
	PackedStringArray get_drives() const;

	// Force an immediate poll (useful on startup or after a known event).
	void refresh();
};
