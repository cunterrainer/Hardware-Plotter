#ifdef LINUX
#include <string>
#include <vector>
#include <filesystem>
#include <string_view>

#include "Log.h"
#include "SerialLinux.h"
#include "RenderWindow.h"

namespace Serial
{
    std::vector<Port> PortListener::GetPorts() noexcept 
    {
        const std::filesystem::path p("/dev/");
        if(!std::filesystem::exists(p))
        {
            const std::string errorMsg = "[PortListener] Failed to query ports, folder: '" + p.string() + "' doesn't exist";
            Err << errorMsg << Endl;
            MsgBoxError(errorMsg.c_str());
            return std::vector<Port>();
        }

        std::vector<Port> ports;
        for (const auto& di : std::filesystem::directory_iterator(p)) 
        {
            const std::string path = di.path().generic_string();
            if(path.size() < 11)
                continue;
            const std::string_view tty(&path[5], 6);
            if(tty == "ttyUSB" || tty == "ttyACM")
                ports.push_back({path, std::string()});
        }
        return ports;
    }
}
#endif