project('ThreadLink', 'cpp',
	version : '0.1',
	default_options : ['warning_level=3', 'cpp_std=c++2a']
)

add_project_arguments('-Wno-pedentic', language: 'cpp')
add_project_arguments('-Wno-missing-field-initializers', language: 'cpp')

compiler = meson.get_compiler('cpp')


ThreadLinkLib = static_library(
	'ThreadLink',
	sources: [
		'ServerThreadLink.cpp',
		'ClientThreadLink.cpp',
	],
	include_directories: [
		'/boot/system/develop/headers/private/shared',
		'/boot/system/develop/headers/private/app',
		'/boot/system/develop/headers/private/kernel/util',
	],
	dependencies: [
		compiler.find_library('be'),
	],
)

ThreadLink = declare_dependency(
	link_with : ThreadLinkLib,
	include_directories: [
		'.',
		'/boot/system/develop/headers/private/shared',
		'/boot/system/develop/headers/private/app',
		'/boot/system/develop/headers/private/kernel/util',
	],
)
