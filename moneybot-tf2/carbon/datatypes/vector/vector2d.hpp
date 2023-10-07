#ifndef _CARBON_DATATYPES_VECTOR_VECTOR2D_HPP_
#define _CARBON_DATATYPES_VECTOR_VECTOR2D_HPP_

#include <algorithm>
#include <stack>
#include <vector>

namespace carbon {
	// todo: easing and more between 2 points for animations
	struct io_vec2 {
		int x, y;

		io_vec2() : x(0), y(0) { }

		io_vec2(const int _x, const int _y) : x(_x), y(_y) { }

		io_vec2 operator+(const io_vec2& rhs) const {
			return { this->x + rhs.x, this->y + rhs.y };
		}

		io_vec2 operator-(const io_vec2& rhs) const {
			return { this->x - rhs.x, this->y - rhs.y };
		}

		io_vec2 operator*(const io_vec2& rhs) const {
			return { this->x * rhs.x, this->y * rhs.y };
		}

		io_vec2 operator/(const io_vec2& rhs) const {
			return { this->x / rhs.x, this->y / rhs.y };
		}

		io_vec2 operator*(const int rhs) const {
			return { this->x * rhs, this->y * rhs };
		}

		io_vec2 operator/(const int rhs) const {
			return { this->x / rhs, this->y / rhs };
		}

		io_vec2& operator*=(const int rhs) {
			this->x *= rhs;
			this->y *= rhs;
			return *this;
		}

		io_vec2& operator/=(const int rhs) {
			this->x /= rhs;
			this->y /= rhs;
			return *this;
		}

		io_vec2& operator+=(const io_vec2& rhs) {
			this->x += rhs.x;
			this->y += rhs.y;
			return *this;
		}

		io_vec2& operator-=(const io_vec2& rhs) {
			this->x -= rhs.x;
			this->y -= rhs.y;
			return *this;
		}

		io_vec2& operator*=(const io_vec2& rhs) {
			this->x *= rhs.x;
			this->y *= rhs.y;
			return *this;
		}

		io_vec2& operator/=(const io_vec2& rhs) {
			this->x /= rhs.x;
			this->y /= rhs.y;
			return *this;
		}

		[[nodiscard]] float length_2d() const {
			return sqrtf(x * x + y * y);
		}

		[[nodiscard]] float length_2d_sqrt() const {
			return x * x + y * y;
		}

		[[nodiscard]] float distance_2d(const io_vec2& v) const {
			return (*this - v).length_2d();
		}

		[[nodiscard]] float distance_2d_sqrt(const io_vec2& v) const {
			return (*this - v).length_2d_sqrt();
		}

		static void swap(io_vec2& a, io_vec2& b) {
			const auto t = a;
			a = b;
			b = t;
		}

		// https://www.geeksforgeeks.org/convex-hull-set-2-graham-scan/
		/**
		 * \brief find the convex hull of a list of points
		 * \param points list to find convex
		 * \return successful
		 */
		static bool convex_hull(std::vector<io_vec2>& points) {
			const auto orientation = [](const io_vec2 p, const io_vec2 q, const io_vec2 r) -> int {
				const auto val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

				// collinear 
				if (val == 0)
					return 0;

				// clock or counterclock wise 
				return val > 0 ? 1 : 2;
			};
			
			// find bottom most point
			auto min_y = points.at(0).y, min = 0;

			for (auto i = 1; i < points.size(); i++) {
				// pick bottom most
				// if there is a tie
				// choose left most
				if (points.at(i).y < min_y || min_y == points.at(i).y &&
					points.at(i).x < points.at(min).x)
					min_y = points[i].y, min = i;
			}

			// place the bottom most point at the first position
			const auto base = points.at(0);
			swap(points.at(0), points.at(min));

			std::sort(points.begin(), points.end(), [&base, orientation](io_vec2& a, io_vec2& b) -> bool {
				const auto o = orientation(base, a, b);

				if (o == 0)
					return base.distance_2d_sqrt(b) >= base.distance_2d_sqrt(a) ? true : false;

				return o == 2 ? true : false;
			});

			auto m = 1; // Initialize size of modified array 
			for (auto i = 1; i < points.size(); i++) {
				while (i < points.size() - 1 && orientation(base, points[i], points[i + 1]) == 0)
					i++;

				points[m] = points[i];
				m++;
			}

			if (m < 3)
				return false;

			std::stack<io_vec2> s;
			s.push(points.at(0));
			s.push(points.at(1));
			s.push(points.at(2));

			const auto next_to_top = [](std::stack<io_vec2>& s) -> io_vec2 {
				const auto p = s.top();
				s.pop();
				const auto res = s.top();
				s.push(p);
				return res;
			};

			for (auto i = 3; i < m; i++) {
				while (orientation(next_to_top(s), s.top(), points[i]) != 2)
					s.pop();
				
				s.push(points[i]);
			}

			// this is a bit ghetto
			points.clear();
			while (!s.empty()) {
				points.push_back(s.top());
				s.pop();
			}

			return true;
		}
	};
}

#endif