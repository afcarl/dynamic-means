solution "Examples"
	configurations{"debug", "release"}
	project "DynMeansExample"
		kind "ConsoleApp"
		language "C++"
		location "build"
		files {"maindm.cpp"}
		links {"lpsolve55"}
		includedirs{"/usr/local/include/eigen3", "/usr/local/include/dynmeans"}
		configuration "debug"
			flags{"Symbols", "ExtraWarnings"}
			buildoptions{"-std=c++0x"}
		configuration "release"
			flags{"Optimize"}
			buildoptions{"-std=c++0x"}
	project "SpecDynMeansExample"
		kind "ConsoleApp"
		language "C++"
		location "build"
		files {"mainsdm.cpp"}
		links {"lpsolve55", "gurobi_c++", "gurobi56"}
		includedirs{"/usr/local/include/eigen3", "/opt/gurobi563/linux64/include", "/usr/local/include/dynmeans"}
		libdirs{"/opt/gurobi563/linux64/lib"}
		configuration "debug"
			flags{"Symbols", "ExtraWarnings"}
			buildoptions{"-std=c++0x"}
		configuration "release"
			flags{"Optimize"}
			buildoptions{"-std=c++0x"}
	project "KernDynMeansExample"
		kind "ConsoleApp"
		language "C++"
		location "build"
		files {"mainkdm.cpp"}
		links {"lpsolve55", "gurobi_c++", "gurobi56"}
		includedirs{"/usr/local/include/eigen3", "/opt/gurobi563/linux64/include", "/usr/local/include/dynmeans"}
		libdirs{"/opt/gurobi563/linux64/lib"}
		configuration "debug"
			flags{"Symbols", "ExtraWarnings"}
			buildoptions{"-std=c++0x"}
		configuration "release"
			flags{"Optimize"}
			buildoptions{"-std=c++0x"}
