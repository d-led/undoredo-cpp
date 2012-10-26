local OS=os.get()

local definitions = {
	dir = {
		linux = "ls",
		windows = "dir"
	},
	BOOST =  {
		linux = "/home/dled/src/boost_1_51_0", --customize
		windows = os.getenv("BOOST")
	},
	BOOST_LIB = {
		linux = path.join("/home/dled/src/boost_1_51_0/","stage/lib"), --customize
		windows = path.join(os.getenv("BOOST"),"stage/lib")
	},
	links = {
		linux = {"pthread"},
		windows=""
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

function CompilerSpecificConfiguration()
	configuration {"xcode*" }
		postbuildcommands {"$TARGET_BUILD_DIR/$TARGET_NAME"}

	configuration {"gmake"}
		postbuildcommands  { "$(TARGET)" }
		buildoptions { "-std=gnu++0x" }

	configuration {"codeblocks" }
		postbuildcommands { "$(TARGET_OUTPUT_FILE)"}

	configuration { "vs*"}
		postbuildcommands { "\"$(TargetPath)\"" }
end

function ConfigureGtestTuple()
	configuration { "vs2010"}
		defines { "GTEST_HAS_TR1_TUPLE=0" }
end

-- A solution contains projects, and defines the available configurations
local sln=solution "undoredo-cpp"
    location "Build"
	sln.absbasedir=path.getabsolute(sln.basedir)
	configurations { "Debug", "Release" }
	platforms { "native" }
	includedirs {
		sln.basedir,
		path.join(sln.basedir,"gtest"),
		path.join(sln.basedir,"undoredo")
	}
	vpaths {
		["Headers"] = "**.h",
		["Sources"] = {"**.cc", "**.cpp"},
	}

----------------------------------------------------------------------------------------------------------------

local gtest=project "gtest-lib"
	local basedir="gtest/gtest"
	kind "StaticLib"
	DefaultConfig()
	language "C++"
	files {
		path.join(basedir,"gtest-all.cc"),
		path.join(basedir,"gtest.h")
	}
	ConfigureGtestTuple()

----------------------------------------------------------------------------------------------------------------

local gtestmain=project "gtest-main"
	local basedir="gtest/gtest"
	kind "StaticLib"
	DefaultConfig()
	language "C++"
	files {
		path.join(basedir,"gtest_main.cc")
	}
	ConfigureGtestTuple()
	
----------------------------------------------------------------------------------------------------------------

local undoredo=project "undoredo"
	local basedir="undoredo"
	kind "StaticLib"
	DefaultConfig()
	language "C++"
	files {
		path.join(basedir,"**.cpp"),
		path.join(basedir,"**.h")
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
	CompilerSpecificConfiguration()
	ConfigureGtestTuple()
