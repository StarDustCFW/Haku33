/*Copyright (c) 2020 D3fau4

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include <stdio.h>
#include <switch.h>

#include "spl.hpp"

namespace spl
{

    static constexpr u32 ExosphereHasRcmBugPatch = 65004;
    static constexpr u32 ExosphereEmummcType = 65007;
    
    const char *GetHardwareType(void)
    {
        Result ret = 0;
        u64 hardware_type = 4;
        static const char *const hardware[] = {
            "Icosa",  // Erista normal
            "Copper", // Erista prototype
            "Hoag",   // Mariko lite
            "Iowa",   // Mariko retail
            "Calcio", // Mariko prototype
            "Aula",   // nx-abcd board
            "Unknown"};

        if (R_FAILED(ret = splGetConfig(SplConfigItem_HardwareType, &hardware_type)) || hardware_type > 5)
        {
            return hardware[6];
        }
        else
            return hardware[hardware_type];
    }

    bool HasRCMbugPatched(void)
    {
        Result ret = 0;
        u64 has_rcm_bug_patch = 0;
        if (R_SUCCEEDED(ret = splGetConfig(static_cast<SplConfigItem>(ExosphereHasRcmBugPatch), &has_rcm_bug_patch)))
        {
            return has_rcm_bug_patch;
        }
        else
        {
            return false;
        }
    }

    bool HasEmummc(void)
    {
        Result ret = 0;
        u64 IsEmummc = 0;

        if (R_SUCCEEDED(ret = splGetConfig(static_cast<SplConfigItem>(ExosphereEmummcType), &IsEmummc)))
        {
            return IsEmummc;
        }
        else
        {
            return false;
        }
    }

} // namespace spl