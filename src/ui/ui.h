#pragma once

#include "ui_context.h"
#include "ui_renderer.h"


namespace visionlab::ui {


class UI
{

public:

    UI();

    ~UI();


    void set_renderer(
        UIRenderer* renderer
    );


    bool render(
        const UIContext& context
    );


private:

    UIRenderer* renderer_ = nullptr;

};


}
