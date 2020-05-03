/**
 * Class to handle the configuration file.
 */
module.exports = function() {

	let
		fps            = 30,
		port           = 16161,
		colors         = 'basicColors',
		logLevel       = 'Info',
		userId         = 1000,
		groupId        = 1000,
		defaultProfile = 'default',
		devices        = [];
		groups         = [];

	return Object.freeze({

		/**
		 * @returns number.
		 */
		getFps() {
			return fps;
		},

		/**
		 * @returns number.
		 */
		getPort() {
			return port;
		},

		/**
		 * @returns string.
		 */
		getColors() {
			return colors;
		},

		/**
		 * @return string.
		 */
		getLogLevel() {
			return logLevel;
		},

		/**
		 * @returns number.
		 */
		getUserId() {
			return userId;
		},

		/**
		 * @returns number.
		 */
		getGroupId() {
			return groupId;
		},

		/**
		 * @return string.
		 */
		getDefaultProfile() {
			return defaultProfile;
		},

		/**
		 * @returns Device[]
		 */
		getDevices() {
			return devices;
		},

		/**
		 * @returns groups[]
		 */
		getGroups() {
			return groups;
		},

		/**
		 * @returns number.
		 */
		setFps(newFps) {
			fps = newFps;
			return this;
		},

		/**
		 * @returns number.
		 */
		setPort(newPort) {
			port = newPort;
			return this;
		},

		/**
		 * @returns string.
		 */
		setColors(newColors) {
			colors = newColors;
			return this;
		},

		/**
		 * @return string.
		 */
		setLogLevel(newLogLevel) {
			logLevel = newLogLevel;
			return this;
		},

		/**
		 * @returns number.
		 */
		setUserId(newUserId) {
			userId = newUserId;
			return this;
		},

		/**
		 * @returns number.
		 */
		setGroupId(newGroupId) {
			groupId = newGroupId;
			return this;
		},

		/**
		 * @return string.
		 */
		setDefaultProfile(newDefaultProfile) {
			defaultProfile = newDefaultProfile;
			return this;
		},

		/**
		 * @returns Device[]
		 */
		setDevices(newDevices) {
			devices = newDevices;
			return this;
		},

		/**
		 * @returns groups[]
		 */
		setGroups(newGroups) {
			groups = newGroups;
			return this;
		},

		/**
		 * @returns string the object converted into XML.
		 */
		toXML() {
			let r = '<?xml version="1.0" encoding="UTF-8"?>\n<LEDSpicer version="1.0" type="Configuration" fps="' + fps + '" port="' + port + '" colors="' + colors + '" logLevel="' + logLevel + '" userId="' + userId + '" groupId="' + groupId + '">\n\t<devices>\n';
			devices.forEach(d => r += d.toXML());
			r += '\t</devices>\n\t<layout defaultProfile="' + defaultProfile + '">\n';
			groups.forEach(g => r += g.toXML());
			r += '\t</layout>\n</LEDSpicer>';
			return r;
		},
	});
};