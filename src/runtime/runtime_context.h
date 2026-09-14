#pragma once

namespace visionlab {

namespace logging {
class Logger;
}

namespace config {
class Config;
}

namespace runtime {

class RuntimeContext
{
public:

    RuntimeContext(
        logging::Logger* logger,
        config::Config* config
    );

    logging::Logger* logger();

    config::Config* config();

private:

    logging::Logger* logger_;
    config::Config* config_;

};

}

}