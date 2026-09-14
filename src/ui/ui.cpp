#include "ui.h"


namespace visionlab::ui {


UI::UI() = default;


UI::~UI() = default;



void UI::set_renderer(
    UIRenderer* renderer
)
{
    renderer_ = renderer;
}



bool UI::render(
    const UIContext& context
)
{
    if (renderer_ == nullptr)
    {
        return false;
    }

    return renderer_->render(context);
}


}
