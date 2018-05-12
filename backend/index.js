try {
	module.exports = require('./build/Release/backend.node');
} catch (err) {
	module.exports = require('./build/Debug/backend.node');
}
