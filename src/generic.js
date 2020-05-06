
/**
 * Creates entries for the different lists.
 * @param string name
 * @param string type
 * @param string toggle if set will open this DOM ID
 * @returns
 */
module.exports.createItem = function(name, type, toggle = null) {
	return $('<li class="elementBox ' + type + '"' + (toggle ? ' data-toggle="modal" data-target="#' + toggle + '"' :'') + '><span>' + name + '</span><code>&#10495;</code></li>');
}

/**
 * Creates options for selects.
 * @param string value
 * @param string label
 * @param boolean selected
 * @returns jQuery object with the option.
 */
module.exports.createOption = function(value, label, selected = false) {
	return $('<option value="' + value + '" '+ (selected ? 'selected' : '') +'>' + label + '</option>');
}

/**
 * Leaves the confirmation dialog ready to be used.
 * @param string buttonId the id of the button that will trigger the affirmative event.
 * @param strin text a text to display.
 */
module.exports.prepareConfirmationDialog = function(buttonId, text = "Are you sure?") {
	let deleteConfirmation = $('#deleteConfirmation');
	$('button:first-child', deleteConfirmation).attr('id', buttonId);
	$('h5', deleteConfirmation).html(text);
}

/**
 * Enable / Disable the save configuration button.
 * @param bool val true to activate / false to deactivate
 */
module.exports.setConfigurationDirty = function(val) {
	$('form', configuration).data('dirty', val);
	$('#saveConfiguration').prop('disabled', !val);
}

