const
	createOption = require('./generic').createOption,
	createItem   = require('./generic').createItem,

	updateGroups = require('./group').updateGroups,

	showMessage  = require('./errors').showMessage,

	setConfigurationDirty = require('./generic').setConfigurationDirty,

	devicesDialog = $('#devicesDialog'),
	devicesForm   = $('form', devicesDialog),
	deviceCode    = $('select[name="deviceCode"]', devicesDialog),
	deviceId      = $('select[name="deviceId"]', devicesDialog),
	deviceOptions = $('#deviceOptions', devicesDialog),
	saveDevice    = $('#saveDevice'),
	deleteDevice  = $('.modal-footer button:first-child', devicesDialog),
	elementList   = $('#elementList'),

	addElement    = $('#addElement'),

	//List of system available devices.
	defaultDevices = {
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
 * Generates the options section for devices.
 * @param array options
 * @returns string
 */
function createDeviceOptions(options) {
	let r = '<div class="row mb-1">';
	for (let c = 0; c < options.length;) {
		r += options[c];
		if (++c < options.length && (c - 1) % 2)
			r += '</div><div class="row">';
	}
	return (r + '</div>');
}

/**
 * Check if a device is already used.
 * @param string name
 * @param string id
 * @returns bool true if found.
 */
function isUsedDevice(name, id) {
	let found = false;
	$('li', devicesList).each(function() {
		let e = $(this).data('values');
		if (e.getCode() == name && e.getId() == id) {
			found = true;
			return false;
		}
	});
	return found;
}

/**
 * Return a list of used IDs for a device family name.
 * @param string name
 * @returns string[]
 */
function getUsedDeviceIds(name) {
	let r = [];
	$('li', devicesList).each(function() {
		let e = $(this).data('values');
		if (e.getCode() == name)
			r.push(e.getId());
	});
	return r;
}

/**
 * Activate validator for devices.
 */
devicesForm.validate({
	messages: {
		deviceCode : 'The device type is required',
		deviceId   : 'The device ID is required'
	},
	errorPlacement: require('./errors').errorPlacement
});

/**
 * Create and disable sortable for element list.
 */
sortable('#elementList', {
	placeholderClass    : 'elementBox element placeholder',
	forcePlaceholderSize: true,
	handle              : 'code',
	orientation         : 'horizontal'
});
sortable('#elementList', 'disable');

/**
 * Create device button opens the device dialog and resets its fields.
 */
devicesDialog.on('show.bs.modal', function(e) {

	// Keeps the clicked device button when editing.
	let button = $(e.relatedTarget);

	sortable('#elementList', 'enable');
	require('./errors').resetErrors(devicesDialog);
	require('./generic').prepareConfirmationDialog('deleteDevice');
	deviceOptions.hide();
	elementList.html('');
	devicesDialog.data('dirty', false);
	deviceCode.html(createOption('', 'Select Device'));
	for (const d in defaultDevices)
		if (getUsedDeviceIds(d).length < defaultDevices[d]['maxDevices'])
			deviceCode.append(createOption(d, defaultDevices[d]['name']));

	// Create.
	if (button.attr('id') == 'addDevice') {
		devicesDialog.data('current', null);
		deviceId.html('');
		deviceId.prop('disabled', true);
		addElement.prop('disabled', true);
		deleteDevice.hide();
		return;
	}

	// Edit.
	deviceId.prop('disabled', false);
	addElement.prop('disabled', false);
	devicesDialog.data('current', e.relatedTarget);

	deleteDevice.show();
	let
		data = button.data('values'),
		sel  = deviceCode.children('option[value="' + data.getCode() + '"]');

	// Populate Code.
	if (!sel.length)
		deviceCode.append(createOption(data.getCode(), data.getName(), true));
	else
		sel.prop('selected', true);

	deviceCode.change();

	// Populate Id.
	let
		opts = deviceId.children("option"),
		opt  = createOption(data.getId(), data.getId(), true);

	if (deviceId.children("option").length == parseInt(data.getId())) {
		deviceId.append(opt);
	}
	else {
		deviceId.children("option").filter(function() {
			return (this.value > data.getId());
		}).first().before(opt);
	}

	// Populate Elements.
	let elementData = data.getElements();
	for (let i in elementData) {
		let e = createItem(elementData[i].getName(), 'element', 'elementDialog');
		$(e).data('values', elementData[i]);
		elementList.append(e);
	}
	sortable('#elementList');

	// Populate Options.
	if (!$.isEmptyObject(defaultDevices[data.getCode()]['options']))
		for (let o in defaultDevices[data.getCode()]['options'])
			$('input[name="' + o + '"]', deviceOptions).val(data.getOptions()[o]);
});

/**
 * Device type selector activates the other elements and provides max pins to elements dialog.
 */
deviceCode.on('change', function(e) {
	deviceId.html(createOption('', 'Select Id'));
	let code = deviceCode.val();
	for (let c = 1; c <= defaultDevices[code]['maxDevices']; ++c)
		if (!isUsedDevice(code, c))
			deviceId.append(createOption(c, c));
	devicesDialog.data('dirty', true);
	deviceId.prop('disabled', false);
	addElement.prop('disabled', false);
	require('./element').setPinMaxValue(defaultDevices[code]['pins']);

	if (!$.isEmptyObject(defaultDevices[code]['options']))
		deviceOptions.html('<legend>Extra Options</legend>' + createDeviceOptions(Object.values(defaultDevices[code]['options']))).show();
	else
		deviceOptions.hide();
});

/**
 * Device Id change.
 */
deviceId.on('change', () => {
	devicesDialog.data('dirty', true);
});

saveDevice.on('click', function() {

	if (!devicesDialog.data('dirty') || !devicesForm.valid())
		return;

	let
		elements   = [],
		deviceNode = devicesDialog.data('current'),
		device     = deviceNode ? $(deviceNode).data('values') : null,
		devText    = deviceCode.children("option:selected").html() + ' [' + deviceId.val() + ']',
		options    = {};

	sortable('#elementList', 'disable');

	// Read Elements.
	$('li', elementList).each(function() {
		elements.push($(this).data('values'));
	});

	// Read Options.
	$('input', deviceOptions).each(function() {
		options[this.name] = this.value;
	});

	setConfigurationDirty(true);
	devicesDialog.data('dirty', false);
	devicesDialog.modal('hide');

	// Create.
	if (!device) {
		device = Device(
			deviceCode.val(),
			deviceId.val(),
			elements,
			options
		);

		deviceNode = createItem(devText, 'device', 'devicesDialog');
		deviceNode.data('values', device);
		devicesList.append(deviceNode[0]);
		showMessage('success', 'Device '+ device.getName() +' with id ' + device.getId() +' Created');
		updateGroups();
		return;
	}

	deviceNode = $(deviceNode);
	// Update.
	device
		.change(deviceCode.val(), deviceId.val())
		.setElements(elements)
		.setOptions(options);
	deviceNode.data('values', device);
	$('span', deviceNode).html(devText);
	showMessage('success', 'Device Updated');
	updateGroups();
});

/**
 * Delete Device callback.
 */
$('#deleteConfirmation').on('click', '#deleteDevice', function() {
	// DeviceDialog holds the element, the element holds the data.
	let
		current = $(devicesDialog.data('current')),
		data    = current.data('values');

	current.remove();
	showMessage('success', 'Device Deleted');
	devicesDialog.modal('hide');
	updateGroups();
	setConfigurationDirty(true)
});

devicesDialog.on('hide.bs.modal', function(e) {
	devicesDialog.data('current', null);
});

/**
 * Class to handle Devices code.
 *
 * @param string mewCode,
 * @param number newId
 * @param array newElements
 * @param Object newOptions
 */
function Device(code, id, elements = [], options = {}) {

	let
		_code     = code,
		_id       = id,
		_elements = elements,
		_options  = options;

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
			_code = newCode;
			_id   = newId;
			return this;
		},

		/**
		 * @returns the device name.
		 */
		getName() {
			return defaultDevices[_code].name;
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

module.exports = Device;
module.exports.isUsedDevice = isUsedDevice;


