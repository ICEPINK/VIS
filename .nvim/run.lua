-- .nvim/run.lua --

-- global variables --
local g_executable = 'VIS'
local g_executable_args = {}
local g_setup = 'cmake'
local g_setup_args = {} 
local g_setup_build_dir = 'build/' 
local g_setup_build_type = 'Release'
local g_setup_c_compiler = 'clang'
local g_setup_cxx_compiler = 'clang++'
local g_setup_generator = 'Ninja' 
local g_setup_source_dir = './' 

-- fuctions --
function in_args(a)
    for i,v in ipairs(arg) do
        if v == a then return true end
    end
    return false
end
local function setup()
    print('@setup(' .. g_setup .. ')')
    return os.execute(g_setup
    .. ' -G "' .. g_setup_generator .. '"'
    .. ' -S ' .. g_setup_source_dir
    .. ' -B ' .. g_setup_build_dir
    .. ' -DCMAKE_BUILD_TYPE=' .. g_setup_build_type
    .. ' -DCMAKE_CXX_COMPILER=' .. g_setup_cxx_compiler
    .. ' -DCMAKE_C_COMPILER=' .. g_setup_c_compiler
    .. ' ' .. table.concat(g_setup_args, ' ')
    )
end
local function build()
    print('@build(' .. g_setup .. ')' )
    return os.execute(g_setup .. ' --build ' .. g_setup_build_dir)
end
local function run()
    print('@run(' .. g_executable .. ' ' .. table.concat(g_executable_args, ' ') ..  ')')
    -- return os.execute('start cmd /C ' .. g_setup_build_dir .. g_executable .. ' ' .. table.concat(g_executable_args, ' '))
    return os.execute(g_setup_build_dir .. g_executable .. ' ' .. table.concat(g_executable_args, ' '))
end

-- script --
print('')
if in_args('--debug') then g_setup_build_type = 'Debug' end
if in_args('--run') then if not run() then print('Exit with ERROR!') return end return end
if not setup() or in_args('--setup') then return end
if not build() or in_args('--build') then return end
if not run() then print('Exit with ERROR!') end
