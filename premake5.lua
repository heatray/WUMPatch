workspace "WUMAnniversaryPatch"
   configurations { "Release", "Debug" }
   platforms { "Win32" }
   architecture "x32"
   location "build"
   -- objdir ("build/obj")
   -- buildlog ("build/log/%{prj.name}.log")
   buildoptions {"-std:c++latest"}

   kind "SharedLib"
   language "C++"
   targetextension ".asi"
   characterset ("UNICODE")
   staticruntime "On"

   files { "source/stdafx.h", "source/stdafx.cpp" }
   files { "source/resources/VersionInfo.rc" }

   includedirs { "source" }
   includedirs { "source/injector" }
   includedirs { "source/inireader" }

   date = os.outputof("date /t")
   yy = tonumber(string.sub(date, 9, 10))
   mm = tonumber(string.sub(date, 4, 5))
   dd = tonumber(string.sub(date, 1, 2))
   build = 0

   defines {
      "rsc_MajorVersion=%{yy}",
      "rsc_MinorVersion=%{mm}",
      "rsc_RevisionID=%{dd}",
      "rsc_BuildID=%{build}"
   }
   defines {
      "rsc_StringVersion=\"%{yy}.%{mm}.%{dd}.%{build}\"",
      "rsc_InternalName=\"%{prj.name}\"",
      "rsc_ProductName=\"Anniversary Patch\"",
      "rsc_OriginalFilename=\"%{prj.name}.dll\"",
      "rsc_CompanyName=\"heatray\"",
      "rsc_LegalCopyright=\"Copyright (C) heatray\"",
      "rsc_UpdateUrl=\"https://github.com/heatray/WUMPatch/releases\""
   }
   
   filter "configurations:Debug*"
      defines "DEBUG"
      symbols "On"

   filter "configurations:Release*"
      defines "NDEBUG"
      optimize "On"

project "WUM.Patch"
   files { "source/patch.cpp" }

   defines { "rsc_FileDescription=\"Patch for Worms Ultimate Mayhem\"" }

project "WUM.Loader"
   files { "source/loader.cpp" }

   defines { "rsc_FileDescription=\"Loader for Worms Ultimate Mayhem\"" }
