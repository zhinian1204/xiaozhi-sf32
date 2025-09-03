#include "mcp_api.h"
#include "mcp_server.h"
#include "cJSON.h"
#include <rtthread.h>
#include <string.h>
#include "rgbled_mcp.h"


extern "C" 
{
    //注册IoT设备(可多个设备)
    void McpServer_ParseMessage(const char* message)
    {
        McpServer::GetInstance().ParseMessage(message);                
    }

    void MCP_RGBLED_CLOSE()
    {
        if (RGBLEDTool::IsLightOn())
        {
            RGBLEDTool::is_color_cycling_ = false;
            rt_thread_mdelay(100);
            // GetRGBLEDController().SetColor(rgb_color_arry[4].color);
            GetRGBLEDController();
        }
    }  




} // extern "C"