!editor {
	CONFIG -= qt
} else {
	CONFIG += qt
}

win32 {
	LIBS += -L../seed/contrib/windows/ -lseed -lseedcontrib -mwindows -lmingw32 -lopengl32 -lopenal32
	INCLUDEPATH += ../seed/contrib/windows/
	CONFIG -= glfw
	CONFIG += sdl
	sdl {
		DEFINES += WIN32 main=SDL_main
		LIBS += -lSDLmain -lSDL -lSDL_image -lgdi32
	}
}

macx {
	!editor:!glfw {
		message("Seed for OSX must use GLFW, disabling SDL.")
		CONFIG -= sdl
		CONFIG += glfw
	}
	INCLUDEPATH += ../seed/contrib/osx/
	LIBS += -lseed -lseedcontrib -framework OpenAL -framework OpenGL -framework Cocoa -framework IOKit
}

unix:!macx {
	DEFINES += LINUX
	CONFIG += sdl

	LIBS += -lseed -lseedcontrib -lGL -lopenal
	QMAKE_CXXFLAGS += -std=c++0x

	sdl {
		LIBS += -lSDL -lSDL_image
	}
}

qt {
	DEFINES += BUILD_QT
	QT += opengl
} else:glfw {
	DEFINES += BUILD_GLFW
} else:sdl {
	DEFINES += BUILD_SDL
}

CONFIG(debug, debug|release) {
	DEFINES += DEBUG
} else {
	DEFINES += RELEASE
}
