#pragma once

#include "ui_renderer.h"


namespace visionlab::ui {


class MockUI
    : public UIRenderer
{

public:

    const char* name() const override;


    bool render(
        const UIContext& context
    ) override;

};


}
