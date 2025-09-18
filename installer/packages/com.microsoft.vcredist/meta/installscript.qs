function Component()
{
}

function isVCRedistInstalled()
{
    try {
        // Registry check for VC++ 2015–2022 x64 runtime
        // Value "Installed" = 1 if present
        var value = installer.value("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\x64", "Installed");
        if (value === "1" || value === 1) {
            return true;
        }
    } catch (e) {
        // Key not found or access denied
    }
    return false;
}

Component.prototype.install = function()
{
    if (!isVCRedistInstalled()) {
        var vcredistPath = installer.componentByName("vcredist").installScriptDir + "/data/vcredist_x64.exe";
        console.log("Installing Visual C++ Redistributable from " + vcredistPath);
        
        // Run silently (/quiet /norestart). This will trigger UAC if needed.
        var result = installer.execute(vcredistPath, ["/quiet", "/norestart"]);
        if (result !== 0) {
            console.log("vcredist_x64.exe exited with code " + result);
        }
    } else {
        console.log("Visual C++ Redistributable already installed — skipping.");
    }
}
