const

	remote = require('electron').remote,
	dialog = remote.dialog,
	fs     = require('fs'),

	resetErrors = require('./errors').resetErrors,
	showMessage = require('./errors').showMessage,

	Element    = require('./element'),
	Device     = require('./device'),
	Group      = require('./group'),
	createItem = require('./generic').createItem,

	setConfigurationDirty = require('./generic').setConfigurationDirty,

	currentFile    = $('#currentFile'),
	devicesList    = $('#devicesList'),
	groupsList     = $('#groupsList'),
	profilesList   = $('#profilesList'),
	animationsList = $('#animationsList'),
	inputsList     = $('#inputsList'),
	configuration  = $('#configuration'),

	configurationForm  = $('form', configuration),
	resetConfiguration = $('#resetConfiguration'),
	loadConfiguration  = $('#loadConfiguration'),
	saveConfiguration  = $('#saveConfiguration'),

	fps            = $('input[name="fps"]', configuration),
	port           = $('input[name="port"]', configuration),
	userId         = $('input[name="userId"]', configuration),
	groupId        = $('input[name="groupId"]', configuration),
	colors         = $('select[name="colors"]', configuration),
	logLevel       = $('select[name="logLevel"]', configuration),
	defaultProfile = $('select[name="defaultProfile"]', configuration);

let file = null;

/**
 * Activate validator for configuration.
 */
configurationForm.validate({
	errorPlacement: require('./errors').errorPlacement
});

/**
 * Write configuration data.
 */
function saveConfigurationToFile() {
	if (!configurationForm.valid())
		return false;

	if ($('li', devicesList).length == 0) {
		showMessage('error', 'You need at least one Device');
		return false;
	}

	let
		d = () => {
			let d = [];
			$('li', devicesList).each(function() {
				d.push($(this).data('values'));
			})
			return d;
		},
		g = () => {
			let g = [];
			$('li', groupsList).each(function() {
				g.push($(this).data('values'));
			})
			return g;
		};

	let currentPath = file ? file : require('os').homedir() + '/ledspicer.conf';
	let r = dialog.showSaveDialogSync(remote.getCurrentWindow(), {
		title: 'Save File',
		message: 'Choise the destination',
		filters: [{
			name      : 'LEDSpicer Configuration file',
			extensions: ['conf', 'xml']
		}],
		defaultPath: currentPath,
		properties: ['showOverwriteConfirmation']
	});

	if (r) {
		currentPath = r;
		try {
			fs.writeFileSync(
				r,
				Configuration()
					.setFps(fps.val())
					.setPort(port.val())
					.setColors(colors.val())
					.setLogLevel(logLevel.val())
					.setUserId(userId.val())
					.setGroupId(groupId.val())
					.setDefaultProfile(defaultProfile.val())
					.setDevices(d())
					.setGroups(g())
					.toXML()
			);
			showMessage('success', 'Configuration file saved');
			setConfigurationDirty(false);
			file = currentPath;
			currentFile.text(file).attr('title', file);
			return true;
		}
		catch(e) {
			showMessage('error', e.message);
		}
	}
	return false;
}

function handleNotSaved() {

	let r = dialog.showMessageBoxSync(remote.getCurrentWindow(), {
		type: 'question',
		buttons: ['Continue without Saving', 'Cancel', 'Save'],
		defaultId: 2,
		title: 'Unsaved Changes',
		message: 'Do you want to save?',
		detail: 'Unsaved Changes will be lost'
	});

	switch (r) {
	case 1:
		return false;
	case 2:
		return saveConfigurationToFile();
	}
	return true;
}

/**
 * Set the form dirty.
 */
$('input, select', configurationForm).on('change', function() {
	setConfigurationDirty(true);
});

/**
 * Resets the configuration form.
 */
resetConfiguration.on('click', function(e) {
	e.preventDefault();
	if (configurationForm.data('dirty'))
		if (!handleNotSaved())
			return;

	configurationForm[0].reset();
	devicesList.html('');
	currentFile.text('none').attr('title', 'none');
	resetErrors(configurationForm);
	Group.updateGroups();
	setConfigurationDirty(false);
});

