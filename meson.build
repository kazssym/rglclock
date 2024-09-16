# meson.build

project('rglclock', 'c', 'cpp',
    version: '4.0',
    default_options: ['cpp_std=c++17'])

gtk = dependency('gtk+-3.0')
glu = dependency('glu')
libxml = dependency('libxml-2.0')
libpng = dependency('libpng')

config = configuration_data()

config.set_quoted('PACKAGE_NAME', 'RGLClock')
config.set_quoted('PACKAGE_VERSION', meson.project_version())
config.set_quoted('PACKAGE_BUGREPORT', 'https://github.com/kazssym/rglclock/issues/new')
config.set_quoted('PACKAGE_TARNAME', meson.project_name())

config.set('ENABLE_NLS', 1)

if meson.get_compiler('c').check_header('unistd.h')
    config.set('HAVE_UNISTD_H', 1,
        description: 'Define to 1 if you have the <unistd.h> header file.')
endif

if meson.get_compiler('c').check_header('sys/time.h')
    config.set('HAVE_SYS_TIME_H', 1,
        description: 'Define to 1 if you have the <sys/time.h> header file.')
endif

configure_file(
    configuration: config,
    output: 'config.h')

add_project_arguments('-DHAVE_CONFIG_H',
    language: ['c', 'cpp'])

subdir('lib')
subdir('programs')
subdir('po')
