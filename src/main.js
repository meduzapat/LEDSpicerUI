'use strict'

const {app, BrowserWindow} = require('electron');

app.on('ready', createWindow);
app.on('window-all-closed', () => {
	if (process.platform !== 'darwin')
		app.quit();
})
app.on('activate', () => {
	if (browserWindow === null)
		createWindow();
})

/**
 * Initialize the app window.
 */
function createWindow () {
	let win = new BrowserWindow({
		width:     1024,
		height:    720,
		webPreferences: {
			nodeIntegration: true
		}
	});
	win.setMenu(null);
	win.openDevTools();
	win.loadFile('index.html');
	win.on('closed', () => {
		win = null
	});
}
