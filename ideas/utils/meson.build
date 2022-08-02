utils_inc = include_directories('.')
utils_lib = library(
    'utils',
    include_directories: utils_inc,
    sources: ['timer.c'],
    )
utils_dep = declare_dependency(
    link_with : utils_lib,
    include_directories: utils_inc,
    dependencies: [thread_dep],
    )