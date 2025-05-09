project "bi-core"
	location "../build/bi-core/"
	staticruntime ("" .. sruntime .. "")
	language "C++"
	cppdialect "C++20"
	targetdir ("" .. tdir .. "/%{prj.name}")
	objdir ("" .. odir .. "/%{prj.name}")
	targetname ("BigInteger")

	files {

		"../include/**.hpp",
		"../src/**.hpp",
		"../src/**.cpp"
	}

	includedirs {

		"../include",
		"../src"
	}

	-- Windows
	filter "system:Windows"
		defines "BI_PLATFORM_WINDOWS"

	-- MacOSX
	filter "system:macosx"
		systemversion "latest"
		defines "BI_PLATFORM_MACOSX"

		externalincludedirs {

			"../include",
			"../src"
		}

	-- Debug
	filter "configurations:Debug*"
		defines "BI_DEBUG"
		runtime "Debug"
		symbols "On"
		optimize "Off"

	-- Release
	filter "configurations:Release*"
		defines "BI_RELEASE"
		runtime "Release"
		symbols "Off"
		optimize "On"

	-- Static
	filter "configurations:*Static"
		kind "StaticLib"
		defines "BI_STATIC"

	-- Shared
	filter "configurations:*DLL or *Shared"
		kind "SharedLib"

	filter "configurations:*DLL"
		defines "BI_BUILD_DLL"

	filter "configurations:*Shared"
		defines "BI_BUILD_SHARED"

	-- Suffixes

	filter { "Debug*", "kind:StaticLib" }
		targetsuffix "-s-d"

	filter { "Debug*", "kind:SharedLib" }
		targetsuffix "-d"

	filter { "Release*", "kind:StaticLib" }
		targetsuffix "-s"

	filter { "Release*", "kind:SharedLib" }
		targetsuffix ""

	filter {}

-- Post build commands
filter "configurations:*DLL or *Shared"
	postbuildcommands {

		("{MKDIR} ../" .. tdir .. "/test"),
		("{COPYDIR} %{cfg.buildtarget.relpath} ../" .. tdir .. "/test")
	}
