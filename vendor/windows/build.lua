workspace "BigInteger"
	location "../../build/"

	printf("OS: Windows")
	system "windows"
	configurations { "Debug-Static", "Release-Static", "Debug-DLL", "Release-DLL" }
	platforms { "x86", "x64" }
	filter { "platforms:x86" }
		architecture "x86"
	filter { "platforms:x64" }
		architecture "x86_64"

	filter {}

	include "../settings.lua"
	include "../projects.lua"
