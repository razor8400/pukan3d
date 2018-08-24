#pragma once

#include "game_object.h"

namespace engine
{
	class scene : public game_object
	{
        DECLARE_CLASS
	public:
        void set_camera(camera* camera) { m_camera = camera; }
        const pointer<camera>& get_camera() const { return m_camera; }
    public:
        virtual void draw(renderer* r);
    private:
        pointer<camera> m_camera = nullptr;
	};
}
