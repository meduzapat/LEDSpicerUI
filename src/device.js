/**
 * Class to handle Devices code.
 */
let Device = (function () {

	let
		code,
		id,
		elements,
		options,
		devices = {};

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

	class Device {

	/**
	 * Creates a new device.
	 * @param string mewCode,
	 * @param number newId
	 * @param array newElements
	 * @param Object newOptions
	 */
	constructor(newCode, newId, newElements = [], newOptions = {}) {
		code     = newCode;
		id       = newId;
		elements = newElements;
		options  = newOptions;
		Device.registerUsed(code, id);
	}

	/**
	 * @returns the device code.
	 */
	getCode() {
		return code;
	}

	/**
	 * @returns the devices ID.
	 */
	getId() {
		return id;
	}

	/**
	 * Changes the code and id.
	 * @param string newCode
	 * @param number newId
	 * @returns Device
	 */
	change(newCode, newId) {
		Device.updateUsed(code, id, newCode, newId);
		code = newCode;
		id   = newId;
		return this;
	}

	/**
	 * @returns the device name.
	 */
	getName() {
		return defaultDevices[code].name;
	}

	/**
	 * @return the device elements.
	 */
	getElements() {
		return elements;
	}

	/**
	 * Populates the elements.
	 * @param array newElements
	 * @returns Device
	 */
	setElements(newElements) {
		elements = newElements;
		return this;
	}

	/**
	 * @returns the device special options.
	 */
	getOptions() {
		return options;
	}

	/**
	 * Sets the device extra options.
	 * @param Object newOptions
	 * @returns Device
	 */
	setOptions(newOptions) {
		options = newOptions;
		return this;
	}

	/**
	 * @param string code
	 * @returns the device name for code.
	 */
	static convertCode(code) {
		return defaultDevices[code].name;
	}

	/**
	 * @return the device default values.
	 */
	static getValues(code) {
		return defaultDevices[code];
	}

	/**
	 * @return the XML for device.
	 */
	toXML() {
		return "<device/>";
	}

	/**
	 * register a device.
	 */
	static registerUsed(code, id) {
		devices[code] ? devices[code].push(id) : devices[code] = [id];
	}

	static updateUsed(code, id, newCode, newId) {
		let oldIdIdx = devices[code].indexOf(id);
		if (code == newCode) {
			devices[code][oldIdIdx] = newId;
			return;
		}
		devices[code].splice(oldIndex, 1);
		Device.registerUsed(newCode, newId);
	}

	/**
	 * @returns true if the Id for a device is available.
	 */
	static isIdAvailable(code, id) {
		return (!devices[code] || (devices[code] && devices[code].indexOf(id) == -1));
	}

	/**
	 * @Returns true if a device code is not available.
	 */
	static isAvailable(code) {
		return (!devices[code] || (devices[code] && devices[code].length < defaultDevices[code]['maxDevices']));
	}

	/**
	 * @return an array with the devices codes and names.
	 */
	static getList() {
		return defaultDevices;
	}

	// close class
	}
	module.exports.Device = Device;

})();
