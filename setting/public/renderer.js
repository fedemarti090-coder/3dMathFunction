import katex from 'katex';

const startColor = document.querySelector('#color-start');
const endColor = document.querySelector('#color-end');
const gradient = document.querySelector('.color-gradient');
const functionInput = document.querySelector('#function-input');
const formulaPreview = document.querySelector('#formula-preview');

function renderFormula() {
    const formula = "f(x, y) =" + functionInput.value.trim();
    formulaPreview.replaceChildren();

    if (!formula) {
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

functionInput.addEventListener('input', renderFormula);
renderFormula();

function updateGradient() {
    gradient.style.setProperty('--start-color', startColor.value);
    gradient.style.setProperty('--end-color', endColor.value);
}

startColor.addEventListener('input', updateGradient);
endColor.addEventListener('input', updateGradient);
updateGradient();

function updateSlider(slider) {
    const output = document.querySelector(`#${slider.id}-value`);
    output.value = slider.id === 'alpha' ? Number(slider.value).toFixed(2) : slider.value;
}

for (const slider of document.querySelectorAll('.range-row input[type="range"], .alpha-row input[type="range"]')) {
    slider.addEventListener('input', () => updateSlider(slider));
    updateSlider(slider);
}

document.getElementById('reset-button').addEventListener('click', resetRotation);

function resetRotation() {
    for (const axis of ['x', 'y', 'z']) {
        const slider = document.getElementById(`rotation-${axis}`);
        slider.value = slider.min;
        slider.dispatchEvent(new Event('input', { bubbles: true }));
    }
}