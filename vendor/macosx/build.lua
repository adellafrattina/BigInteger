workspace "BigInteger"
	location "../../build/"

	printf("OS: MacOSX")
	system "macosx"
	configurations { "Debug-Static", "Release-Static", "Debug-Shared", "Release-Shared" }
	platforms { "Universal" }
	filter { "platforms:Universal" }
		architecture "universal"

	filter {}

	include "../settings.lua"
	include "../projects.lua"
