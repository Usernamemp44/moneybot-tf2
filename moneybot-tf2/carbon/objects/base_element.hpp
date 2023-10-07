#ifndef _CARBON_OBJECTS_BASE_ELEMENT_HPP_
#define _CARBON_OBJECTS_BASE_ELEMENT_HPP_

// todo: go through everything and clean

// ReSharper disable CppUnusedIncludeDirective

// common includes
#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../datatypes/datatypes.hpp"

#include "../util/draw.hpp"
#include "../util/input.hpp"
#include "../util/util.hpp"

namespace carbon::element {
	class io_element : public std::enable_shared_from_this<io_element> {
	protected:
		std::string name_;
		std::string text_;

		io_vec4 bounds_;
		io_vec2 offset_{};

		bool visible_ = true;
		bool disabled_ = false;

		int window_id_ = -1;

		std::shared_ptr<io_element> parent_;
		std::vector<std::shared_ptr<io_element>> children_;

		friend class io_groupbox;
	public:
		io_element();
		io_element(std::string_view element_name, io_vec4 element_bounds = {});

		virtual ~io_element();

		[[nodiscard]] std::string get_name() const;

		[[nodiscard]] io_vec4 get_bounds() const;
		[[nodiscard]] io_vec2 get_size() const;

		void set_bounds(io_vec4 bounds);

		void set_y_offset(int offset);

		std::shared_ptr<io_element> get_parent() const;

		[[nodiscard]] virtual int get_total_height() const;

		void set_visible(bool visible);
		[[nodiscard]] bool get_visible() const;

		[[nodiscard]] io_vec2 get_absolute_position() const;
		[[nodiscard]] io_vec2 get_relative_position() const;

		std::vector<std::shared_ptr<io_element>>& get_children();
		virtual std::shared_ptr<io_element> add_child(std::shared_ptr<io_element> item);
		bool remove_child(std::string_view element_name);

		std::shared_ptr<io_element> get_top_parent() const;

		void set_disabled(bool disabled);
		void set_disabled_callbacks(bool disabled);

		[[nodiscard]] int get_relative_x() const;
		[[nodiscard]] int get_relative_y() const;

		std::shared_ptr<io_element> find_child_element(std::string_view element_name);

		int get_window_id() const;
		bool is_on_top() const;

		virtual bool hovered();

		virtual void draw();
		virtual void reset();
	};
}

#endif