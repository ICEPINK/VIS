require("scripts.BuildDebug")

if not (BUILD_OUT == nil)
then
    print("[Debug.lua]")
    os.execute("lldb ./build/VIS")
end
