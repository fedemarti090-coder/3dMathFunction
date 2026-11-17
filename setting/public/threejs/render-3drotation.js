import * as THREE from 'three';

const scene = new THREE.Scene();
scene.background = new THREE.Color(0x000000);

const camera = new THREE.PerspectiveCamera(
    40,
    1,
    0.1,
    100
);
camera.position.set(3, 2.4, 4.5);
camera.lookAt(0, 0, 0);

const canvas = document.getElementById('rotation-preview-canvas');
const preview = canvas.parentElement;
const renderer = new THREE.WebGLRenderer({
    canvas,
    antialias: true,
    powerPreference: 'low-power'
});
renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
renderer.outputColorSpace = THREE.SRGBColorSpace;

const vertices = [
    [0, 1, 0],
    [-0.9, -0.75, 0.8],
    [0.9, -0.75, 0.8],
    [0, -0.75, -1]
];
const faces = [
    [0, 1, 2],
    [0, 2, 3],
    [0, 3, 1],
    [1, 3, 2]
];
const geometry = new THREE.BufferGeometry();
const positions = faces.flatMap(face => face.flatMap(index => vertices[index]));
geometry.setAttribute('position', new THREE.Float32BufferAttribute(positions, 3));
faces.forEach((face, index) => geometry.addGroup(index * 3, 3, index));
geometry.computeVertexNormals();

const materials = [
    new THREE.MeshBasicMaterial({ color: 0xff0000, side: THREE.DoubleSide }),
    new THREE.MeshBasicMaterial({ color: 0x00ff00, side: THREE.DoubleSide }),
    new THREE.MeshBasicMaterial({ color: 0x0000ff, side: THREE.DoubleSide }),
    new THREE.MeshBasicMaterial({ color: 0xffff00, side: THREE.DoubleSide })
];
const triangle = new THREE.Mesh(geometry, materials);
scene.add(triangle);

const rotationSliders = {
    x: document.getElementById('rotation-x'),
    y: document.getElementById('rotation-y'),
    z: document.getElementById('rotation-z')
};

function render() {
    triangle.rotation.set(
        THREE.MathUtils.degToRad(Number(rotationSliders.x.value)),
        THREE.MathUtils.degToRad(Number(rotationSliders.y.value)),
        THREE.MathUtils.degToRad(Number(rotationSliders.z.value))
    );
    renderer.render(scene, camera);
}

for (const slider of Object.values(rotationSliders)) {
    slider.addEventListener('input', render);
}

function resizePreview() {
    const width = preview.clientWidth;
    const height = preview.clientHeight;
    if (width === 0 || height === 0) return;

    camera.aspect = width / height;
    camera.updateProjectionMatrix();
    renderer.setSize(width, height, false);
    renderer.render(scene, camera);
}

new ResizeObserver(resizePreview).observe(preview);

render();
resizePreview();