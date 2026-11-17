import katex from 'katex';

const startColor = document.querySelector('#color-start');
const endColor = document.querySelector('#color-end');
const gradient = document.querySelector('.color-gradient');
const functionInput = document.querySelector('#function-input');
const formulaPreview = document.querySelector('#formula-preview');

const defaultValues = {
    'function-input': 'sin(x) + cos(y)',
    'domain-min': '-6.28',
    'domain-max': '6.28',
    'function-step': '0.1'
};

for (const [id, value] of Object.entries(defaultValues)) {
    const input = document.getElementById(id);
    if (!input.value) input.value = value;
}

function renderFormula() {
    const formula = "f(x, y) =" + functionInput.value.trim();
    formulaPreview.replaceChildren();

    if (!functionInput.value.trim()) {
        const placeholder = document.createElement('span');
        placeholder.className = 'formula-placeholder';
        placeholder.textContent = "L'anteprima della formula apparira qui";
        formulaPreview.append(placeholder);
        return;
    }

    katex.render(formula, formulaPreview, {
        displayMode: true,
        throwOnError: false,
        trust: false
    });
}

function colorToRgb(hexColor) {
    return {
        R: parseInt(hexColor.slice(1, 3), 16) / 255,
        G: parseInt(hexColor.slice(3, 5), 16) / 255,
        B: parseInt(hexColor.slice(5, 7), 16) / 255
    };
}

function buildDataGet() {
    return {
        function: functionInput.value.trim(),
        dominioMax: Number(document.getElementById('domain-max').value),
        dominioMin: Number(document.getElementById('domain-min').value),
        step: Number(document.getElementById('function-step').value),
        rotateX: Number(document.getElementById('rotation-x').value),
        rotateY: Number(document.getElementById('rotation-y').value),
        rotateZ: Number(document.getElementById('rotation-z').value),
        col1: colorToRgb(startColor.value),
        col2: colorToRgb(endColor.value),
        alpha: Number(document.getElementById('alpha').value),
        asseX: document.getElementById('axis-x').checked,
        asseY: document.getElementById('axis-y').checked,
        asseZ: document.getElementById('axis-z').checked,
        Superfici0: document.getElementById('zero-surfaces').checked
    };
}

async function sendCurrentSettings() {
    try {
        const sent = await window.electronAPI.sendSetting(buildDataGet());
        if (!sent) console.warn('[UI] Socket C++ non connesso: impostazioni non inviate.');
    } catch (error) {
        console.error('[UI] Errore invio impostazioni:', error);
    }
}

functionInput.addEventListener('input', renderFormula);
functionInput.addEventListener('input', () => {
    functionInput.classList.remove('input-error');
    functionInput.setAttribute('aria-invalid', 'false');
});
renderFormula();

function updateGradient() {
    gradient.style.setProperty('--start-color', startColor.value);
    gradient.style.setProperty('--end-color', endColor.value);
}

startColor.addEventListener('input', updateGradient);
endColor.addEventListener('input', updateGradient);
updateGradient();

for (const input of document.querySelectorAll('#color-start, #color-end, #alpha')) {
    input.addEventListener('input', sendCurrentSettings);
}

for (const input of document.querySelectorAll('#axis-x, #axis-y, #axis-z, #zero-surfaces')) {
    input.addEventListener('change', sendCurrentSettings);
}

function updateSlider(slider) {
    const output = document.querySelector(`#${slider.id}-value`);
    output.value = slider.id === 'alpha' ? Number(slider.value).toFixed(2) : slider.value;
}

for (const slider of document.querySelectorAll('.range-row input[type="range"], .alpha-row input[type="range"]')) {
    slider.addEventListener('input', () => updateSlider(slider));
    updateSlider(slider);
}

document.getElementById('reset-button').addEventListener('click', resetRotation);

let applyingCppRotation = false;
const rotationInputs = document.querySelectorAll('#rotation-x, #rotation-y, #rotation-z');
for (const input of rotationInputs) {
    input.addEventListener('input', () => {
        if (!applyingCppRotation) sendCurrentSettings();
    });
}

let graphSettingsTimer;
const graphInputs = document.querySelectorAll(
    '#function-input, #domain-min, #domain-max, #function-step'
);
for (const input of graphInputs) {
    input.addEventListener('input', () => {
        clearTimeout(graphSettingsTimer);
        graphSettingsTimer = setTimeout(sendCurrentSettings, 350);
    });
    input.addEventListener('change', () => {
        clearTimeout(graphSettingsTimer);
        sendCurrentSettings();
    });
}

for (const button of document.querySelectorAll('.examples-section .example-row button[data-function]')) {
    button.addEventListener('click', () => {
        functionInput.value = button.dataset.function;
        functionInput.dispatchEvent(new Event('input', { bubbles: true }));
        functionInput.dispatchEvent(new Event('change', { bubbles: true }));
    });
}

window.electronAPI.onDataGive(({ rotateX, rotateY }) => {
    const rotationX = document.getElementById('rotation-x');
    const rotationY = document.getElementById('rotation-y');
    rotationX.value = String(Math.max(Number(rotationX.min), Math.min(Number(rotationX.max), rotateX)));
    rotationY.value = String(Math.max(Number(rotationY.min), Math.min(Number(rotationY.max), rotateY)));
    applyingCppRotation = true;
    rotationX.dispatchEvent(new Event('input', { bubbles: true }));
    rotationY.dispatchEvent(new Event('input', { bubbles: true }));
    applyingCppRotation = false;
});

window.electronAPI.onFormulaStatus(({ valid }) => {
    functionInput.classList.toggle('input-error', !valid);
    functionInput.setAttribute('aria-invalid', String(!valid));
});

window.electronAPI.onSocketStatus((connected) => {
    if (connected) sendCurrentSettings();
});
window.electronAPI.isSocketConnected().then((connected) => {
    if (connected) sendCurrentSettings();
});

function resetRotation() {
    for (const axis of ['x', 'y', 'z']) {
        const slider = document.getElementById(`rotation-${axis}`);
        slider.value = slider.min;
        slider.dispatchEvent(new Event('input', { bubbles: true }));
    }
}