-- .nvim/run.lua ---------------------------------------------------------------
-- Globals ---------------------------------------------------------------------
local g_build_type = 'Release'
local g_executable = 'start cmd /C .\\build\\VIS.exe'
local g_executable_args = {}
local g_cmake_cxx_compiler = 'clang++'
local g_cmake_c_compiler = 'clang'
-- FncDefinitions --------------------------------------------------------------
function in_args(a)
    for i,v in ipairs(arg) do
        if v == a then 
            return true
        end
    end
    return false
end
function to_cmd(arr)
    local result_cmd = ''
    for i,v in ipairs(arr) do
        result_cmd = result_cmd .. v .. ' '
    end
    return result_cmd
end
function setup()
    print('@setup(cmake)')
    setup_cmd = {
        'cmake',
        '-G "Ninja"',
        '-S ./',
        '-B ./build/',
        '-DCMAKE_BUILD_TYPE=' .. g_build_type,
        '-DCMAKE_CXX_COMPILER=' .. g_cmake_cxx_compiler,
        '-DCMAKE_C_COMPILER=' .. g_cmake_c_compiler,
    }
    return os.execute(to_cmd(setup_cmd))
end
function build()
    print('@build(cmake)')
    build_cmd = {
        'cmake',
        '--build ./build/',
    }
    return os.execute(to_cmd(build_cmd))
end
function run()
    print('@run(' .. g_executable .. ' ' .. to_cmd(g_executable_args) .. ')')
    return os.execute(g_executable .. ' ' .. to_cmd(g_executable_args))
end
-- Handle Args -----------------------------------------------------------------
print('')
if in_args('--release') then
    g_build_type = 'Release'
end
if in_args('--debug') then
    g_build_type = 'Debug'
end
if in_args('--run-only') then
    if not run() then return end
    return
end
if in_args('--run') then
    if not setup() then return end
    if not build() then return end
    if not run() then return end
    return
end
if in_args('--build') then
    if not setup() then return end
    if not build() then return end
    return
end
if in_args('--setup') then
    if not setup() then return end
    return
end
if not setup() then return end
if not build() then return end
if not run() then return end
-- End -------------------------------------------------------------------------
