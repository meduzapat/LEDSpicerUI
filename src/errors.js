/**
 * Resets errors on the dialog
 * @param dialog
 */
function resetErrors(dialog) {
	$('.error', dialog).removeClass('error');
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
	$('#messages').prepend(elem);
	elem.slideDown(400);
	setTimeout(() => {
		elem.fadeOut(400, () => {elem.remove()});
	}, 5000)
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

module.exports.showMessage    = showMessage;
module.exports.resetErrors    = resetErrors;
module.exports.errorPlacement = errorPlacement;