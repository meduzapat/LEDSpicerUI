/**
 * Class to keep Group data.
 */
let Group = (function () {

	let
		name,
		elements,
		allGroups = {};

	class Group {

	constructor(newName, newElements = []) {
		name     = newName;
		elements = newElements;
		Group.registerGroup(name, elements);
	}

	getName() {
		return name;
	}

	setName(newName) {
		name = newName;
		return this;
	}

	getElements() {
		return elements;
	}

	setElements(newElemennts) {
		elements = newElemennts;
		return this;
	}

	static removeGroup(name) {
		delete allGroups[name];
	}

	static registerGroup(newName, newElements) {
		allGroups[newName] = newElements;
	}

	toXML() {
		return '<groups/>';
	}

	}

	module.exports.Group = Group;

})();
