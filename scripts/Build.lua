require("scripts.Setup")

if not (SETUP_OUT == nil)
then
    print("[Build.lua]")
    BUILD_OUT = os.execute("cmake --build ./build")
end
