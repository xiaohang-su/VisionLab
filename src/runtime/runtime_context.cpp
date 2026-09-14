#include "runtime_context.h"


namespace visionlab {
namespace runtime {


RuntimeContext::RuntimeContext(
    logging::Logger* logger,
    config::Config* config
)
    :
    logger_(logger),
    config_(config)
{
}


logging::Logger* RuntimeContext::logger()
{
    return logger_;
}


config::Config* RuntimeContext::config()
{
    return config_;
}


}
}