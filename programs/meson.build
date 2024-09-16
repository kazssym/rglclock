# programs/meson.build

localedir = get_option('prefix') / get_option('localedir')

includes = include_directories('..', '../lib', '../libgnu')

executable('rglclock', 'rglclock.cpp', 'about.cpp', 'profile.cpp',
    'options.cpp', 'modaldialog.cpp',
    c_args: ['-DLOCALEDIR="' + localedir + '"'],
    cpp_args: ['-DLOCALEDIR="' + localedir + '"'],
    include_directories: includes,
    link_with: [librglclock],
    dependencies: [gtk, glu, libxml],
    install: true)
