'use strict'

const {app, BrowserWindow} = require('electron');

let window = null;

app.on('ready', createWindow);
app.on('window-all-closed', () => {
	if (process.platform !== 'darwin')
		app.quit();
})
app.on('activate', () => {
	if (window === null)
		createWindow();
})

/**
 * Initialize the app window.
 */
function createWindow () {
	window = new BrowserWindow({
		width : 1024,
		height: 720,
		webPreferences: {
			nodeIntegration: true
		}
	});
	window.setMenu(null);
	window.openDevTools();
	window.loadFile(`${__dirname}/index.html`);
	window.on('closed', () => {
		window = null;
	});
}
