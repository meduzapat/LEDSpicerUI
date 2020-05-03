/**
 * Creates a new element.
 * @param string newName.
 * @param number[] newPins.
 */
module.exports = function(newName, newPins = []) {

	let
		name = newName,
		pins = newPins.map(x => parseInt(x));

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
				pins = [parseInt(led), 0 , 0];
			else
				pins[0] = parseInt(led);
			return this;
		},

		/**
		 * @param nunber led.
		 * @returns Element
		 */
		setGreen(led) {
			if (!isRGB())
				pins = [0, parseInt(led) , 0];
			else
				pins[1] = parseInt(led);
			return this;
		},

		/**
		 * @param nunber led.
		 * @returns Element
		 */
		setBlue(led) {
			if (!isRGB())
				pins = [0, 0, parseInt(led)];
			else
				pins[2] = parseInt(led);
			return this;
		},

		/**
		 * @param nunber led.
		 * @returns Element
		 */
		setLed(led) {
			pins = [parseInt(led)];
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
			pins = newPins.map(x => parseInt(x));
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

