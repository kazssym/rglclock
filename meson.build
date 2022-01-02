# meson.build

project('rglclock', 'c', 'cpp',
    version: '4.0',
    default_options: ['cpp_std=c++17'])

gtk = dependency('gtk+-3.0')
x11 = dependency('x11')
glu = dependency('glu')
libxml = dependency('libxml-2.0')
libpng = dependency('libpng')

config = configuration_data()

config.set_quoted('PACKAGE_NAME', 'RGLClock')
config.set_quoted('PACKAGE_VERSION', meson.project_version())
config.set_quoted('PACKAGE_BUGREPORT', 'https://github.com/kazssym/rglclock/issues/new')
config.set_quoted('PACKAGE_TARNAME', meson.project_name())

config.set('ENABLE_NLS', 1)

configure_file(
    configuration: config,
    output: 'config.h')

add_project_arguments('-DHAVE_CONFIG_H',
    language: ['c', 'cpp'])

subdir('lib')
subdir('programs')
subdir('po')
