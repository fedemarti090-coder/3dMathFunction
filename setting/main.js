import { app, BrowserWindow, ipcMain } from 'electron';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import net from 'node:net';

const __dirname = path.dirname(fileURLToPath(import.meta.url));
let win;
let client;
let reconnectTimer;
let isShuttingDown = false;
let incomingData = '';

const isFiniteNumber = (value) => typeof value === 'number' && Number.isFinite(value);

function serializeDataGet(data) {
    const scalarFields = [
        data.dominioMax,
        data.dominioMin,
        data.step,
        data.rotateX,
        data.rotateY,
        data.rotateZ,
        data.alpha
    ];
    const colors = [data.col1, data.col2];

    if (typeof data.function !== 'string' || /[;\r\n]/.test(data.function)
        || !scalarFields.every(isFiniteNumber)
        || !colors.every((color) => color && ['R', 'G', 'B'].every((key) => isFiniteNumber(color[key])))
        || !['asseX', 'asseY', 'asseZ', 'Superfici0'].every((key) => typeof data[key] === 'boolean')) {
        throw new TypeError('DataGet contiene valori non validi');
    }

    const colorString = (color) => `${color.R},${color.G},${color.B}`;
    return `${data.function};${data.dominioMax};${data.dominioMin};${data.step};${data.rotateX};${data.rotateY};${data.rotateZ};${colorString(data.col1)};${colorString(data.col2)};${data.alpha};${data.asseX ? 1 : 0},${data.asseY ? 1 : 0},${data.asseZ ? 1 : 0},${data.Superfici0 ? 1 : 0}\n`;
}

function sendSetting(dataGetObj) {
    const rawPayload = serializeDataGet(dataGetObj);
    if (!client || client.destroyed || !client.writable || client.connecting) {
        return false;
    }
    client.write(rawPayload);
    return true;
}

function sendSocketStatus(connected) {
    if (win && !win.isDestroyed()) {
        win.webContents.send('socket:status', connected);
    }
}

function handleCppData(data) {
    incomingData += data.toString('utf8');
    const lines = incomingData.split('\n');
    incomingData = lines.pop();

    for (const line of lines) {
        if (!line.trim()) continue;
        if (line.startsWith('FORMULA_STATUS;')) {
            const status = line.slice('FORMULA_STATUS;'.length);
            if (status !== 'VALID' && status !== 'INVALID') {
                console.error('[Electron] Stato formula non valido:', line);
                continue;
            }
            if (win && !win.isDestroyed()) {
                win.webContents.send('cpp:formula-status', { valid: status === 'VALID' });
            }
            continue;
        }

        const values = line.split(';').map(Number);
        if (values.length !== 2 || !values.every(Number.isFinite)) {
            console.error('[Electron] Messaggio DataGive non valido:', line);
            continue;
        }
        if (win && !win.isDestroyed()) {
            win.webContents.send('cpp:data', { rotateX: values[0], rotateY: values[1] });
        }
    }
}

function connectToCpp() {
    if (isShuttingDown) return;

    client = net.createConnection({ host: '127.0.0.1', port: 1234 });
    client.on('connect', () => {
        console.log('[Electron] Connesso con successo al motore C++!');
        incomingData = '';
        sendSocketStatus(true);
    });
    client.on('data', handleCppData);
    client.on('close', () => {
        sendSocketStatus(false);
        if (isShuttingDown) return;
        reconnectTimer = setTimeout(connectToCpp, 500);
    });
    client.on('error', (err) => {
        console.error('[Electron] Errore di connessione al C++:', err.message);
    });
}

function createWindow() {
    win = new BrowserWindow({
        width: 1200,
        height: 800,
        icon: path.join(__dirname, '../', 'img/', 'logo-only.png'),
        autoHideMenuBar: true,
        webPreferences: {
            preload: path.join(__dirname, 'preload.cjs'),
            nodeIntegration: false,
            contextIsolation: true
        }
    });

    win.webContents.on('did-finish-load', () => {
        sendSocketStatus(Boolean(client && !client.connecting && client.writable));
    });
    win.loadFile('public/index.html');
}

ipcMain.handle('settings:send', (_event, data) => sendSetting(data));
ipcMain.handle('socket:is-connected', () => Boolean(client && !client.connecting && client.writable));

app.whenReady().then(() => {
    connectToCpp();
    createWindow();

    app.on('activate', () => {
        if (BrowserWindow.getAllWindows().length === 0) createWindow();
    });
});

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') app.quit();
});

app.on('before-quit', () => {
    isShuttingDown = true;
    clearTimeout(reconnectTimer);
    if (client && !client.destroyed) client.destroy();
});