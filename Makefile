all:
	gcc particles.c -L./Linux -lturtle -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -DOS_LINUX -DDEBUGGING_FLAG -Wall -o particles.o
rel:
	gcc particles.c -L./Linux -lturtle -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -DOS_LINUX -O3 -o particles.o
textures:
	gcc particles.c -L./Linux -lturtletextures -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -DOS_LINUX -DDEBUGGING_FLAG -Wall -o particles.o
reltextures:
	gcc particles.c -L./Linux -lturtletextures -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -DOS_LINUX -O3 -o particles.o
lib:
	cp turtle.h turtlelib.c
	gcc turtlelib.c -c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -DTURTLE_IMPLEMENTATION -DOS_LINUX -O3 -o Linux/libturtle.a
	gcc turtlelib.c -c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -DTURTLE_IMPLEMENTATION -DTURTLE_ENABLE_TEXTURES -DOS_LINUX -O3 -o Linux/libturtletextures.a
	rm turtlelib.c
win:
	gcc particles.c -L./Windows -lturtle -lglfw3 -lopengl32 -lgdi32 -lglad -lole32 -luuid -lwsock32 -lWs2_32 -DOS_WINDOWS -DDEBUGGING_FLAG -Wall -o particles.exe
winrel:
	gcc particles.c -L./Windows -lturtle -lglfw3 -lopengl32 -lgdi32 -lglad -lole32 -luuid -lwsock32 -lWs2_32 -DOS_WINDOWS -O3 -o particles.exe
wintextures:
	gcc particles.c -L./Windows -lturtletextures -lglfw3 -lopengl32 -lgdi32 -lglad -lole32 -luuid -lwsock32 -lWs2_32 -DOS_WINDOWS -DDEBUGGING_FLAG -Wall -o particles.exe
winreltextures:
	gcc particles.c -L./Windows -lturtletextures -lglfw3 -lopengl32 -lgdi32 -lglad -lole32 -luuid -lwsock32 -lWs2_32 -DOS_WINDOWS -O3 -o particles.exe
winlib:
	cp turtle.h turtlelib.c
	gcc turtlelib.c -c -L./Windows -lglfw3 -lopengl32 -lgdi32 -lglad -lole32 -luuid -lwsock32 -lWs2_32 -DTURTLE_IMPLEMENTATION -DOS_WINDOWS -O3 -o Windows/turtle.lib
	gcc turtlelib.c -c -L./Windows -lglfw3 -lopengl32 -lgdi32 -lglad -lole32 -luuid -lwsock32 -lWs2_32 -DTURTLE_IMPLEMENTATION -DTURTLE_ENABLE_TEXTURES -DOS_WINDOWS -O3 -o Windows/turtletextures.lib
	rm turtlelib.c