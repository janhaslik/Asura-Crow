#include <string>

namespace jetpp
{
    enum Methods
    {
        Get,
        Post,
        Put,
        Patch,
        Delete,
        Options
    };
    Methods stringToMethod(std::string method);

}