project('miniasync-painground', 'c',default_options:['warning_level=0'])

cmake = import('cmake')
sub_proj = cmake.subproject('miniasync')
miniasync_dep = sub_proj.dependency('miniasync')
thread_dep = dependency('threads')

subdir('ideas')
