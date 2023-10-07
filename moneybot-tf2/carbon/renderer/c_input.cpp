#include "c_input.hpp"

#include <map>
#include <mutex>
#include <Windows.h>

namespace input {
	carbon::io_vec2 _mouse_position;
	int _scroll_state;

	std::mutex _key_mutex;
	std::map<uint32_t, bool> _key_map, _key_read, _key_read_update;
}

void input::capture_mouse_move(unsigned long lparam) {
	_mouse_position = { LOWORD(lparam), HIWORD(lparam) };
}

carbon::io_vec2 input::get_mouse_position() {
	return _mouse_position;
}

int input::get_scroll_state() {
	return _scroll_state;
}

void input::clear_scroll() {
	_scroll_state = 0;
}

// ghetto
bool input::register_key_press(UINT message, WPARAM wparam) {
	if (message == 255)
		return false;

	std::lock_guard<std::mutex> guard(_key_mutex);

	switch (message) {
		/*case WM_CHAR:
			if (wparam > 0 && wparam < 0x10000)
				_last_char = static_cast<char>(wparam);
			return true;*/
	case WM_KEYDOWN:
		if (wparam >= 0 && wparam < 256)
			_key_map[wparam] = true;
		return true;
	case WM_KEYUP:
		if (wparam >= 0 && wparam < 256) {
			_key_map[wparam] = false;
			_key_read[wparam] = false;
			_key_read_update[wparam] = false;
		}
		return true;
	case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
		if (GET_XBUTTON_WPARAM(wparam) & XBUTTON1)
			_key_map[VK_XBUTTON1] = true;
		else if (GET_XBUTTON_WPARAM(wparam) & XBUTTON2)
			_key_map[VK_XBUTTON2] = true;
		return true;
	case WM_XBUTTONUP:
		if (GET_XBUTTON_WPARAM(wparam) & XBUTTON1) {
			_key_map[VK_XBUTTON1] = false;
			_key_read[VK_XBUTTON1] = false;
			_key_read_update[VK_XBUTTON1] = false;
		}
		else if (GET_XBUTTON_WPARAM(wparam) & XBUTTON2) {
			_key_map[VK_XBUTTON2] = false;
			_key_read[VK_XBUTTON2] = false;
			_key_read_update[VK_XBUTTON2] = false;
		}
		return true;
	case WM_SYSKEYDOWN:
		if (wparam >= 0 && wparam < 256)
			_key_map[wparam] = true;
		return true;
	case WM_SYSKEYUP:
		if (wparam >= 0 && wparam < 256) {
			_key_map[wparam] = false;
			_key_read[wparam] = false;
			_key_read_update[wparam] = false;
		}
		return true;
	case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
		_key_map[VK_MBUTTON] = true;
		return true;
	case WM_MBUTTONUP:
		_key_map[VK_MBUTTON] = false;
		_key_read[VK_MBUTTON] = false;
		_key_read_update[VK_MBUTTON] = false;
		return true;
	case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
		_key_map[VK_LBUTTON] = true;
		return true;
	case WM_LBUTTONUP:
		_key_map[VK_LBUTTON] = false;
		_key_read[VK_LBUTTON] = false;
		_key_read_update[VK_LBUTTON] = false;
		return true;
	case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
		_key_map[VK_RBUTTON] = true;
		return true;
	case WM_RBUTTONUP:
		_key_map[VK_RBUTTON] = false;
		_key_read[VK_RBUTTON] = false;
		_key_read_update[VK_RBUTTON] = false;
		return true;
	case WM_MOUSEWHEEL:
		_scroll_state = GET_WHEEL_DELTA_WPARAM(wparam) > 0 ? 1 : -1;
		//_scroll_state = GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA;
		return true;
	default:
		return false;
	}
}

bool input::key_down(const uint32_t key) {
	return _key_map[key];
}

bool input::key_pressed(const uint32_t key) {
	std::lock_guard<std::mutex> guard(_key_mutex);

	if (!_key_map[key])
		return false;

	if (_key_read[key])
		return false;

	_key_read_update[key] = true;

	return true;
}

void input::update() {
	_key_read = _key_read_update;
	input::clear_scroll(); // clear for next time around
}
