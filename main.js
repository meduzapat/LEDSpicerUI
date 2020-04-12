const { app, BrowserWindow } = require('electron')

class Main {

	constructor() {
		let $ = require('jquery')
		app.on('ready', this.createWindow)
		app.on('window-all-closed', () => {
			if (process.platform !== 'darwin') {
				app.quit()
			}
		})
		app.on('activate', () => {
			if (this.browserWindow === null) {
				this.createWindow()
			}
		})
	}

	createWindow () {
		this.win = new BrowserWindow({
			width:     1280,
			height:    1024,
			minWidth:  640,
			minHeight: 480,
			webPreferences: {
				nodeIntegration: true
			}
		})

		this.win.loadFile('index.html')
		this.win.on('closed', () => {
			this.win = null
		})
	}

}

const main = new Main()
