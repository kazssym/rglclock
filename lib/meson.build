# lib/meson.build

includes = include_directories('..', '../libgnu')

librglclock = library('rglclock', 'movement.cpp', 'mat4.c',
    '../modules/simple.c',
    include_directories: includes,
    dependencies: [gtk, glu, libpng],
    version: '1.0.0',
    install: true)
