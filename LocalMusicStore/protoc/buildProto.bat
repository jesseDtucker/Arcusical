setlocal
cls
cd %~dp0
rm -f -r impl
mkdir impl
FOR %%p in (*.proto) Do (
  ..\..\bin\protoc\protoc.exe --cpp_out=./impl/ %%p
)
endlocal