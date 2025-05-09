project "test"
	location "../build/test/"
	staticruntime ("" .. sruntime .. "")
	language "C++"
	cppdialect "C++20"
	targetdir ("" .. tdir .. "/%{prj.name}")
	objdir ("" .. odir .. "/%{prj.name}")
	kind "ConsoleApp"

	files {

		"../test/**"
	}

	includedirs {

		"../include"
	}

	links {

		"bi-core"
	}

	-- MacOSX
	filter "system:macosx"
		systemversion "latest"

		externalincludedirs {

			"../include"
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
		defines "BI_STATIC"
