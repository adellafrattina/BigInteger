flags { "MultiProcessorCompile" }
startproject "test"

outputdir = "%{cfg.buildcfg}-%{cfg.platform}"
config = "%{cfg.buildcfg}"
arch = "%{cfg.architecture}"
OS = "%{cfg.system}"
sruntime = "Off"
tdir = "../bin/" .. outputdir
odir = "../bin-int/" .. outputdir
