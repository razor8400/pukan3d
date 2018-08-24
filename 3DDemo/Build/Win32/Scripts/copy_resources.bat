cd ..
cd ..
cd ..
cd ..

xcopy "ThirdParty\freetype\win32\freetype.dll" "3DDemo\Build\Win32\Debug" /Y
xcopy "ThirdParty\GLFW3\win32\glew32.dll" "3DDemo\Build\Win32\Debug" /Y
xcopy "3Ddemo\Assets" "3DDemo\Build\Win32\Debug\Assets\" /Y /E /I