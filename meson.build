project('mortis-launcher', ['c', 'cpp'], 
    default_options : [
        'cpp_std=c++17',
        'b_sanitize=none',
        'b_lto=false',
        'b_pch=false',
        'buildtype=debug' 
    ]
)

# Get compiler object
cpp = meson.get_compiler('cpp')

# Base Windows Arguments
 windows_args = [
    '-DUNICODE',
    '-D_UNICODE',
    '-DWIN32',
    '-D_WINDOWS',
    '-DCOBJMACROS',
    '-D_CRT_SECURE_NO_WARNINGS'

] 

# Add DEBUG definition ONLY if we are actually in debug mode
if get_option('buildtype') == 'debug'
    windows_args += '-D_DEBUG'
endif

# Add linker arguments for static linking
link_args = [
    '-static',
    '-static-libgcc',
    '-static-libstdc++',
    '--enable-stdcall-fixup',
    '-Wl,-Bstatic',
]

# Source files
src_files = files(
    'src/main.cpp',
    'src/app/Application.cpp',
    'src/core/GameLauncher.cpp',
    'src/core/Window.cpp',
    'src/graphics/Renderer.cpp',
    'src/ui/UIManager.cpp',
    'src/ui/Theme.cpp',

    'include/imgui/imgui.cpp',
    'include/imgui/imgui_demo.cpp',
    'include/imgui/imgui_draw.cpp',
    'include/imgui/backends/imgui_impl_sdl2.cpp',
    'include/imgui/backends/imgui_impl_opengl3.cpp',
    'include/imgui/backends/imgui_impl_sdlrenderer2.cpp',
    'include/imgui/imgui_tables.cpp',
    'include/imgui/imgui_widgets.cpp',

    'include/glad/src/glad.c',
)

# Determine the architecture and set the paths accordingly
if host_machine.cpu_family() == 'x86_64'

    # Path to MingW
    message('Selecting MingW x86_64 architecture')
    mingw_path = 'C:/msys64/mingw64'

elif host_machine.cpu_family() == 'x86'

    # Path to MingW
    message('Selecting MingW x86 architecture')
    mingw_path = 'C:/msys64/mingw32'

endif

# Include directories
inc_dirs = [
    include_directories('src'),

    include_directories('headers'),
    include_directories('include'),
    include_directories('include/glad/include'),
    include_directories('include/imgui'),
    include_directories('include/imgui/backends'),
    include_directories(mingw_path + '/include/SDL2')
]

# Library paths
lib_path = mingw_path + '/lib'

# Define dependencies
sdl2_dep = cpp.find_library('sdl2', static : true, dirs : [lib_path])
opengl_dep = cpp.find_library('opengl32')
ole32_dep = cpp.find_library('ole32')
uuid_dep = cpp.find_library('uuid')
kernel32_dep = cpp.find_library('kernel32')
user32_dep = cpp.find_library('user32')
gdi32_dep = cpp.find_library('gdi32')
winspool_dep = cpp.find_library('winspool')
comdlg32_dep = cpp.find_library('comdlg32')
advapi32_dep = cpp.find_library('advapi32')
shell32_dep = cpp.find_library('shell32')
winmm_dep = cpp.find_library('winmm')
imm32_dep = cpp.find_library('imm32')
version_dep = cpp.find_library('version')
setupapi_dep = cpp.find_library('setupapi')
cfgmgr32_dep = cpp.find_library('cfgmgr32')
dwmapi_dep = cpp.find_library('dwmapi')

# Import the Windows module
windows = import('windows')

# Compile the resources
win_resources = windows.compile_resources('resources.rc')

# Decide which build we're doing
if get_option('buildtype') == 'debug'
    subsystem_type = 'console'
else
    subsystem_type = 'windows'
endif

# Build the EXE
executable('mortis-launcher',
    [src_files, win_resources],
    include_directories : inc_dirs,
    dependencies : [
        sdl2_dep,
        opengl_dep,
        ole32_dep,
        uuid_dep,
        kernel32_dep,
        user32_dep,
        gdi32_dep,
        winspool_dep,
        comdlg32_dep,
        advapi32_dep,
        shell32_dep,
        winmm_dep,
        imm32_dep,
        version_dep,
        setupapi_dep,
        cfgmgr32_dep,
        dwmapi_dep
    ],
    cpp_args : windows_args,
    cpp_pch : 'headers/pch.h',
    win_subsystem : subsystem_type,
    link_args : link_args,
    name_prefix : '',
)