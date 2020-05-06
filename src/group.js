const

	setConfigurationDirty = require('./generic').setConfigurationDirty,
	showMessage  = require('./errors').showMessage,

	createItem       = require('./generic').createItem,

	groupsDialog     = $('#groupsDialog'),
	groupsForm       = $('form', groupsDialog),
	groupsList       = $('#groupsList'),
	groupName        = $('input[name="groupName"]'),
	deleteGroup      = $('.modal-footer button:first-child', groupsDialog),
	groupElementList = $('#groupElementList'),
	allElementList   = $('#allElementList'),
	saveGroup        = $('#saveGroup'),
	allTip           = $('#allTip');

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
 * @param string value
 * @param element
 * @returns bool true if the element does not exists.
 */
function alreadyInUseGroupName(value, element) {
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
$.validator.addMethod('alreadyInUseGroupName', alreadyInUseGroupName, function(value, element) {
	return 'Element name ' + element.value + ' is already in use by other element';
});

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
	errorPlacement: require('./errors').errorPlacement
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

	require('./errors').resetErrors(groupsDialog);
	require('./generic').prepareConfirmationDialog('deleteGroup');
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

	setConfigurationDirty(true)

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
$('#deleteConfirmation').on('click', '#deleteGroup', function() {
	let
		current = $(groupsDialog.data('current')),
		data    = current.data('values');
	current.remove();
	showMessage('success', 'Group Removed');
	groupsDialog.modal('hide');
	updateGroups();
	setConfigurationDirty(true)
});

groupsDialog.on('hide.bs.modal', function(e) {
	groupsDialog.data('current', null);
});


/**
 * Class to keep Group data.
 */
function Group(newName, newElements = []) {

	let
		name     = newName,
		elements = newElements;

	return Object.freeze({

		/**
		 * @returns string the group name.
		 */
		getName() {
			return name;
		},

		/**
		 * Replaces the name.
		 * @param string newName
		 */
		setName(newName) {
			name = newName;
			return this;
		},

		/**
		 * @return string[] elements
		 */
		getElements() {
			return elements;
		},

		/**
		 * Adds elements into the group.
		 * @param string[] newElements.
		 */
		setElements(newElemennts) {
			elements = newElemennts;
			return this;
		},

		/**
		 * @returns string the object converted into XML.
		 */
		toXML() {
			let r = '\t\t<group name="' + name + '">\n';
			elements.forEach(e => r += '\t\t\t<element name="' + e + '"/>\n');
			r += '\t\t</group>\n';
			return r;
		}
	});
}

module.exports = Group;
module.exports.updateGroups = updateGroups;
module.exports.alreadyInUseGroupName = alreadyInUseGroupName;
