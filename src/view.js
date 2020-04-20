'use strict'

let body, messages;

function generateRandomColor() {
	let r = "#"
	for (var c = 0; c < 3; ++c) {
		r += (Math.floor(Math.random() * 4 ) + 1).toString(16)
		r += Math.floor(Math.random()*16).toString(16)
	}
	return r;
}

function createItem(name, type, toggle = null) {
	return $('<li class="btn elementBox ' + type + '"' + (toggle ? 'data-toggle="modal" data-target="#' + toggle + '"' :'') + '><span>' + name + '</span><code>&#10495;</code></li>');
}

function createOption(value, label, selected = false) {
	return $('<option value="' + value + '" '+ (selected ? 'selected' : '') +'>' + label + '</option>');
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
 * Extract pin information out of the element data record.
 * @param data
 * @returns
 */
function getElementPins(data) {
	if (typeof data.pin == 'undefined')
		return [data.r, data.g, data.b];
	return [data.pin];
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

$(document).ready(() => {

	body     = $('body');
	messages = $('#messages');

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
		value = parseInt(value);
		let
			good    = true,
			current = deleteElement.data('current');

		$('.elementBox', elementList).each(function() {
			if (this != current && getElementPins($(this).data('values')).indexOf(value) != -1)
				good = false;
		});
		return good;
	}, function(value, element) {
		let e = $(element);
		return 'Pin ' + e.val() + ' set in ' + e.attr('name') + ' is already in use by other element on this device';
	});

	/**
	 * Checks that the element's name is not used.
	 */
	$.validator.addMethod('alreadyInUseElementName', function(value, element) {
		// TODO, elements from other devices.
		let
			good    = true,
			current = deleteElement.data('current');

		$('.elementBox', elementList).each(function() {
			if (this != current && $(this).data('values').name == value)
				good = false;
		});
		return good;
	}, function(value, element) {
		return 'Element name ' + $(element).val() + ' is already in use by other element';
	});

	let
		devicesList    = $('#devicesList'),
		groupList      = $('#groupList'),
		profilesList   = $('#profilesList'),
		animationsList = $('#animationsList'),
		inputsList     = $('#inputsList');

/***************
 * Device Form *
 ***************/

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

	/**
	 * Create device button opens the device dialog and resets its fields.
	 */
	devicesDialog.on('show.bs.modal', function (e) {

		// Keeps the clicked device when editing.
		let element = $(e.relatedTarget);

		sortable('#elementList', 'enable');
		resetErrors(devicesDialog);
		deviceOptions.hide();
		elementList.html('');
		devicesDialog.data('dirty', false);
		deviceCode.html(createOption('', 'Select Device'));
		for (const d in devList)
			if (Device.isAvailable(d))
				deviceCode.append(createOption(d, devList[d]['name']));

		/*************************************************************************************** DELETE ME /*/
		for (let c = 1; c < 20; ++c) {
			let e = createItem('P1_BUTTON' + c, 'element', 'elementDialog');
			$(e).data('values', {name: 'P1_BUTTON' + c, pin: parseInt(c)});
			elementList.append(e);
		}



		// Create.
		if (element.attr('id') == 'addDevice') {
			saveDevice.data('current', null);
			deviceId.html('');
			deviceId.prop('disabled', true);
			addElement.prop('disabled', true);
			deleteDevice.hide();
			return;
		}

		// Edit.
		deviceId.prop('disabled', false);
		addElement.prop('disabled', false);
		saveDevice.data('current', element);
		deleteDevice.show();
		let
			data = element.data('values'),
			sel  = deviceCode.children('option[value="' + data.code + '"]');

		// Populate Code.
		if (!sel.length)
			deviceCode.append(createOption(data.getCode(), Device.convertCode(data.getCode()), true));
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
			let e = createItem(elementData[i].name, 'element', 'elementDialog');
			$(e).data('values', elementData[i]);
			elementList.append(e);
		}

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
			deviceNode = saveDevice.data('current'),
			device     = deviceNode ? deviceNode.data('values') : null,
			devText    = deviceCode.val() + ' [' + deviceId.val() + ']',
			options    = {};

		$('li', elementList).each(function() {
			elements.push($(this).data('values'));
		});

		$('input', deviceOptions).each(function() {
			options[this.name] = this.value;
		});

		devicesDialog.modal('hide');

		// Create.
		if (!device) {
			device = new Device(
				deviceCode.val(),
				parseInt(deviceId.val()),
				elements,
				options
			);

			deviceNode = createItem(devText, 'device', 'devicesDialog');
			devicesList.append(deviceNode);
			deviceNode.data('values', device);
			showMessage('success', 'Device '+ device.getName() +' with id ' + device.getId() +' Created');
			return;
		}

		// Update.
		device
			.change(deviceCode.val(), parseInt(deviceId.val()))
			.setElements(elements)
			.setOptions(options);
		$('span', deviceNode).html(devText);
		showMessage('success', 'Device Updated');
	});

	/**
	 * Delete Device callback.
	 */
	$('#deleteDevice').on('click', function() {
		// Save device holds the element, the element holds the data.
		$(saveDevice.data('current')).remove();
		showMessage('success', 'Device Deleted');
		devicesDialog.modal('hide');
	});

/****************
 * Element Form *
 ****************/

	let
		rgbSel    = $('#rgbSel').prop('checked', true),
		singleSel = $('#singleSel').prop('checked', true);

	/**
	 * Create and disable sortable for element list.
	 */
	sortable('#elementList',{
		placeholderClass: 'btn elementBox element placeholder',
		forcePlaceholderSize: true,
		handle: 'code'
	});
	sortable('#elementList', 'disable');

	/**
	 * Create or Edit elements.
	 */
	elementDialog.on('show.bs.modal', function (e) {

		let button = $(e.relatedTarget);
		resetErrors(elementDialog);

		// Create.
		if (button.attr('id') == 'addElement') {
			elementName.val('');
			rgbSel.click();
			keepOpen.parent().show();
			keepOpen.prop('checked', false);
			deleteElement.hide();
			deleteElement.data('current', null);
			//deviceDialog.modal('handleUpdate');
			return;
		}

		// Edit.
		let data = button.data('values');
		elementName.val(data.name);
		if (!data.pin) {
			rgbSel.click();
			elementR.val(data.r);
			elementG.val(data.g);
			elementB.val(data.b);
		}
		else {
			singleSel.click();
			elementPin.val(data.pin);
		}

		deleteElement.data('current', e.relatedTarget);
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
			data    = {name: elementName.val()},
			elems   = $('.elementBox', elementList),
			current = deleteElement.data('current');

		if ($('input[name="elementType"]:checked', elementDialog).val() == 'rgb') {
			data.r = parseInt(elementR.val());
			data.g = parseInt(elementG.val());
			data.b = parseInt(elementB.val());
		}
		else {
			data.pin = parseInt(elementPin.val());
		}

		// Create.
		if (!current) {
			let elemNode = createItem(data.name, 'element', 'elementDialog');
			elemNode.data('values', data);
			elementList.append(elemNode);
			if (!keepOpen.is(':checked'))
				elementDialog.modal('hide');
			showMessage('success', 'Element Created');
		}
		// Update.
		else {
			current = $(current);
			current.data('values', data);
			$('span', current).html(data.name);
			elementDialog.modal('hide');
			showMessage('success', 'Element Updated');
		}
		sortable('#elementList', 'reload');
		devicesDialog.data('dirty', true);
	});

	/**
	 * Delete element button.
	 */
	$('#deleteElement').on('click', () => {
		$(deleteElement.data('current')).remove();
		sortable('#elementList', 'reload');
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

/**********
 * Groups *
 **********/

	let
		groupsDialog     = $('#groupsDialog'),
		groupName        = $('input[name="groupName"]'),
		deleteGroup      = $('#deleteGroup'),
		groupElementList = $('#groupElementList'),
		allElementList   = $('#allElementList');

	/**
	 * Creates the All group.
	 */
	groupList.append(createItem('All', 'group', 'groupsDialog').attr('id', 'All'));

	function getAllElements() {
		let r = [];
		$('li', devicesList).each(function () {
			let e = $(this).data('values').getElements();
			for (let ei in e)
				r.push(e[ei].name);
		});
		return r;
	}

	/**
	 * Create and disable sortable for groups element lists.
	 */
	sortable('#allElementList, #groupElementList', {
		placeholderClass: 'btn elementBox element placeholder',
		forcePlaceholderSize: true,
		handle: 'code',
		acceptFrom: '#groupElementList, #allElementList'
	});
	sortable('#groupElementList, #allElementList', 'disable');

	/**
	 * Create or Edit groups.
	 */
	groupsDialog.on('show.bs.modal', function (e) {

		resetErrors(groupsDialog);

		groupElementList.html('');
		allElementList.html('');
		deleteGroup.hide();

		let
			button      = $(e.relatedTarget),
			isAll       = button.attr('id') == 'All',
			isCreate    = button.attr('id') == 'addGroup',
			allElements = getAllElements(),
			elements    = [],
			data;

		// isAll settings.
		if (isAll) {
			button.data('values', new Group('All', getAllElements()));
			allElementList.hide();
			groupName.prop('disabled', true);
			sortable('#allElementList', 'disable');
		}
		// Edit/New shared settings.
		else {
			sortable('#allElementList', 'enable');
			allElementList.show();
			groupName.prop('disabled', false);
		}

		sortable('#groupElementList', 'enable');

		// Populate Elements.
		if (!isCreate) {
			data        = button.data('values');
			elements    = data.getElements();
		}
		for (let i in allElements) {
			let name = allElements[i];
			if (elements.indexOf(name) === -1)
				allElementList.append(createItem(name, 'element'));
			else
				groupElementList.append(createItem(name, 'element'));
		}

		sortable('#groupElementList, #allElementList', 'reload');

		// Create.
		if (isCreate) {
			groupName.val('');
			deleteGroup.data('current', null);
			return;
		}

		// Edit.
		// Populate Name.
		groupName.val(data.getName());
		deleteGroup.data('current', e.relatedTarget);
	});

/************
 * Profiles *
 ************/

});
