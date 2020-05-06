const

	showMessage   = require('./errors').showMessage,

	devicesDialog = $('#devicesDialog'),

	elementDialog = $('#elementDialog'),
	elementForm   = $('form', elementDialog),
	addElement    = $('#addElement'),
	elementName   = $('input[name="elementName"]', elementDialog),
	elementR      = $('input[name="R"]', elementDialog),
	elementG      = $('input[name="G"]', elementDialog),
	elementB      = $('input[name="B"]', elementDialog),
	elementPin    = $('input[name="pin"]', elementDialog),
	keepOpen      = $('input[name="keepOpen"]', elementDialog),
	deleteElement = $('.modal-footer button:first-child', elementDialog),
	rgbSel        = $('#rgbSel').prop('checked', true),
	singleSel     = $('#singleSel').prop('checked', true);

/**
 * Sets the maximun allowed pin nomber.
 * @param number value
 */
function setPinMaxValue(value) {
	[elementR, elementG, elementB, elementPin].forEach((e) => e.attr('max', value));
}

/**
 * Checks that the pin is not repeated on the element dialog.
 */
$.validator.addMethod('duplicatedPin', function(value, element) {

	if (value == '')
		return true;

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
 * Activate validator for elements.
 */
elementForm.validate({
	rules: {
		elementName: {
			alreadyInUseElementName: true
		},
		R: {
			duplicatedPin  : true,
			alreadyInUsePin: true
		},
		G: {
			duplicatedPin  : true,
			alreadyInUsePin: true
		},
		B: {
			duplicatedPin  : true,
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
	errorPlacement: require('./errors').errorPlacement
});

/**
 * Create or Edit elements.
 */
elementDialog.on('show.bs.modal', function(e) {

	let button = $(e.relatedTarget);
	require('./errors').resetErrors(elementDialog);

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
			elemNode = require('./generic').createItem(data.getName(), 'element', 'elementDialog');
		elemNode.data('values', data);
		elementList.append(elemNode[0]);
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

/**
 * Creates a new element.
 * @param string newName.
 * @param number[] newPins.
 */
function Element(newName, newPins = []) {

	let
		name = newName,
		pins = newPins;

	/**
	 * @returns bool true if the element is RGB.
	 */
	function isRGB() {
		return pins.length == 3;
	}

	/**
	 * @returns string name.
	 */
	function getName() {
		return name;
	}

	/**
	 * @returns number the red led.
	 */
	function getRed() {
		return pins[0];
	}

	/**
	 * @returns number the green led.
	 */
	function getGreen() {
		return pins[1];
	}

	/**
	 * @returns number the red led.
	 */
	function getBlue() {
		return pins[2];
	}

	/**
	 * @returns number the single led.
	 */
	function getLed() {
		return pins[0];
	}

	return Object.freeze({

		isRGB    : isRGB,
		getName  : getName,
		getRed   : getRed,
		getGreen : getGreen,
		getBlue  : getBlue,
		getLed   : getLed,

		/**
		 * Sets a the name.
		 * @param string newName
		 * @returns Element
		 */
		setName(newName) {
			name = newName;
			return this;
		},

		/**
		 * @param nunber led.
		 * @returns Element
		 */
		setRed(led) {
			if (!isRGB())
				pins = [led, 0 , 0];
			else
				pins[0] = led;
			return this;
		},

		/**
		 * @param nunber led.
		 * @returns Element
		 */
		setGreen(led) {
			if (!isRGB())
				pins = [0, led , 0];
			else
				pins[1] = led;
			return this;
		},

		/**
		 * @param nunber led.
		 * @returns Element
		 */
		setBlue(led) {
			if (!isRGB())
				pins = [0, 0, led];
			else
				pins[2] = led;
			return this;
		},

		/**
		 * @param nunber led.
		 * @returns Element
		 */
		setLed(led) {
			pins = [led];
			return this;
		},

		/**
		 * @returns number[].
		 */
		getPins() {
			return pins;
		},

		/**
		 * Sets a the pin(s).
		 * @param number[] newPins
		 * @returns Element
		 */
		setPins(newPins) {
			pins = newPins;
			return this;
		},

		/**
		 * @returns string the object converted into XML.
		 */
		toXML() {
			let r = '\t\t\t<element name="' + getName() + '"';
			if (isRGB())
				r += ' red="' + getRed() + '" green="' + getGreen() + '" blue="' + getBlue() + '"';
			else
				r += ' led="' + getLed() + '"';
			r += ' />\n';
			return r;
		}
	})
};

module.exports = Element;
module.exports.setPinMaxValue = setPinMaxValue;
