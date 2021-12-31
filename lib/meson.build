# lib/meson.build

includes = include_directories('..', '../include', '../libgnu')

librglclock = library('rglclock', 'movement.cpp', 'glgdkx.cpp',
    '../modules/simple.c',
    include_directories: includes,
    dependencies: [gtk, x11, glu, libpng],
    version: '1.0.0',
    install: true)
