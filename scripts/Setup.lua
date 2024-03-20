print("[Setup.lua]")

local cmake =
{
    "cmake",
    "-G \"Ninja\"",
    "-S ./",
    "-B ./build",
    "-DCMAKE_BUILD_TYPE=Release",
    "-DCMAKE_C_COMPILER=clang",
    "-DCMAKE_CXX_COMPILER=clang++"
}

local function merge_lines(string_array)
    string = ""

    ---@diagnostic disable-next-line: unused-local
    for i, value in ipairs(string_array) do
        string = string .. value .. " "
    end

    return string;
end

SETUP_OUT = os.execute(merge_lines(cmake))
