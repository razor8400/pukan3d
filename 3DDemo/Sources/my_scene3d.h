#pragma once

#include "engine.h"

class my_scene3d : public engine::scene
{
public:
    void on_enter() override;
    void on_exit() override;
    void draw(engine::renderer* r) override;
    void update(float dt) override;
private:
    bool on_touch_began(const math::vector2d& location);
    void on_touch_moved(const math::vector2d& location);
    void on_touch_ended(const math::vector2d& location);
private:
    engine::touch_listener* m_listener = nullptr;
    math::vector2d m_mouse;

	bool m_touch_began = false;

	float m_vertical_scroll = 0;
	float m_horisontal_scroll = 0;
};
