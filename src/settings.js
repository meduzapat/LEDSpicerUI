$(document).ready(() => {

	(function () {

		let
			settingsDialog = $('#settingsDialog'),
			darkMode       = $('input[name="darkMode"]', settingsDialog),
			displayHelp    = $('input[name="displayHelp"]', settingsDialog);

		(() => {
			// Check Storage.
			if (typeof(Storage) === "undefined") {
				console.warn('no storage found');
				$('input, select', settingsDialog).prop('disabled',true);
			}

			// Set Dark Mode.
			let darkModeVal = localStorage.darkMode;
			if (!darkModeVal)
				darkModeVal = 'off';
			$('input[name="darkMode"][value="' + darkModeVal + '"]', settingsDialog).click();

			if (darkModeVal == 'on')
				$('body').addClass('dark');

			// Set Display Help.
			/**
			 * Activate tooltips
			 */
			$('.tooltipElem:not(span)').tooltip();

			let displayHelpVal = localStorage.displayHelp;
			if (!displayHelpVal)
				displayHelpVal = 'on';
			$('input[name="displayHelp"][value="' + displayHelpVal + '"]', settingsDialog).click();
			if (displayHelp == 'on')
				$('span.tooltipElem').tooltip();
			else
				$('span.tooltipElem').addClass('hide')

		})();

		/**
		 * Handle the dark mode switch.
		 */
		darkMode.on('change', () => {
			localStorage.setItem('darkMode', $('input[name="darkMode"]:checked', settingsDialog).val());
			if (localStorage.darkMode == 'on')
				$('body').addClass('dark');
			else
				$('body').removeClass('dark');
		});

		/**
		 * Handle the dark mode switch.
		 */
		displayHelp.on('change', () => {
			localStorage.setItem('displayHelp', $('input[name="displayHelp"]:checked', settingsDialog).val());
			if (localStorage.displayHelp == 'on')
				$('span.tooltipElem').removeClass('hide').tooltip('enable');
			else
				$('span.tooltipElem').tooltip('disable').addClass('hide')
		});

	})();
});