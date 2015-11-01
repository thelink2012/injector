--[[
    Injector2 Build Script (OPTIONAL - FOR DEV AND TEST)
    Use 'premake5 --help' for help
--]]


--[[
    Options and Actions
--]]

newoption {
    trigger     = "outdir",
    value       = "path",
    description = "Output directory for the build files"
}
if not _OPTIONS["outdir"] then
    _OPTIONS["outdir"] = "build"
end

-- TODO '--test' option

--[[
    The Solution
--]]
solution "injector2"

    configurations { "Release", "Debug" }

    location( _OPTIONS["outdir"] )
    targetprefix "" -- no 'lib' prefix on gcc
    targetdir "bin"
    implibdir "bin"

    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "_SCL_SECURE_NO_WARNINGS"
    }

    includedirs {
        "include",
        "test/shared"
    }

    startproject "Sandbox"

    configuration "Debug*"
        flags { "Symbols" }
        
    configuration "Release*"
        defines { "NDEBUG" }
        optimize "Speed"

    project "Sandbox"
        language "C++"
        kind "ConsoleApp"

        flags { "NoPCH" }

        files {
            "include/**.hpp",
            "include/**.dox",
            --"test/shared/**.cpp",
            "test/shared/**.hpp",
            "test/sandbox.cpp"
        }

    project "Test"
        language "C++"
        kind "ConsoleApp"
        
        flags { "NoPCH" }
        
        files {
            "include/**.hpp",
            "include/**.dox",
            "test/**.cpp",
            "test/**.hpp",
        }
        excludes { "test/sandbox.cpp" }
        
        

