setlocal
cls
set PATH=%PATH%;D:\Development\P4\citadel\protoc
cd %~dp0
rm -f -r impl
mkdir impl
protoc --cpp_out=./impl/ ./*.proto
endlocal