loadConfiguration.on('click', function(e) {
	e.preventDefault();
	if (configurationForm.data('dirty'))
		if (!handleNotSaved())
			return;

	file = dialog.showOpenDialogSync(remote.getCurrentWindow(), {
		title  : 'Open Configuration file',
		filters: [{
			name      : 'LEDSpicer Configuration file',
			extensions: ['conf', 'xml']
		}],
		properties: ['openFile']
	});

	if (file === undefined) return false;
	file = file[0];
	fs.readFile(file, 'utf8', function(err, data) {
		if (err) return showMessage('error', err);
		try {
			data = $.parseXML(data).firstElementChild;
		} catch (e) {
			return showMessage('error', 'Invalid or Corrupted file: ' + e.message);
		}
		if (data.nodeName != 'LEDSpicer') return showMessage('error', 'Invalid file type');
		if (data.getAttribute('version') != '1.0') return showMessage('error', 'Invalid version');
		if (data.getAttribute('type') != 'Configuration') return showMessage('error', 'Sould be a configuration file');

		currentFile.text(file).attr('title', file);
		fps.val(data.getAttribute('fps'));
		port.val(data.getAttribute('port'));
		colors.val(data.getAttribute('colors'));
		logLevel.val(data.getAttribute('logLevel'));
		userId.val(data.getAttribute('userId'));
		groupId.val(data.getAttribute('groupId'));
		devicesList.html('');
		setConfigurationDirty(false);
		resetErrors(configurationForm);

		// Extract Devices.
		let
			checkName = [],
			targetNode = data.getElementsByTagName("devices")[0];
		for (let device = targetNode.firstChild; device !== null; device = device.nextSibling) {

			if (device.nodeName != 'device')
				continue;

			if (Device.isUsedDevice(device.getAttribute('name'), device.getAttribute('boardId'))) {
				showMessage('error', 'Device ' + device.getAttribute('name') + ' [' + device.getAttribute('boardId') + '] is already used');
				continue;
			}

			// Extract Elements.
			let
				elements  = [],
				checkPins = [];
			for (let element = device.firstChild; element !== null; element = element.nextSibling) {

				if (element.nodeName != 'element')
					continue;

				if (checkName.indexOf(element.getAttribute('name')) != -1) {
					showMessage('error', 'Duplicated element ' + element.getAttribute('name'));
					continue;
				}
				checkName.push(element.getAttribute('name'));
				let elementData = Element(element.getAttribute('name'));
				if (element.getAttribute('led')) {
					if (checkPins.indexOf(element.getAttribute('led')) != -1) {
						showMessage('error', 'Duplicated Pin number ' + element.getAttribute('led'));
						continue;
					}
					checkPins.push(element.getAttribute('led'));
					elementData.setLed(element.getAttribute('led'));
				}
				else {
					if (
						checkPins.indexOf(element.getAttribute('red'))   != -1 ||
						checkPins.indexOf(element.getAttribute('green')) != -1 ||
						checkPins.indexOf(element.getAttribute('blue'))  != -1
					) {
						showMessage('error', 'Duplicated Pin number on element ' + element.getAttribute('name'));
						continue;
					}
					checkPins.push(element.getAttribute('red'));
					checkPins.push(element.getAttribute('green'));
					checkPins.push(element.getAttribute('blue'));
					elementData.setPins([element.getAttribute('red'), element.getAttribute('green'), element.getAttribute('blue')]);
				}
				elements.push(elementData);
			}
			let deviceObj = Device(device.getAttribute('name'), device.getAttribute('boardId'), elements);
			devicesList.append(createItem(deviceObj.getName() + ' [' + deviceObj.getId() + ']', 'device', 'devicesDialog').data('values', deviceObj));
		}

		// Extract Layout data.
		targetNode = data.getElementsByTagName("layout")[0];
		defaultProfile.val(targetNode.getAttribute('defaultProfile'));

		// Extract Groups.
		groupsList.html('');
		for (let group = targetNode.firstChild; group !== null; group = group.nextSibling) {
			if (group.nodeName != 'group')
				continue;
			if (!Group.alreadyInUseGroupName(group.getAttribute('name'))) {
				showMessage('error', 'duplicated group ' + group.getAttribute('name'));
				continue;
			}
			let elements = [];
			for (let element = group.firstChild; element !== null; element = element.nextSibling) {
				if (element.nodeName != 'element')
					continue;
				elements.push(element.getAttribute('name'));
			}
			let groupObj = Group(group.getAttribute('name'), elements);
			groupsList.append(
				createItem(groupObj.getName(), 'group', 'groupsDialog')
					.data('values', groupObj)
					.attr('id', groupObj.getName() == 'All' ? 'All': '')
			);
		}
		Group.updateGroups();
	});
});

saveConfiguration.on('click', function(e) {
	e.preventDefault();
	if (!saveConfigurationToFile())
		return;
	setConfigurationDirty(false);
});

/**
 * Class to handle the configuration file.
 */
function Configuration() {

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
