#include "base_element.hpp"
carbon::element::io_element::io_element() = default;

carbon::element::io_element::io_element(const std::string_view element_name, const io_vec4 element_bounds):
	name_(element_name),
	bounds_(element_bounds) {
}

carbon::element::io_element::~io_element() = default;

std::string carbon::element::io_element::get_name() const {
	return name_;
}

carbon::io_vec4 carbon::element::io_element::get_bounds() const {
	return bounds_;
}

carbon::io_vec2 carbon::element::io_element::get_size() const {
	return { bounds_.w, bounds_.h };
}

void carbon::element::io_element::set_bounds(const io_vec4 bounds) {
	bounds_ = bounds;
}

void carbon::element::io_element::set_y_offset(const int offset) {
	offset_.y = offset;
}

std::shared_ptr<carbon::element::io_element> carbon::element::io_element::get_parent() const {
	return parent_;
}

int carbon::element::io_element::get_total_height() const {
	return bounds_.h;
}

void carbon::element::io_element::set_visible(const bool visible) {
	visible_ = visible;
}

bool carbon::element::io_element::get_visible() const {
	return visible_;
}

carbon::io_vec2 carbon::element::io_element::get_absolute_position() const {
	auto x = bounds_.x + offset_.x;
	auto y = bounds_.y + offset_.y;

	for (auto parent = get_parent(); !!parent; parent = parent->get_parent()) {
		x += parent->bounds_.x + offset_.x;
		y += parent->bounds_.y + offset_.y;
	}

	return {x, y};
}

carbon::io_vec2 carbon::element::io_element::get_relative_position() const {
	auto x = bounds_.x;
	auto y = bounds_.y + offset_.y;

	for (auto parent = get_parent(); !!parent; parent = parent->get_parent()) {
		x += parent->bounds_.x;
		y += parent->bounds_.y + parent->offset_.y;
	}

	return {x, y};
}

std::vector<std::shared_ptr<carbon::element::io_element>>& carbon::element::io_element::get_children() {
	return children_;
}

std::shared_ptr<carbon::element::io_element> carbon::element::io_element::add_child(std::shared_ptr<io_element> item) {
	item->parent_ = shared_from_this();
	children_.emplace(children_.end(), item);
	return item;
}

bool carbon::element::io_element::remove_child(const std::string_view element_name) {
	for (size_t i = 0; i < children_.size(); ++i) {
		if (children_.at(i)->get_name() == element_name) {
			children_.erase(children_.begin() + i);
			return true;
		}
	}

	return false;
}

std::shared_ptr<carbon::element::io_element> carbon::element::io_element::get_top_parent() const {
	for (auto it = parent_;; it = it->get_parent())
		if (!it->get_parent())
			return it;
}

void carbon::element::io_element::set_disabled(const bool disabled) {
	disabled_ = disabled;

	for (auto& it : children_)
		it->set_disabled(disabled);
}

void carbon::element::io_element::set_disabled_callbacks(const bool disabled) {
	get_top_parent()->set_disabled(disabled);
	disabled_ = false;
}

int carbon::element::io_element::get_relative_x() const {
	auto x = bounds_.x;
	for (auto parent = get_parent(); !!parent; parent = parent->get_parent())
		x += parent->bounds_.x;

	return x;
}

int carbon::element::io_element::get_relative_y() const {
	auto y = bounds_.y + offset_.y;
	for (auto parent = get_parent(); !!parent; parent = parent->get_parent())
		y += parent->bounds_.y + parent->offset_.y;

	return y;
}

std::shared_ptr<carbon::element::io_element> carbon::element::io_element::find_child_element(
	const std::string_view element_name) {
	if (children_.empty())
		return nullptr;

	for (auto& it : children_) {
		if (!it->get_name().empty() && it->get_name() == element_name)
			return it;

		auto it_find = it->find_child_element(element_name);
		if (it_find != it) return it_find;
	}

	return shared_from_this();
}

int carbon::element::io_element::get_window_id() const {
	return window_id_;
}

bool carbon::element::io_element::is_on_top() const {
	return window_id_ == ctx->io.focused_window_id;
}

bool carbon::element::io_element::hovered() {
	// element is not active
	if (disabled_)
		return false;
	
	return input::is_mouse_over({ get_absolute_position(), get_size() });
}

void carbon::element::io_element::draw() {
}

void carbon::element::io_element::reset() {
	offset_.y = 0;
}
