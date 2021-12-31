# programs/meson.build

includes = include_directories('..', '../lib', '../include', '../libgnu')

executable('rglclock', 'rglclock.cpp', 'about.cpp', 'profile.cpp',
    'options.cpp', 'modaldialog.cpp',
    include_directories: includes,
    link_with: librglclock,
    dependencies: [gtk, glu, libxml],
    install: true)
