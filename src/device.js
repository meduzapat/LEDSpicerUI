/**
 * Storage static singleton class.
 */
const DeviceStorage = (function() {

	/**
	 * used devices in the form of:
	 * name:[ids]
	 */
	let storage = {};

	const defaultDevices = {
		UltimarcUltimate : {
			name: 'Ultimarc Ultimate I/O',
			pins: 96,
			pwm: true,
			maxDevices: 4,
			options: {}
		},
		UltimarcPacLed64 : {
			name: 'Ultimarc PacDrive64',
			pins: 64,
			pwm: true,
			maxDevices: 4,
			options: {}
		},
		UltimarcNanoLed : {
			name: 'Ultimarc NanoLed',
			pins: 60,
			pwm: true,
			maxDevices: 4,
			options: {}
		},
		UltimarcPacDrive : {
			name: 'Ultimarc Pac Drive',
			pins: 60,
			pwm: false,
			maxDevices: 4,
			options: {
				changePoint: '<div class="input-group col-6"><label for="changePoint" class="input-group-text col-6">Change Point</label><input type="range" name="changePoint" min="1" max="254" class="form-control-range col-6"></div>'
			}
		},
		LedWiz32 : {
			name: 'Led-Wiz',
			pins: 32,
			pwm: true,
			maxDevices: 4,
			options: {}
		},
		Howler : {
			name: 'WolfWare Tech Howder',
			pins: 96,
			pwm: true,
			maxDevices: 4,
			options: {}
		},
		RaspberryPi : {
			name: 'RaspberryPi GPIO',
			pins: 28,
			pwm: true,
			maxDevices: 1,
			options: {}
		}
	};

	/**
	 * Register a device.
	 * @param string code
	 * @param number id
	 */
	function register(code, id) {
		storage[code] ? storage[code].push(id) : storage[code] = [id];
	}

	return Object.freeze({

		register: register,

		defaultDevices: defaultDevices,

		/**
		 * @param string code
		 * @returns the device name for code.
		 */
		getName(code){
			return defaultDevices[code].name;
		},

		/**
		 * @return the device default Id values.
		 */
		getIdValues(code) {
			return defaultDevices[code];
		},

		/**
		 * Replaces a registered devices with a different one.
		 * @param string code of device to replace
		 * @param number id of  device to replace
		 * @param string newCode new code
		 * @param number id new id
		 */
		update(code, id, newCode, newId) {
			let oldIdIdx = storage[code].indexOf(id);
			if (code == newCode) {
				storage[code][oldIdIdx] = newId;
				return;
			}
			storage[code].splice(oldIdIdx, 1);
			register(newCode, newId);
		},

		/**
		 * Removes a device from the registered devices.
		 * @param string code
		 * @param number id
		 */
		remove(code, id) {
			storage[code].splice(storage[code].indexOf(id), 1)
		},

		/**
		 * @returns true if the Id for a device is available.
		 */
		isIdAvailable(code, id){
			return (!storage[code] || (storage[code] && storage[code].indexOf(id) == -1));
		},

		/**
		 * @Returns true if a device code is not available.
		 */
		isAvailable(code) {
			return (!storage[code] || (storage[code] && storage[code].length < defaultDevices[code]['maxDevices']));
		},

		reset() {
			storage = {};
		},

		/**
		 * @return an array with the devices codes and names.
		 */
		getList() {
			return defaultDevices;
		}
	});

})();

/**
 * Class to handle Devices code.
 *
 * @param string mewCode,
 * @param number newId
 * @param array newElements
 * @param Object newOptions
 */
module.exports = function(code, id, elements = [], options = {}) {

		let _code     = code;
		let _id       = id;
		let _elements = elements;
		let _options  = options;

	DeviceStorage.register(code, id);

	return Object.freeze({

		/**
		 * @returns the device code.
		 */
		getCode() {
			return _code;
		},

		/**
		 * @returns the devices ID.
		 */
		getId() {
			return _id;
		},

		/**
		 * Changes the code and id.
		 * @param string newCode
		 * @param number newId
		 * @returns Device
		 */
		change(newCode, newId) {
			DeviceStorage.update(_code, _id, newCode, newId);
			_code = newCode;
			_id   = newId;
			return this;
		},

		/**
		 * @returns the device name.
		 */
		getName() {
			return DeviceStorage.defaultDevices[_code].name;
		},

		/**
		 * @return the device elements.
		 */
		getElements() {
			return _elements;
		},

		/**
		 * Populates the elements.
		 * @param array newElements
		 * @returns Device
		 */
		setElements(newElements) {
			_elements = newElements;
			return this;
		},

		/**
		 * @return string[]
		 */
		getElementsName() {
			let r = [];
			_elements.forEach(e => r.push(e.getName()));
			return r;
		},

		/**
		 * @returns the device special options.
		 */
		getOptions() {
			return _options;
		},

		/**
		 * Sets the device extra options.
		 * @param Object newOptions
		 * @returns Device
		 */
		setOptions(newOptions) {
			_options = newOptions;
			return this;
		},

		remove() {
			DeviceStorage.remove(_code, _id);
		},

		/**
		 * @returns string the object converted into XML.
		 */
		toXML() {
			let r = '\t\t<device name="' + _code + '" boardId="' + _id + '"';
			for (let o in _options)
				r += ' ' + o + '="' + _options[o] + '"';
			r += '>\n';
			_elements.forEach(e => r += e.toXML());
			r += '\t\t</device>\n';
			return r;
		},
	});
};

module.exports.getList       = DeviceStorage.getList;
module.exports.isAvailable   = DeviceStorage.isAvailable;
module.exports.isIdAvailable = DeviceStorage.isIdAvailable;
module.exports.reset         = DeviceStorage.reset;

