#pragma once

#include "vector2d.h"
#include "mat4.h"

namespace math
{
	class rect
	{
	public:
		rect();
		rect(float x, float y, float w, float h);
		rect(const vector2d& origin, const vector2d& size);

		bool contains(const vector2d& location) const;

		vector2d bottom_left() const;
		vector2d top_left() const;
		vector2d top_right() const;
		vector2d bottom_right() const;
		vector2d center() const;

		rect operator*(const float s) const;
		rect operator*(const mat4& m4) const;

		const vector2d& get_origin() const { return m_origin; }
		const vector2d& get_size() const { return m_size; }
	private:
		vector2d m_origin;
		vector2d m_size;
	};
}
