local OS=os.get()

local definitions = {
	dir = {
		linux = "ls",
		windows = "dir"
	},
	BOOST =  {
		linux = "/usr/include", --customize
		windows = os.getenv("BOOST")
	},
	BOOST_LIB = {
		linux = "/usr/local/lib", --customize
		windows = path.join(os.getenv("BOOST"),"stage/lib")
	},
	links = {
	}
}

local cfg={}

for i,v in pairs(definitions) do
 cfg[i]=definitions[i][OS]
end

-- Apply to current "filter" (solution/project)
function DefaultConfig()
	location "Build"
	configuration "Debug"
		defines { "DEBUG", "_DEBUG" }
		objdir "Build/obj"
		targetdir "Build/Debug"
		flags { "Symbols" }
	configuration "Release"
		defines { "RELEASE" }
		objdir "Build/obj"
		targetdir "Build/Release"
		flags { "Optimize" }
	configuration "*" -- to reset configuration filter
end

function SetUpCompilerSpecificPostBuildEvent()
	configuration {"xcode*" }
		postbuildcommands {"$TARGET_BUILD_DIR/$TARGET_NAME"}

	configuration {"gmake"}
		postbuildcommands  { "$(TARGET)" }
		buildoptions { "-std=c++0x" }

	configuration {"codeblocks" }
		postbuildcommands { "$(TARGET_OUTPUT_FILE)"}

	configuration { "vs*"}
		postbuildcommands { "\"$(TargetPath)\"" }
end

-- A solution contains projects, and defines the available configurations
local sln=solution "undo-redo"
    location "Build"
	sln.absbasedir=path.getabsolute(sln.basedir)
	configurations { "Debug", "Release" }
	platforms { "native" }
	includedirs {
		sln.basedir,
		path.join(sln.basedir,"undoredo")
	}
	vpaths {
		["Headers"] = "**.h",
		["Sources"] = {"**.cc", "**.cpp"},
	}

----------------------------------------------------------------------------------------------------------------

local undoredotests=project "undoredotests"
	local basedir="undoredotests"
	kind "ConsoleApp"
	DefaultConfig()
	language "C++"
	files {
		path.join(basedir,"**.cpp"),
		path.join(basedir,"**.h")
	}
	uses {
		"gtest-lib",
		"gtest-main"
	}
	links {
		"gtest-lib",
		"gtest-main",
		cfg.links
	}
	SetUpCompilerSpecificPostBuildEvent()
