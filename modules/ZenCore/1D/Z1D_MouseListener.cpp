#include "Z1D_MouseListener.h"
#include "scene/3d/camera_3d.h"
#include "scene/main/viewport.h"

void Z1D_MouseListener::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_target_2d", "path"), &Z1D_MouseListener::set_target_2d);
    ClassDB::bind_method(D_METHOD("get_target_2d"),         &Z1D_MouseListener::get_target_2d);

    ClassDB::bind_method(D_METHOD("set_target_3d", "path"), &Z1D_MouseListener::set_target_3d);
    ClassDB::bind_method(D_METHOD("get_target_3d"),         &Z1D_MouseListener::get_target_3d);

    ClassDB::bind_method(D_METHOD("_input", "event"),       &Z1D_MouseListener::_input);

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "target_2d", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Node2D"), "set_target_2d", "get_target_2d");
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "target_3d", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Node3D"), "set_target_3d", "get_target_3d");

    ADD_SIGNAL(MethodInfo("on_hover",
        PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_NODE_TYPE, "Node"),
        PropertyInfo(Variant::BOOL, "hovered")));

    ADD_SIGNAL(MethodInfo("on_mouse_click",
        PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_NODE_TYPE, "Node"),
        PropertyInfo(Variant::OBJECT, "event", PROPERTY_HINT_RESOURCE_TYPE, "InputEventMouseButton")));
}

void Z1D_MouseListener::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY: {
            set_process_input(true);
            _update_targets();
        } break;
    }
}

void Z1D_MouseListener::_update_targets() {
    target_2d = nullptr;
    target_3d = nullptr;

    if (!is_inside_tree()) {
        return;
    }

    if (!target_2d_path.is_empty()) {
        target_2d = Object::cast_to<Node2D>(get_node_or_null(target_2d_path));
    }
    if (!target_3d_path.is_empty()) {
        target_3d = Object::cast_to<Node3D>(get_node_or_null(target_3d_path));
    }

    _rebuild_tracked_children();
}

void Z1D_MouseListener::_rebuild_tracked_children() {
    HashMap<Node *, bool> old_hovered = hovered_children;
    hovered_children.clear();

    // Add children of target_2d
    if (target_2d) {
        for (int i = 0; i < target_2d->get_child_count(); i++) {
            Node *child = target_2d->get_child(i);
            if (old_hovered.has(child)) {
                hovered_children[child] = old_hovered[child];
            } else {
                hovered_children[child] = false;
            }
        }
    }

    // Add children of target_3d
    if (target_3d) {
        for (int i = 0; i < target_3d->get_child_count(); i++) {
            Node *child = target_3d->get_child(i);
            if (old_hovered.has(child)) {
                hovered_children[child] = old_hovered[child];
            } else {
                hovered_children[child] = false;
            }
        }
    }
}

void Z1D_MouseListener::set_target_2d(const NodePath &p_path) {
    target_2d_path = p_path;
    _update_targets();
}

NodePath Z1D_MouseListener::get_target_2d() const {
    return target_2d_path;
}

void Z1D_MouseListener::set_target_3d(const NodePath &p_path) {
    target_3d_path = p_path;
    _update_targets();
}

NodePath Z1D_MouseListener::get_target_3d() const {
    return target_3d_path;
}

void Z1D_MouseListener::_input(const Ref<InputEvent> &p_event) {
    Ref<InputEventMouseMotion> motion = p_event;
    Ref<InputEventMouseButton> button = p_event;

    // Rebuild in case children changed
    _rebuild_tracked_children();

    if (motion.is_valid()) {
        _check_hover(motion->get_position());
    }

    if (button.is_valid() && button->is_pressed()) {
        for (KeyValue<Node *, bool> &kv : hovered_children) {
            if (kv.value) {
                emit_signal("on_mouse_click", kv.key, button);
            }
        }
    }
}

void Z1D_MouseListener::_check_hover(const Vector2 &mouse_pos) {
    for (KeyValue<Node *, bool> &kv : hovered_children) {
        bool now_hovered = _point_in_node(kv.key, mouse_pos);
        if (now_hovered != kv.value) {
            kv.value = now_hovered;
            emit_signal("on_hover", kv.key, now_hovered);
        }
    }
}

bool Z1D_MouseListener::_point_in_node(Node *p_node, const Vector2 &mouse_pos) {
    if (!p_node || !p_node->is_inside_tree()) {
        return false;
    }

    // Check Node2D
    Node2D *node_2d = Object::cast_to<Node2D>(p_node);
    if (node_2d) {
        Vector2 screen_pos = node_2d->get_global_transform_with_canvas().get_origin();
        return mouse_pos.distance_to(screen_pos) < 50.0f; // Placeholder radius
    }

    // Check Node3D
    Node3D *node_3d = Object::cast_to<Node3D>(p_node);
    if (node_3d) {
        Viewport *vp = node_3d->get_viewport();
        if (!vp) {
            return false;
        }
        Camera3D *camera = vp->get_camera_3d();
        if (!camera) {
            return false;
        }
        Vector3 world_pos = node_3d->get_global_position();
        if (!camera->is_position_behind(world_pos)) {
            Vector2 screen_pos = camera->unproject_position(world_pos);
            return mouse_pos.distance_to(screen_pos) < 50.0f; // Placeholder radius
        }
    }

    return false;
}
