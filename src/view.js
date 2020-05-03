'use strict'

$(document).ready(() => {

	/**
	 * Creates entries for the different lists.
	 * @param string name
	 * @param string type
	 * @param string toggle if set will open this DOM ID
	 * @returns
	 */
	function createItem(name, type, toggle = null) {
		return $('<li class="elementBox ' + type + '"' + (toggle ? 'data-toggle="modal" data-target="#' + toggle + '"' :'') + '><span>' + name + '</span><code>&#10495;</code></li>');
	}

	/**
	 * Creates options for selects.
	 * @param string value
	 * @param string label
	 * @param boolean selected
	 * @returns jQuery object with the option.
	 */
	function createOption(value, label, selected = false) {
		return $('<option value="' + value + '" '+ (selected ? 'selected' : '') +'>' + label + '</option>');
	}

	function prepareConfirmationDialog(buttonId, text = "Are you sure?") {
		$('button:first-child', deleteConfirmation).attr('id', buttonId);
		$('h5', deleteConfirmation).html(text);
	}

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

	function showMessage(type, message, custLabel = null) {
		let aClass, label;
		switch (type) {
		case 'error':
			aClass = 'alert-danger';
			label  = custLabel ? custLabel : 'Error';
			break;
		case 'warning':
			aClass = 'alert-warning';
			label  = custLabel ? custLabel : 'Warning';
			break;
		case 'success':
			aClass = 'alert-success';
			label  = custLabel ? custLabel : 'Success';
			break;
		default:
			aClass = 'alert-info';
			label  = custLabel ? custLabel : 'Info';
		}
		let elem = $('<li class="alert ' + aClass + ' hide" role="alert" ><strong>' + label + '</strong>' + message + '</li>');
		messages.prepend(elem);
		elem.slideDown(400);
		setTimeout(() => {
			elem.fadeOut(400, () => {elem.remove()});
		}, 5000)
	}

	/**
	 * Resets errors on the dialog
	 * @param dialog
	 */
	function resetErrors(dialog) {
		$('.error', dialog).removeClass('error');
	}

	/**
	 * Displays an error from the validator.
	 * @param error element with the error msg.
	 * @param element element that generated the error.
	 * @returns bool
	 */
	function errorPlacement(error, element) {
		showMessage('error', error.html());
		return true;
	}

	let
		body     = $('body'),
		messages = $('#messages'),
		file     = null;

	/**
	 * Updates the Groups after an element is modified.
	 */
	function updateGroups() {

		if (!$('#All').length) {
			groupsList.prepend(createItem('All', 'group', 'groupsDialog').attr('id', 'All'));
		}

		let
			all          = $('#All'),
			allGroupData = all.data('values'),
			allElements  = [];

		// Get all elements.
		$('li', devicesList).each(function() {
			allElements = allElements.concat($(this).data('values').getElementsName());
		});

		// Check generic groups.
		$('li:not(#All)', groupsList).each(function() {
			let
				self = $(this),
				vals = self.data('values'),
				elem = allElements.filter(d => vals.getElements().includes(d));
			if (!elem.length)
				self.remove();
			else
				vals.setElements(elem);
		});

		// If does not exists create the All group.
		if (!allGroupData) {
			all.data('values', Group('All', allElements));
			return;
		}

		// Update All group.
		let
			oldElements = allGroupData.getElements().filter(d => allElements.includes(d)),
			newElements = allElements.filter(d => !oldElements.includes(d));
		all.data('values', Group('All', oldElements.concat(newElements)));
	}

	/**
	 * Checks that the pin is not repeated on the element dialog.
	 */
	$.validator.addMethod('duplicatedPin', function(value, element) {

		let
			name = $(element).attr('name'),
			good = true;

		[elementR, elementG, elementB].forEach((e) => {
			if (name != e.attr('name') && value == e.val())
				good = false;
		});

		return good;
	}, function(value, element) {
		let e = $(element);
		return 'Pin ' + e.val() + ' set on ' + e.attr('name') + ' is already set on another pin for this element';
	});

	/**
	 * Checks that the element's pin is not used on the device.
	 */
	$.validator.addMethod('alreadyInUsePin', function(value, element) {

		if (value == '')
			return true;

		value = parseInt(value);
		let
			found   = false,
			current = elementDialog.data('current');

		$('li span', elementList).each(function() {
			// Edit.
			if (current) {
				if ($(this).html() != $(current).data('values').getName() && $(this).parent().data('values').getPins().indexOf(value) != -1) {
					found = true;
					return false;
				}
			}
			// New.
			else {
				if ($(this).parent().data('values').getPins().indexOf(value) != -1) {
					found = true;
					return false;
				}
			}
		});

		return !found;
	}, function(value, element) {
		let e = $(element);
		return 'Pin ' + e.val() + ' set in ' + e.attr('name') + ' is already in use by other element on this device';
	});

	/**
	 * Checks that the element's name is not used.
	 */
	$.validator.addMethod('alreadyInUseElementName', function(value, element) {

		if (elementName.val() == '')
			return true;

		let
			found   = false,
			current = elementDialog.data('current');

		// Search current Dialog.
		$('li span', elementList).each(function() {
			let elem = $(this);
			// Edit.
			if (current) {
				if (current != elem.parent().get(0) && elem.html() == value) {
					found = true;
					return false;
				}
			}
			// New.
			else {
				if (elem.html() == value) {
					found = true;
					return false;
				}
			}
		});

		current = devicesDialog.data('current');
		// Search in other Devices.
		if (!found) {
			$('li', devicesList).each(function() {
				let elem = $(this);
				// Edit.
				if (current) {
					if (current != elem.get(0) && elem.data('values').getElementsName().indexOf(value) != -1) {
						found = true;
						return false;
					}
				}
				// New.
				else {
					if (elem.data('values').getElementsName().indexOf(value) != -1) {
						found = true;
						return false;
					}
				}
			});
		}

		return !found;

	}, function(value, element) {
		return 'Element name ' + element.value + ' is already in use by other element';
	});

	/**
	 * @param string value
	 * @param element
	 * @returns bool true if the element does not exists.
	 */
	function checkAlreadyInUseName(value, element) {
		let
			current = groupsDialog.data('current'),
			groups  = [];

		$('li span', groupsList).each(function() {
			groups.push($(this).text());
		});

		if (!groups)
			return true;

		if (!current) {
			return (groups.indexOf(value) == -1);
		}
		return (value == $(current).data('values').getName() || groups.indexOf(value) == -1);
	}

	/**
	 * Checks that the element's name is not used.
	 */
	$.validator.addMethod('alreadyInUseGroupName', checkAlreadyInUseName, function(value, element) {
		return 'Element name ' + element.value + ' is already in use by other element';
	});

	/**********************
	 * Configuration Form *
	 **********************/

	let
		currentFile    = $('#currentFile'),
		devicesList    = $('#devicesList'),
		groupsList      = $('#groupsList'),
		profilesList   = $('#profilesList'),
		animationsList = $('#animationsList'),
		inputsList     = $('#inputsList'),
		configuration  = $('#configuration'),
		resetConfiguration = $('#resetConfiguration'),
		loadConfiguration  = $('#loadConfiguration'),
		saveConfiguration  = $('#saveConfiguration'),
		configurationForm  = $('form', configuration),
		fps            = $('input[name="fps"]', configuration),
		port           = $('input[name="port"]', configuration),
		userId         = $('input[name="userId"]', configuration),
		groupId        = $('input[name="groupId"]', configuration),
		colors         = $('select[name="colors"]', configuration),
		logLevel       = $('select[name="logLevel"]', configuration),
		defaultProfile = $('select[name="defaultProfile"]', configuration);

	/**
	 * Activate validator for configuration.
	 */
	configurationForm.validate({
		errorPlacement: errorPlacement
	});

	/**
	 * Enable / Disable the save configuration button.
	 * @param bool val true to activate / false to deactivate
	 */
	function setDirtyConfiguration(val) {
		configurationForm.data('dirty', val);
		saveConfiguration.prop('disabled', !val);
	}
	setDirtyConfiguration(false);

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
					Config()
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
				setDirtyConfiguration(false);
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
		setDirtyConfiguration(true);
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
		updateGroups();
		setDirtyConfiguration(false);
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
			Device.reset();
			setDirtyConfiguration(false);

			// Extract Devices.
			let
				checkName = [],
				targetNode = data.getElementsByTagName("devices")[0];
			for (let device = targetNode.firstChild; device !== null; device = device.nextSibling) {

				if (device.nodeName != 'device')
					continue;

				if (!Device.isIdAvailable(device.getAttribute('name'), device.getAttribute('boardId'))) {
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
				if (!checkAlreadyInUseName(group.getAttribute('name'))) {
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
			updateGroups();
		});
	});

	saveConfiguration.on('click', function(e) {
		e.preventDefault();
		if (!saveConfigurationToFile())
			return;
		setDirtyConfiguration(false);
	});

/***************
 * Device Form *
 ***************/

	let deleteConfirmation = $('#deleteConfirmation');

	let
		devicesDialog = $('#devicesDialog'),
		devicesForm   = $('form', devicesDialog),
		deviceCode    = $('select[name="deviceCode"]', devicesDialog),
		deviceId      = $('select[name="deviceId"]', devicesDialog),
		elementList   = $('#elementList'),
		deviceOptions = $('#deviceOptions', devicesDialog),
		devList       = Device.getList(),
		saveDevice    = $('#saveDevice'),
		deleteDevice  = $('.modal-footer button:first-child', devicesDialog),
		// Element
		elementDialog = $('#elementDialog'),
		elementForm   = $('form', elementDialog),
		addElement    = $('#addElement', devicesDialog),
		elementName   = $('input[name="elementName"]', elementDialog),
		elementR      = $('input[name="R"]', elementDialog),
		elementG      = $('input[name="G"]', elementDialog),
		elementB      = $('input[name="B"]', elementDialog),
		elementPin    = $('input[name="pin"]', elementDialog),
		keepOpen      = $('input[name="keepOpen"]', elementDialog),
		deleteElement = $('.modal-footer button:first-child', elementDialog);

	/**
	 * Activate validator for devices.
	 */
	devicesForm.validate({
		messages: {
			deviceCode : 'The device type is required',
			deviceId   : 'The device ID is required'
		},
		errorPlacement: errorPlacement
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
		resetErrors(devicesDialog);
		prepareConfirmationDialog('deleteDevice');
		deviceOptions.hide();
		elementList.html('');
		devicesDialog.data('dirty', false);
		deviceCode.html(createOption('', 'Select Device'));
		for (const d in devList)
			if (Device.isAvailable(d))
				deviceCode.append(createOption(d, devList[d]['name']));

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

		if (deviceId.children("option").length == data.getId()) {
			deviceId.append(opt);
		}
		else {
			deviceId.children("option").filter(function() {
				return parseInt(this.value) > data.getId();
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
		if (!$.isEmptyObject(devList[data.getCode()]['options']))
			for (let o in devList[data.getCode()]['options'])
				$('input[name="' + o + '"]', deviceOptions).val(data.getOptions()[o]);
	});

	/**
	 * Device type selector activates the other elements and provides max pins to elements dialog.
	 */
	deviceCode.on('change', function(e) {
		deviceId.html(createOption('', 'Select Id'));
		let code = deviceCode.children("option:selected").val();
		for (let c = 1; c <= devList[code]['maxDevices']; ++c)
			if (Device.isIdAvailable(code, c))
				deviceId.append(createOption(c, c));
		devicesDialog.data('dirty', true);
		deviceId.prop('disabled', false);
		addElement.prop('disabled', false);
		[elementR, elementG, elementB, elementPin].forEach((e) => e.attr('max', devList[code]['pins']));

		if (!$.isEmptyObject(devList[code]['options']))
			deviceOptions.html('<legend>Extra Options</legend>' + createDeviceOptions(Object.values(devList[code]['options']))).show();
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

		setDirtyConfiguration(true)
		devicesDialog.data('dirty', false);
		devicesDialog.modal('hide');

		// Create.
		if (!device) {
			device = Device(
				deviceCode.val(),
				parseInt(deviceId.val()),
				elements,
				options
			);

			deviceNode = createItem(devText, 'device', 'devicesDialog');
			deviceNode.data('values', device);
			devicesList.append(deviceNode);
			showMessage('success', 'Device '+ device.getName() +' with id ' + device.getId() +' Created');
			updateGroups();
			return;
		}

		deviceNode = $(deviceNode);
		// Update.
		device
			.change(deviceCode.val(), parseInt(deviceId.val()))
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
	deleteConfirmation.on('click', '#deleteDevice', function() {
		// DeviceDialog holds the element, the element holds the data.
		let
			current = $(devicesDialog.data('current')),
			data    = current.data('values');

		data.remove();
		current.remove();
		showMessage('success', 'Device Deleted');
		devicesDialog.modal('hide');
		updateGroups();
		setDirtyConfiguration(true)
	});

	devicesDialog.on('hide.bs.modal', function(e) {
		devicesDialog.data('current', null);
	});

/****************
 * Element Form *
 ****************/

	/**
	 * Activate validator for elements.
	 */
	elementForm.validate({
		rules: {
			elementName: {
				alreadyInUseElementName: true
			},
			R: {
				duplicatedPin: true,
				alreadyInUsePin: true
			},
			G: {
				duplicatedPin: true,
				alreadyInUsePin: true
			},
			B: {
				duplicatedPin: true,
				alreadyInUsePin: true
			},
			pin: {
				alreadyInUsePin: true
			}
		},
		messages: {
			elementName:  {
				required: 'The element name is required',
			},
			R: {
				required: 'R (red color) pin is required',
			},
			G: {
				required: 'G (green color ) pin is required',
			},
			B: {
				required: 'B (blue color) pin is required',
			},
			pin: {
				required: 'pin is required',
			}
		},
		errorPlacement: errorPlacement
	});

	let
		rgbSel    = $('#rgbSel').prop('checked', true),
		singleSel = $('#singleSel').prop('checked', true);

	/**
	 * Create or Edit elements.
	 */
	elementDialog.on('show.bs.modal', function(e) {

		let button = $(e.relatedTarget);
		resetErrors(elementDialog);

		// Create.
		if (button.attr('id') == 'addElement') {
			elementName.val('');
			rgbSel.click();
			keepOpen.parent().show();
			keepOpen.prop('checked', false);
			deleteElement.hide();
			elementDialog.data('current', null);
			return;
		}

		// Edit.
		let data = button.data('values');
		elementName.val(data.getName());
		if (data.isRGB()) {
			rgbSel.click();
			elementR.val(data.getRed());
			elementG.val(data.getGreen());
			elementB.val(data.getBlue());
		}
		else {
			singleSel.click();
			elementPin.val(data.getLed());
		}

		elementDialog.data('current', e.relatedTarget);
		deleteElement.show();
		keepOpen.prop('checked', false);
		keepOpen.parent().hide();

	});

	/**
	 * Save element button.
	 */
	$('#saveElement').on('click', () => {

		if (!elementForm.valid())
			return;

		let
			pins    = [],
			elems   = $('.elementBox', elementList),
			current = elementDialog.data('current');

		if ($('input[name="elementType"]:checked', elementDialog).val() == 'rgb')
			pins = [elementR.val(), elementG.val(), elementB.val()];
		else
			pins = [elementPin.val()];

		// Create.
		if (!current) {
			let
				data     = Element(elementName.val(), pins),
				elemNode = createItem(data.getName(), 'element', 'elementDialog');
			elemNode.data('values', data);
			elementList.append(elemNode);
			if (!keepOpen.is(':checked'))
				elementDialog.modal('hide');
			showMessage('success', 'Element Created');
		}
		// Update.
		else {
			current = $(current);
			current.data('values').setName(elementName.val()).setPins(pins);
			$('span', current).html(elementName.val());
			elementDialog.modal('hide');
			showMessage('success', 'Element Updated');
		}
		sortable('#elementList');
		devicesDialog.data('dirty', true);
	});

	/**
	 * Delete element button.
	 */
	$('#deleteElement').on('click', () => {
		$(elementDialog.data('current')).remove();
		sortable('#elementList');
		showMessage('success', 'Element Deleted');
		elementDialog.modal('hide');
		devicesDialog.data('dirty', true);
	});

	/**
	 * Toggle for the RGB and single pin, for some reason the collapse is crashing.
	 */
	rgbSel.on('click', () => {
		if ($(this).hasClass('active'))
			return;
		[elementR, elementG, elementB].forEach((e) => e.val(''));
		$('#single').removeClass('show');
		$('#RGB').addClass('show');
	});

	/**
	 * Toggle for the RGB and single pin, for some reason the collapse is crashing.
	 */
	singleSel.on('click', () => {
		if ($(this).hasClass('active'))
			return;
		elementPin.val('');
		$('#single').addClass('show');
		$('#RGB').removeClass('show');
	});

	elementDialog.on('hide.bs.modal', function(e) {
		elementDialog.data('current', null);
	});

/**********
 * Groups *
 **********/

	let
		groupsDialog     = $('#groupsDialog'),
		groupsForm       = $('form', groupsDialog),
		groupName        = $('input[name="groupName"]'),
		deleteGroup      = $('.modal-footer button:first-child', groupsDialog),
		groupElementList = $('#groupElementList'),
		allElementList   = $('#allElementList'),
		saveGroup        = $('#saveGroup'),
		allTip           = $('#allTip');

	/**
	 * Activate Validator
	 */
	groupsForm.validate({
		rules: {
			groupName: {
				alreadyInUseGroupName: true
			}
		},
		messages: {
			groupName: {
				required: 'The group name is required.'
			}
		},
		errorPlacement: errorPlacement
	});

	/**
	 * Create and disable sortable for groups element lists.
	 */
	sortable('#allElementList, #groupElementList', {
		placeholderClass    : 'elementBox element placeholder',
		forcePlaceholderSize: true,
		handle              : 'code',
		acceptFrom          : '#groupElementList, #allElementList',
		orientation         : 'horizontal'
	});
	sortable('#groupElementList, #allElementList', 'disable');

	/**
	 * Create or Edit groups.
	 */
	groupsDialog.on('show.bs.modal', function(e) {

		resetErrors(groupsDialog);
		prepareConfirmationDialog('deleteGroup');
		groupElementList.html('');
		allElementList.html('');

		let
			button      = $(e.relatedTarget),
			isAll       = button.attr('id') == 'All',
			isCreate    = button.attr('id') == 'addGroup',
			allElements = [],
			elements    = [],
			data;

		$('li', devicesList).each(function() {
			allElements = allElements.concat($(this).data('values').getElementsName());
		});

		// isAll settings.
		if (isAll) {
			allTip.show();
			deleteGroup.hide();
			allElementList.parent().hide();
			groupName.prop('disabled', true);
			sortable('#allElementList', 'disable');
		}
		// Edit/New shared settings.
		else {
			allTip.hide();
			deleteGroup.show();
			sortable('#allElementList', 'enable');
			allElementList.parent().show();
			groupName.prop('disabled', false);
		}

		sortable('#groupElementList', 'enable');

		// Populate Elements.
		if (!isCreate) {
			data        = button.data('values');
			elements    = data.getElements().filter(d => allElements.includes(d));
			allElements = allElements.filter(d => !elements.includes(d));
			groupsDialog.data('current', button);
		}
		for (let i in elements)
			groupElementList.append(createItem(elements[i], 'element'));

		for (let i in allElements)
			allElementList.append(createItem(allElements[i], 'element'));

		sortable('#groupElementList, #allElementList');

		// Create.
		if (isCreate) {
			groupName.val('');
			deleteGroup.hide();
			groupsDialog.data('current', null);
			return;
		}

		// Edit.
		// Populate Name.
		groupName.val(data.getName());
		groupsDialog.data('current', e.relatedTarget);
	});

	saveGroup.on('click', function(e) {

		if (!groupsForm.valid())
			return e.preventDefault();

		if (groupElementList.text() == '') {
			showMessage('error', 'You need at least one element in the group.');
			return e.preventDefault();
		}

		setDirtyConfiguration(true)

		let
			elements = [],
			current  = groupsDialog.data('current'),
			name     = groupName.val();
		$('span', groupElementList).each(function() {
			elements.push($(this).text());
		});

		// Update.
		if (current) {
			current = $(current);
			current.data('values').setName(name).setElements(elements);
			$('span', current).html(name);
		}

		// Create.
		else {
			groupsList.append(createItem(name, 'group', 'groupsDialog').data('values', Group(name, elements)));
		}

		sortable('#groupElementList, #allElementList', 'disable');
		groupsDialog.modal('hide');
	});

	/**
	 * Delete group callback.
	 */
	deleteConfirmation.on('click', '#deleteGroup', function() {
		let
			current = $(groupsDialog.data('current')),
			data    = current.data('values');
		current.remove();
		showMessage('success', 'Group Removed');
		groupsDialog.modal('hide');
		updateGroups();
		setDirtyConfiguration(true)
	});

	groupsDialog.on('show.bs.modal', function(e) {
		groupsDialog.data('current', null);
	});
/************
 * Profiles *
 ************/


	// Creates the All group.
	updateGroups();
});
