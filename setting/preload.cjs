const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('electronAPI', {
    sendSetting: (data) => ipcRenderer.invoke('settings:send', data),
    isSocketConnected: () => ipcRenderer.invoke('socket:is-connected'),
    onSocketStatus: (callback) => {
        const listener = (_event, connected) => callback(connected);
        ipcRenderer.on('socket:status', listener);
        return () => ipcRenderer.removeListener('socket:status', listener);
    },
    onDataGive: (callback) => {
        const listener = (_event, data) => callback(data);
        ipcRenderer.on('cpp:data', listener);
        return () => ipcRenderer.removeListener('cpp:data', listener);
    }
});
