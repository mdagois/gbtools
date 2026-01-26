pushd "%~dp0"
ctags -f tags --recurse=yes --exclude=build --languages=C --languages=C++ --extras=+f .

