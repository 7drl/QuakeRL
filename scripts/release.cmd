rmdir /s /q c:\temp\ggj13\
mkdir c:\temp\ggj13\

copy %SDL_HOME%\lib\x86\SDL.dll c:\temp\ggj13\
copy %OPENAL_HOME%\libs\Win32\*.dll c:\temp\ggj13\
copy %SDL_IMAGE_HOME%\lib\x86\libpng15-15.dll c:\temp\ggj13\
copy %SDL_IMAGE_HOME%\lib\x86\SDL_image.dll c:\temp\ggj13\
copy %SDL_IMAGE_HOME%\lib\x86\zlib1.dll c:\temp\ggj13\

xcopy /s ..\resources\*.* c:\temp\ggj13\

copy ..\project\vs2012\ggj13\Release\reapers.exe c:\temp\ggj13\ 

copy %WINDIR%\system32\msvcp110.dll c:\temp\ggj13\
copy %WINDIR%\system32\msvcr110.dll c:\temp\ggj13\

copy ..\install\*.exe c:\temp\ggj13\
copy ..\README c:\temp\ggj13\
copy ..\README c:\temp\ggj13\readme.txt
