#pragma once

#include "ui_context.h"


namespace visionlab::ui {


class UIRenderer
{

public:

    virtual ~UIRenderer() = default;


    virtual const char* name() const = 0;


    virtual bool render(
        const UIContext& context
    ) = 0;

};


}
