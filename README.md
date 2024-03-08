# SE - PRACTICA 2

Comandos:

make: compilará y creara los ficheros necesarios para led_blinky.c y hellow_world.c
make build_led: solo compilara y creara los ficheros necesarios para led_blinky.c
make build_hello: Solo compilara y creara los ficheros para hellow_world.c

make clean: hara un clean general de los ficheros creados durante el make, independientemente de si esten asociados a led_blinky o a hellow_world.
make clean_led: solo eliminara los archivos asociados a el led_blinky (Cuidado: Tambien elimina los creados en la carpeta drivers)
make clean_hello: solo eliminara los archivos asociados a el hello_world (Cuidado: Tambien elimina los creados en la carpeta drivers)

flash led: hara un flash del led_blinky
flash hello: hara un flash del hello_world
