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

	-- Static
	filter "configurations:*Static"
		defines "BI_STATIC"
