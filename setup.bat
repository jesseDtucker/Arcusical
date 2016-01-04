cd boost
call bootstrap.bat
b2 headers
cd ..\protobuf
mkdir vs
cd vs
cmake -G "Visual Studio 14 2015 Win64" -Dprotobuf_BUILD_TESTS=OFF ..\cmake
call fixupProjects.bat
cd ..\..\LocalMusicStore\proto
mkdir impl
cd ..\..\