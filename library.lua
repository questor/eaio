
require "tundra.syntax.glob"
require "tundra.syntax.files"
local native = require "tundra.native"

StaticLibrary {
   Name = "eaio",
   Sources = {
      FGlob {
         Dir = _G.LIBROOT_EAIO,
         Extensions = { ".h", ".cpp" },
         Filters = {
            -- these are included directly into cpp-files
            {Config="ignore"; Pattern="Win32/"},
            {Config="ignore"; Pattern="Unix/"},
            {Config="ignore"; Pattern="StdC/"},
         },
      },
   },
   Env = {
      CPPPATH = {
         "extlibs",
      },
   },
   Depends = { "eastl" },
}
