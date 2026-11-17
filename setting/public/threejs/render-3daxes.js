import * as THREE from 'three';

const canvas = document.getElementById('axes-preview-canvas');
const scene = new THREE.Scene();
scene.background = new THREE.Color(0x000000);

const camera = new THREE.PerspectiveCamera(42, 1, 0.1, 100);
camera.position.set(3.8, 3.1, 4.5);
camera.lookAt(0, 0, 0);

const renderer = new THREE.WebGLRenderer({
    canvas,
    antialias: true,
    powerPreference: 'low-power'
});
renderer.setPixelRatio(Math.min(window.devicePixelRatio || 1, 2));
renderer.outputColorSpace = THREE.SRGBColorSpace;

const axes = {
    x: {
        checkbox: document.getElementById('axis-x'),
        direction: new THREE.Vector3(1, 0, 0),
        color: 0xff4040
    },
    y: {
        checkbox: document.getElementById('axis-y'),
        direction: new THREE.Vector3(0, 1, 0),
        color: 0x40ff40
    },
    z: {
        checkbox: document.getElementById('axis-z'),
        direction: new THREE.Vector3(0, 0, 1),
        color: 0x4080ff
    }
};

const axisGroups = {};
const axisMaterials = {};

for (const [name, axis] of Object.entries(axes)) {
    const lineGeometry = new THREE.BufferGeometry().setFromPoints([
        axis.direction.clone().multiplyScalar(-1.8),
        axis.direction.clone().multiplyScalar(1.8)
    ]);
    const lineMaterial = new THREE.LineBasicMaterial({
        color: axis.color,
        transparent: true,
        depthTest: false
    });
    const line = new THREE.Line(lineGeometry, lineMaterial);
    line.renderOrder = 2;

    const arrow = new THREE.ArrowHelper(axis.direction, new THREE.Vector3(0, 0, 0), 1.8, axis.color, 0.28, 0.16);
    arrow.line.visible = false;
    arrow.line.material.depthTest = false;
    arrow.line.material.transparent = true;
    arrow.cone.material.depthTest = false;
    arrow.cone.material.transparent = true;
    arrow.renderOrder = 2;

    const group = new THREE.Group();
    group.add(line, arrow);
    axisMaterials[name] = [lineMaterial, arrow.line.material, arrow.cone.material];
    axisGroups[name] = group;
    scene.add(group);
}

function createPlane(rotation, color) {
    const plane = new THREE.Mesh(
        new THREE.PlaneGeometry(3.6, 3.6),
        new THREE.MeshBasicMaterial({
            color,
            side: THREE.DoubleSide,
            transparent: true,
            opacity: 0.22,
            depthWrite: false
        })
    );
    plane.rotation.set(...rotation);
    plane.renderOrder = 1;
    plane.visible = false;
    scene.add(plane);
    return plane;
}

const zeroSurfaces = document.getElementById('zero-surfaces');
const planes = [
    createPlane([0, Math.PI / 2, 0], 0xff4040),
    createPlane([Math.PI / 2, 0, 0], 0x40ff40),
    createPlane([0, 0, 0], 0x4080ff)
];

function render() {
    for (const [name, axis] of Object.entries(axes)) {
        const selected = axis.checkbox.checked;
        for (const material of axisMaterials[name]) {
            material.color.setHex(selected ? axis.color : 0x808080);
            material.opacity = selected ? 1 : 0.75;
        }
    }

    for (const plane of planes) {
        plane.visible = zeroSurfaces.checked;
    }

    renderer.render(scene, camera);
}

for (const axis of Object.values(axes)) {
    axis.checkbox.addEventListener('change', render);
}
zeroSurfaces.addEventListener('change', render);

function resizePreview() {
    const width = canvas.clientWidth;
    const height = canvas.clientHeight;
    if (width === 0 || height === 0) return;

    camera.aspect = width / height;
    camera.updateProjectionMatrix();
    renderer.setSize(width, height, false);
    render();
}

new ResizeObserver(resizePreview).observe(canvas);
resizePreview();
