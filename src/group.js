/**
 * Class to keep Group data.
 */
module.exports = function(newName, newElements = []) {

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
