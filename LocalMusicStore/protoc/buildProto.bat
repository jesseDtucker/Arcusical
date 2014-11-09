setlocal
cls
cd %~dp0
rm -f -r impl
mkdir impl
..\..\..\protoc\protoc.exe --cpp_out=./impl/ ./*.proto
endlocal