var opts = {
    angle: -0.25,
        lineWidth: 0.2,
        radiusScale:0.9,
        pointer: {
            length: 0.6,
            strokeWidth: 0.05,
            color: '#000000'
        },
        staticLabels: {
            font: "10px sans-serif",
            labels: [0, 25, 50, 250],
            fractionDigits: 0
        },
        staticZones: [
            {strokeStyle: "#F03E3E", min: 0, max: 25},
            {strokeStyle: "#FFDD00", min: 25, max: 50},
            {strokeStyle: "#30B32D", min: 50, max: 250}
        ],
        limitMax: false,
        limitMin: false,
        highDpiSupport: true
};
var target = document.getElementById('gauge'); // your canvas element
var gauge = new Gauge(target).setOptions(opts); // create sexy gauge!
document.getElementById("weight").className = "weight";
gauge.setTextField(document.getElementById("weight"));
gauge.maxValue = 250; // set max gauge value
gauge.setMinValue(0);  // set min value
gauge.set(125);

const ssidElement = document.querySelector('#ssid')

fetch("/ssid", {method:"GET"})
    .then((request) => {return request.json()})
    .then((json) => {ssidElement.textContent=json.ssid})

setInterval(function ( ) {
    fetch("/weight", {method:"GET"})
        .then((request) => {return request.json()})
        .then((json) => {gauge.set(json.weight)})
}, 10000 );
