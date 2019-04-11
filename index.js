var elios_protocol = null;

// Load the precompiled binary for windows.
if(process.platform == "win32" && process.arch == "x64") {
	elios_protocol = require('./bin/winx64/elios_protocol');  
} else if(process.platform == "win32" && process.arch == "ia32") {
	elios_protocol = require('./bin/winx86/elios_protocol');  
} else {
	// Load the new built binary for other platforms.
	elios_protocol = require('./build/Release/elios_protocol');  
}

module.exports = elios_protocol;