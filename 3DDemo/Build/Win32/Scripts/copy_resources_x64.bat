cd ..
cd ..
cd ..
cd ..

xcopy "ThirdParty\freetype\win64\freetype.dll" "3DDemo\Build\Win32\x64\Debug" /Y
xcopy "ThirdParty\GLFW3\win64\glew32.dll" "3DDemo\Build\Win32\x64\Debug" /Y
xcopy "3DDemo\Assets" "3DDemo\Build\Win32\x64\Debug\Assets\" /Y /E /I