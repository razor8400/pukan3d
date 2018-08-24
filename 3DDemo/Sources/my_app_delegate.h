#pragma once

#include "engine.h"

class my_app_delegate : public engine::app_delegate
{
public:
	bool application_launched(engine::application* application) override;
    void application_terminated(engine::application* application) override;
    void application_enter_background(engine::application* application) override;
    void application_enter_foreground(engine::application* application) override;
private:
    
};
